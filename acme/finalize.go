// finalize (RFC 8555 §7.4): the CSR arrives here, and issuance execs the
// yca CLI - the single owner of the CA store. The pipeline mirrors the
// manual flow: get nonce -> sign server --csr - (DER on stdin, CN on
// stdout) -> get server / get ca for the chain. One issuance at a time:
// the enrolled identity holds a single pending nonce, and the PKCS#11
// login is seconds - a mutex keeps the pipeline honest.
package main

import (
	"bytes"
	"context"
	"crypto/x509"
	"encoding/base64"
	"encoding/json"
	"fmt"
	"log"
	"net/http"
	"os/exec"
	"slices"
	"strings"
	"sync"
	"time"
)

// valid is passed to `yca sign --valid` verbatim when non-empty; the CA
// validates it ([5m, ee_valid_days]) - the daemon only requests.
type ycaRunner struct {
	bin, config, store, id, valid string
	mu                            sync.Mutex
}

func newYcaRunner(bin, config, store, id, valid string) *ycaRunner {
	return &ycaRunner{bin: bin, config: config, store: store, id: id,
		valid: valid}
}

// run execs one yca command; stderr goes to our log on failure.
func (y *ycaRunner) run(stdin []byte, args ...string) (string, error) {
	ctx, cancel := context.WithTimeout(context.Background(), 60*time.Second)
	defer cancel()
	base := []string{}
	if y.config != "" {
		base = append(base, "--config", y.config)
	}
	if y.store != "" {
		base = append(base, "--store", y.store)
	}
	cmd := exec.CommandContext(ctx, y.bin, append(base, args...)...)
	if stdin != nil {
		cmd.Stdin = bytes.NewReader(stdin)
	}
	var out, errb bytes.Buffer
	cmd.Stdout = &out
	cmd.Stderr = &errb
	if err := cmd.Run(); err != nil {
		return "", fmt.Errorf("yca %s: %w: %s", args[0], err,
			strings.TrimSpace(errb.String()))
	}
	return strings.TrimSpace(out.String()), nil
}

// selfCheck verifies the binary is runnable at startup.
func (y *ycaRunner) selfCheck() error {
	out, err := y.run(nil, "--version")
	if err != nil {
		return err
	}
	log.Printf("issuing via %q (%s), enrolled id %q", y.bin, out, y.id)
	return nil
}

// issue runs the CSR through the CA and returns the PEM chain (EE first).
func (y *ycaRunner) issue(csrDER []byte) (chain, cn string, err error) {
	y.mu.Lock()
	defer y.mu.Unlock()
	nonce, err := y.run(nil, "get", "nonce", "--id", y.id)
	if err != nil {
		return "", "", err
	}
	args := []string{"sign", "server", "--id", y.id,
		"--nonce", nonce, "--csr", "-"}
	if y.valid != "" {
		args = append(args, "--valid", y.valid)
	}
	cn, err = y.run(csrDER, args...)
	if err != nil {
		return "", "", err
	}
	ee, err := y.run(nil, "get", "server", "--cn", cn)
	if err != nil {
		return "", "", err
	}
	issuer, err := y.run(nil, "get", "ca", "--cn", "signing-ca")
	if err != nil {
		return "", "", err
	}
	return ee + "\n" + issuer + "\n", cn, nil
}

// revoke revokes the exact certificate by serial (uppercase hex).
func (y *ycaRunner) revoke(serialHex, reason string) error {
	y.mu.Lock()
	defer y.mu.Unlock()
	_, err := y.run(nil, "revoke", "server", "--serial", serialHex,
		"--reason", reason)
	return err
}

func (s *server) handleFinalize(w http.ResponseWriter, r *http.Request) {
	req, p := s.verifyJWS(r, false)
	if p != nil {
		s.writeProblem(w, p)
		return
	}
	order, authzs, p := s.loadOwnedOrder(req, r.PathValue("id"))
	if p != nil {
		s.writeProblem(w, p)
		return
	}
	if order.Status != "ready" {
		s.writeProblem(w, problem(http.StatusForbidden, "orderNotReady",
			"order status is "+order.Status))
		return
	}

	var payload struct {
		CSR string `json:"csr"`
	}
	if err := json.Unmarshal(req.payload, &payload); err != nil {
		s.writeProblem(w, problem(http.StatusBadRequest, "malformed",
			"finalize payload is not JSON"))
		return
	}
	der, err := base64.RawURLEncoding.DecodeString(payload.CSR)
	if err != nil {
		s.writeProblem(w, problem(http.StatusBadRequest, "badCSR",
			"csr is not base64url"))
		return
	}
	csr, err := x509.ParseCertificateRequest(der)
	if err != nil || csr.CheckSignature() != nil {
		s.writeProblem(w, problem(http.StatusBadRequest, "badCSR",
			"unparsable CSR or bad CSR signature"))
		return
	}
	// Only dNSName SANs are acceptable: the challenges validated dns
	// identifiers and nothing else, and the CA honors email/IP SANs on
	// its operator-trusted CSR path - any other SAN type here would ride
	// into the certificate unvalidated.
	if len(csr.IPAddresses) > 0 || len(csr.EmailAddresses) > 0 ||
		len(csr.URIs) > 0 {
		s.writeProblem(w, problem(http.StatusBadRequest, "badCSR",
			"CSR carries non-dns SANs"))
		return
	}
	// The CSR must request exactly the order's identifiers (RFC 8555 §7.4);
	// the CN, when present, must be one of them.
	names := map[string]bool{}
	for _, d := range csr.DNSNames {
		names[strings.ToLower(d)] = true
	}
	if cn := strings.ToLower(csr.Subject.CommonName); cn != "" {
		if !slices.Contains(order.Identifiers, cn) {
			s.writeProblem(w, problem(http.StatusBadRequest, "badCSR",
				"CSR CN is not among the order identifiers"))
			return
		}
		names[cn] = true
	}
	if !sameSet(names, order.Identifiers) {
		s.writeProblem(w, problem(http.StatusBadRequest, "badCSR",
			"CSR names do not match the order identifiers"))
		return
	}

	_ = s.db.SetOrderStatus(order.ID, "processing", "", "")
	chain, cn, err := s.yca.issue(der)
	if err != nil {
		log.Printf("order %s: issuance failed: %v", order.ID, err)
		perr, _ := json.Marshal(problem(http.StatusInternalServerError,
			"serverInternal", "issuance failed"))
		_ = s.db.SetOrderStatus(order.ID, "invalid", "", string(perr))
		s.writeProblem(w, problem(http.StatusInternalServerError,
			"serverInternal", "issuance failed"))
		return
	}
	cert := &Cert{
		ID:        newID(),
		OrderID:   order.ID,
		AccountID: req.account.ID,
		CN:        cn,
		ChainPEM:  chain,
	}
	if err := certMeta(cert); err != nil {
		log.Printf("order %s: issued chain unparsable: %v", order.ID, err)
		s.writeProblem(w, problem(http.StatusInternalServerError,
			"serverInternal", "issued chain unparsable"))
		return
	}
	if err := s.db.InsertCert(cert); err != nil {
		s.writeProblem(w, problem(http.StatusInternalServerError,
			"serverInternal", "certificate store"))
		return
	}
	_ = s.db.SetOrderStatus(order.ID, "valid", cert.ID, "")
	log.Printf("order %s: issued CN %q (cert %s)", order.ID, cn, cert.ID)

	order, _ = s.db.OrderByID(order.ID)
	w.Header().Set("Location", s.url("/acme/order/"+order.ID))
	s.writeJSON(w, http.StatusOK, s.orderJSON(order, authzs))
}

func (s *server) handleCert(w http.ResponseWriter, r *http.Request) {
	req, p := s.verifyJWS(r, false)
	if p != nil {
		s.writeProblem(w, p)
		return
	}
	cert, err := s.db.CertByID(r.PathValue("id"))
	if err != nil {
		s.writeProblem(w, problem(http.StatusInternalServerError,
			"serverInternal", "certificate lookup"))
		return
	}
	if cert == nil || cert.AccountID != req.account.ID {
		s.writeProblem(w, problem(http.StatusNotFound, "malformed",
			"no such certificate"))
		return
	}
	nonce, err := s.db.IssueNonce()
	if err != nil {
		http.Error(w, "internal error", http.StatusInternalServerError)
		return
	}
	w.Header().Set("Replay-Nonce", nonce)
	w.Header().Set("Content-Type", "application/pem-certificate-chain")
	_, _ = w.Write([]byte(cert.ChainPEM))
}

func sameSet(got map[string]bool, want []string) bool {
	if len(got) != len(want) {
		return false
	}
	for _, w := range want {
		if !got[w] {
			return false
		}
	}
	return true
}

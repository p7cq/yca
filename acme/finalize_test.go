package main

import (
	"crypto/ecdsa"
	"crypto/rand"
	"crypto/x509"
	"crypto/x509/pkix"
	"encoding/base64"
	"encoding/json"
	"encoding/pem"
	"math/big"
	"net/http"
	"os"
	"path/filepath"
	"strings"
	"testing"
	"time"
)

// stub mimics the yca CLI pipeline (get nonce / sign / get server / get
// ca / revoke) with a REAL issuer+leaf pair, so the exec plumbing AND the
// leaf-derived metadata (serial, ARI id) are testable without a CA store.
type stub struct {
	bin     string
	leaf    *x509.Certificate
	leafKey *ecdsa.PrivateKey
	leafPEM string
	caPEM   string
	args    string // file collecting one line per invocation
}

func newStub(t *testing.T, cn string) *stub {
	t.Helper()
	dir := t.TempDir()
	caKey, _ := generateKey()
	caTmpl := &x509.Certificate{
		SerialNumber: big.NewInt(1),
		Subject:      pkix.Name{CommonName: "stub CA"},
		NotBefore:    time.Now().Add(-time.Hour),
		NotAfter:     time.Now().Add(90 * 24 * time.Hour),
		IsCA:         true, BasicConstraintsValid: true,
		SubjectKeyId: []byte{1, 2, 3, 4},
	}
	caDER, err := x509.CreateCertificate(rand.Reader, caTmpl, caTmpl,
		caKey.Public(), caKey)
	if err != nil {
		t.Fatal(err)
	}
	caCert, _ := x509.ParseCertificate(caDER)

	leafKey, _ := generateKey()
	serial, _ := rand.Int(rand.Reader, big.NewInt(1).Lsh(big.NewInt(1), 127))
	leafTmpl := &x509.Certificate{
		SerialNumber: serial,
		Subject:      pkix.Name{CommonName: cn},
		DNSNames:     []string{cn},
		NotBefore:    time.Now().Add(-time.Hour),
		NotAfter:     time.Now().Add(90 * 24 * time.Hour),
	}
	leafDER, err := x509.CreateCertificate(rand.Reader, leafTmpl, caCert,
		leafKey.Public(), caKey)
	if err != nil {
		t.Fatal(err)
	}
	leaf, _ := x509.ParseCertificate(leafDER)

	pemOf := func(der []byte) string {
		return string(pem.EncodeToMemory(
			&pem.Block{Type: "CERTIFICATE", Bytes: der}))
	}
	s := &stub{
		bin:     filepath.Join(dir, "yca-stub"),
		leaf:    leaf,
		leafKey: leafKey,
		leafPEM: pemOf(leafDER),
		caPEM:   pemOf(caDER),
		args:    filepath.Join(dir, "args.log"),
	}
	if err := os.WriteFile(filepath.Join(dir, "leaf.pem"),
		[]byte(s.leafPEM), 0o644); err != nil {
		t.Fatal(err)
	}
	if err := os.WriteFile(filepath.Join(dir, "ca.pem"),
		[]byte(s.caPEM), 0o644); err != nil {
		t.Fatal(err)
	}
	script := `#!/bin/sh
dir="$(dirname "$0")"
echo "$@" >> "$dir/args.log"
case "$1" in
  --version) echo "yca version 0.0.0-stub"; exit 0;;
  get)
    case "$2" in
      nonce)  echo "deadbeefdeadbeef"; exit 0;;
      server) cat "$dir/leaf.pem"; exit 0;;
      ca)     cat "$dir/ca.pem"; exit 0;;
    esac;;
  sign)
    cat > /dev/null              # consume the CSR from stdin
    echo "` + cn + `"; exit 0;;  # the CN, like the real sign does
  revoke)
    # Second revoke of anything: behave like the CA (already revoked).
    if [ -f "$dir/revoked" ]; then
      echo "no active server certificate with serial x" >&2; exit 1
    fi
    touch "$dir/revoked"; exit 0;;
esac
echo "stub: unexpected args: $*" >&2
exit 1
`
	if err := os.WriteFile(s.bin, []byte(script), 0o755); err != nil {
		t.Fatal(err)
	}
	return s
}

// stubYca keeps the old shape used by the finalize tests.
func stubYca(t *testing.T) string { return newStub(t, "localhost").bin }

// failingYca refuses to sign (e.g. renewal window not reached).
func failingYca(t *testing.T) string {
	t.Helper()
	script := `#!/bin/sh
case "$1" in
  --version) echo "yca version 0.0.0-stub"; exit 0;;
  get) [ "$2" = nonce ] && { echo "deadbeef"; exit 0; };;
esac
echo "an active server certificate for CN 'localhost' already exists" >&2
exit 1
`
	path := filepath.Join(t.TempDir(), "yca-fail")
	if err := os.WriteFile(path, []byte(script), 0o755); err != nil {
		t.Fatal(err)
	}
	return path
}

// csrFor builds a real (signed) CSR for the given DNS names.
func csrFor(t *testing.T, cn string, dns []string) []byte {
	t.Helper()
	key, err := generateKey()
	if err != nil {
		t.Fatal(err)
	}
	tmpl := &x509.CertificateRequest{DNSNames: dns}
	if cn != "" {
		tmpl.Subject = pkix.Name{CommonName: cn}
	}
	der, err := x509.CreateCertificateRequest(rand.Reader, tmpl, key)
	if err != nil {
		t.Fatal(err)
	}
	return der
}

func finalizeBody(t *testing.T, der []byte) []byte {
	t.Helper()
	body, err := json.Marshal(map[string]string{
		"csr": base64.RawURLEncoding.EncodeToString(der)})
	if err != nil {
		t.Fatal(err)
	}
	return body
}

func TestFinalizeIssues(t *testing.T) {
	e := newTestEnv(t)
	e.register()
	st := newStub(t, "localhost")
	e.s.yca = newYcaRunner(st.bin, "", "", "acme")
	orderPath := runChallenge(t, e, false)

	_, order := e.post(orderPath, nil, e.kid, "")
	fin := e.path(order["finalize"].(string))
	resp, v := e.post(fin, finalizeBody(t, csrFor(t, "", []string{"localhost"})),
		e.kid, "")
	if resp.StatusCode != http.StatusOK || v["status"] != "valid" {
		t.Fatalf("finalize: %d %v", resp.StatusCode, v)
	}

	// The certificate URL serves the chain: EE first, then the issuer.
	certURL, _ := v["certificate"].(string)
	if certURL == "" {
		t.Fatal("no certificate URL on the valid order")
	}
	resp, body := e.post(e.path(certURL), nil, e.kid, "")
	if resp.Header.Get("Content-Type") != "application/pem-certificate-chain" {
		t.Fatalf("cert content-type: %q", resp.Header.Get("Content-Type"))
	}
	chain := body["pem"].(string)
	if !strings.Contains(chain, strings.TrimSpace(st.leafPEM)) ||
		!strings.Contains(chain, strings.TrimSpace(st.caPEM)) ||
		strings.Index(chain, strings.TrimSpace(st.leafPEM)) >
			strings.Index(chain, strings.TrimSpace(st.caPEM)) {
		t.Fatalf("chain wrong or out of order:\n%s", chain)
	}
}

func TestFinalizeNameMismatch(t *testing.T) {
	e := newTestEnv(t)
	e.register()
	e.s.yca = newYcaRunner(stubYca(t), "", "", "acme")
	orderPath := runChallenge(t, e, false)
	_, order := e.post(orderPath, nil, e.kid, "")
	fin := e.path(order["finalize"].(string))

	cases := [][]string{
		{"other.test.ca"},              // different name
		{"localhost", "extra.test.ca"}, // superset
		{},                             // empty
	}
	for _, dns := range cases {
		resp, v := e.post(fin, finalizeBody(t, csrFor(t, "", dns)), e.kid, "")
		if problemType(v) != "badCSR" {
			t.Errorf("CSR %v: got %d %v, want badCSR", dns, resp.StatusCode, v)
		}
	}
	// CN counting as a name: CN=localhost with no SANs is exactly the set.
	resp, v := e.post(fin, finalizeBody(t, csrFor(t, "localhost", nil)), e.kid, "")
	if resp.StatusCode != http.StatusOK || v["status"] != "valid" {
		t.Fatalf("CN-only CSR: %d %v", resp.StatusCode, v)
	}
}

func TestFinalizeBeforeReady(t *testing.T) {
	e := newTestEnv(t)
	e.register()
	e.s.yca = newYcaRunner(stubYca(t), "", "", "acme")
	order, _ := e.order("localhost") // pending, never validated
	fin := e.path(order["finalize"].(string))
	resp, v := e.post(fin, finalizeBody(t, csrFor(t, "", []string{"localhost"})),
		e.kid, "")
	if resp.StatusCode != http.StatusForbidden || problemType(v) != "orderNotReady" {
		t.Fatalf("finalize on pending order: %d %v", resp.StatusCode, v)
	}
}

func TestFinalizeIssuanceFailure(t *testing.T) {
	e := newTestEnv(t)
	e.register()
	e.s.yca = newYcaRunner(failingYca(t), "", "", "acme")
	orderPath := runChallenge(t, e, false)
	_, order := e.post(orderPath, nil, e.kid, "")
	fin := e.path(order["finalize"].(string))
	resp, v := e.post(fin, finalizeBody(t, csrFor(t, "", []string{"localhost"})),
		e.kid, "")
	if resp.StatusCode != http.StatusInternalServerError {
		t.Fatalf("failing issuance: %d %v", resp.StatusCode, v)
	}
	_, o := e.post(orderPath, nil, e.kid, "")
	if o["status"] != "invalid" {
		t.Fatalf("order after failed issuance: %v", o)
	}
}

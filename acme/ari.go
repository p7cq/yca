// ACME Renewal Information (RFC 9773): unauthenticated GET keyed by
// base64url(AKI keyIdentifier) "." base64url(DER serial content octets).
// The suggested window mirrors the CA's renewal-window policy, so clients
// renew exactly when the CA will accept a successor.
package main

import (
	"crypto/x509"
	"encoding/base64"
	"encoding/pem"
	"errors"
	"fmt"
	"math/big"
	"net/http"
	"strconv"
	"strings"
	"time"
)

// Mirrors app::renew_window_pct in src/app.h - both changed together. The
// window opens when renewPct% of the lifetime remains (the CA starts
// accepting a successor) and closes at closePct% (headroom before expiry).
const (
	renewPct = 33
	closePct = 10
)

// Spread for a certificate that must be replaced at once. Issuance is
// serialized behind a single CA (one PKCS#11 login per exec), so a fleet
// arriving together only queues; clients randomize inside the window
// instead. Revocation of one certificate is urgent but small, hence the
// short default; an operator accelerating a whole issuer sizes that case
// with `yca-acme ari accelerate --window`.
const revokedWindow = time.Hour

// derIntBytes returns the DER INTEGER content octets of a serial: big.Int
// bytes with a leading 0x00 when the high bit is set (two's complement).
func derIntBytes(n *big.Int) []byte {
	b := n.Bytes()
	if len(b) == 0 {
		return []byte{0}
	}
	if b[0]&0x80 != 0 {
		return append([]byte{0}, b...)
	}
	return b
}

// ariID computes the RFC 9773 CertID of a certificate.
func ariID(cert *x509.Certificate) (string, error) {
	if len(cert.AuthorityKeyId) == 0 {
		return "", errors.New("certificate has no AKI extension")
	}
	return base64.RawURLEncoding.EncodeToString(cert.AuthorityKeyId) + "." +
		base64.RawURLEncoding.EncodeToString(derIntBytes(cert.SerialNumber)), nil
}

// leafOf parses the first certificate of a PEM chain.
func leafOf(chainPEM string) (*x509.Certificate, error) {
	block, _ := pem.Decode([]byte(chainPEM))
	if block == nil || block.Type != "CERTIFICATE" {
		return nil, errors.New("chain does not start with a PEM certificate")
	}
	return x509.ParseCertificate(block.Bytes)
}

// replaceNow reports the window to suggest when a certificate should be
// replaced at once rather than at its policy age, and how long the fleet
// has to get there. Two things call for it: the certificate is revoked, or
// an operator accelerated everything its issuer signed (a CA compromise,
// see docs/ca-rotation.md). Callers spread themselves across the window,
// which matters because issuance is serialized behind one CA.
func (s *server) replaceNow(cert *Cert) (time.Duration, bool) {
	if !cert.RevokedAt.IsZero() {
		return revokedWindow, true
	}
	leaf, err := leafOf(cert.ChainPEM)
	if err != nil {
		return 0, false
	}
	window, on, err := s.db.AccelWindow(leaf.Issuer.CommonName)
	if err != nil || !on {
		return 0, false
	}
	return window, true
}

func (s *server) handleRenewalInfo(w http.ResponseWriter, r *http.Request) {
	cert, err := s.db.CertByARI(r.PathValue("id"))
	if err != nil {
		http.Error(w, "internal error", http.StatusInternalServerError)
		return
	}
	if cert == nil || cert.ARIID == "" {
		http.Error(w, "unknown certificate", http.StatusNotFound)
		return
	}

	window, now := s.replaceNow(cert)
	var start, end time.Time
	if now {
		// Already open, so a client renews as soon as it reads this; it
		// picks a uniformly random moment up to `end` (RFC 9773 4.2),
		// which is what keeps a whole fleet from arriving at once.
		start = time.Now()
		end = start.Add(window)
	} else {
		life := cert.NotAfter.Sub(cert.NotBefore)
		start = cert.NotAfter.Add(-life * renewPct / 100)
		end = cert.NotAfter.Add(-life * closePct / 100)
	}

	w.Header().Set("Content-Type", "application/json")
	// Polling cadence hint (RFC 9773 4.3.1: the server sets this from how
	// fast it needs the fleet to move): a quarter of the window, clamped
	// to [1h, 24h] normally and to [10m, 1h] while replacing at once.
	retry := time.Until(end) / 4
	if now {
		retry = min(max(retry, 10*time.Minute), time.Hour)
	} else {
		retry = min(max(retry, time.Hour), 24*time.Hour)
	}
	w.Header().Set("Retry-After", strconv.Itoa(int(retry.Seconds())))
	fmt.Fprintf(w, `{"suggestedWindow":{"start":%q,"end":%q}}`+"\n",
		start.UTC().Format(time.RFC3339), end.UTC().Format(time.RFC3339))
}

// certMeta fills a Cert's leaf-derived columns (serial, ARI id, validity).
func certMeta(c *Cert) error {
	leaf, err := leafOf(c.ChainPEM)
	if err != nil {
		return err
	}
	c.Serial = strings.ToUpper(leaf.SerialNumber.Text(16))
	c.NotBefore, c.NotAfter = leaf.NotBefore, leaf.NotAfter
	if id, err := ariID(leaf); err == nil {
		c.ARIID = id // no AKI = no ARI entry; not fatal
	}
	return nil
}

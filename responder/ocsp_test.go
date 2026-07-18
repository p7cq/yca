package main

import (
	"crypto"
	"crypto/ecdsa"
	"crypto/elliptic"
	"crypto/rand"
	"crypto/x509"
	"crypto/x509/pkix"
	"math/big"
	"testing"
	"time"

	"golang.org/x/crypto/ocsp"
)

func TestSerialHex(t *testing.T) {
	cases := []struct {
		in   []byte
		want string
	}{
		{[]byte{0xB3, 0x5C, 0x03, 0xBE}, "B35C03BE"},
		{[]byte{0x00, 0xAB, 0xCD}, "ABCD"}, // minimal encoding: pad stripped
		{[]byte{0x01}, "01"},
		{nil, ""}, // zero serial never matches a row -> unknown
	}
	for _, c := range cases {
		if got := serialHex(new(big.Int).SetBytes(c.in)); got != c.want {
			t.Errorf("serialHex(%x) = %q, want %q", c.in, got, c.want)
		}
	}
}

func TestFillStatus(t *testing.T) {
	var tmpl ocsp.Response

	fillStatus(&tmpl, CertStatus{}, false)
	if tmpl.Status != ocsp.Unknown {
		t.Errorf("absent -> %d, want Unknown", tmpl.Status)
	}

	fillStatus(&tmpl, CertStatus{Status: "active"}, true)
	if tmpl.Status != ocsp.Good {
		t.Errorf("active -> %d, want Good", tmpl.Status)
	}

	fillStatus(&tmpl, CertStatus{Status: "revoked", RevokedAt: 1751500000,
		Reason: 1}, true)
	if tmpl.Status != ocsp.Revoked || tmpl.RevocationReason != 1 ||
		!tmpl.RevokedAt.Equal(time.Unix(1751500000, 0).UTC()) {
		t.Errorf("revoked -> %+v", tmpl)
	}
}

// testCA builds a self-signed CA and a leaf issued by it.
func testCA(t *testing.T, cn string) (*x509.Certificate, *x509.Certificate) {
	t.Helper()
	key, err := ecdsa.GenerateKey(elliptic.P256(), rand.Reader)
	if err != nil {
		t.Fatal(err)
	}
	caTmpl := &x509.Certificate{
		SerialNumber:          big.NewInt(1),
		Subject:               pkix.Name{CommonName: cn},
		NotBefore:             time.Now().Add(-time.Hour),
		NotAfter:              time.Now().Add(time.Hour),
		IsCA:                  true,
		BasicConstraintsValid: true,
		KeyUsage:              x509.KeyUsageCertSign,
	}
	caDER, err := x509.CreateCertificate(rand.Reader, caTmpl, caTmpl,
		&key.PublicKey, key)
	if err != nil {
		t.Fatal(err)
	}
	ca, err := x509.ParseCertificate(caDER)
	if err != nil {
		t.Fatal(err)
	}

	leafKey, err := ecdsa.GenerateKey(elliptic.P256(), rand.Reader)
	if err != nil {
		t.Fatal(err)
	}
	leafTmpl := &x509.Certificate{
		SerialNumber: big.NewInt(0xB35C),
		Subject:      pkix.Name{CommonName: cn + " leaf"},
		NotBefore:    time.Now().Add(-time.Hour),
		NotAfter:     time.Now().Add(time.Hour),
	}
	leafDER, err := x509.CreateCertificate(rand.Reader, leafTmpl, ca,
		&leafKey.PublicKey, key)
	if err != nil {
		t.Fatal(err)
	}
	leaf, err := x509.ParseCertificate(leafDER)
	if err != nil {
		t.Fatal(err)
	}
	return ca, leaf
}

// TestAuthorized exercises the CertID issuer matching against real OCSP
// requests (as produced by ocsp.CreateRequest, the client side).
func TestAuthorized(t *testing.T) {
	issuer, leaf := testCA(t, "UT CA")
	other, otherLeaf := testCA(t, "Other CA")

	h, err := newOCSPHandler(&Store{Issuer: issuer}, time.Hour)
	if err != nil {
		t.Fatal(err)
	}

	for _, hash := range []crypto.Hash{crypto.SHA1, crypto.SHA256} {
		der, err := ocsp.CreateRequest(leaf, issuer,
			&ocsp.RequestOptions{Hash: hash})
		if err != nil {
			t.Fatal(err)
		}
		req, err := ocsp.ParseRequest(der)
		if err != nil {
			t.Fatal(err)
		}
		if !h.authorized(req) {
			t.Errorf("%v request for own issuer not authorized", hash)
		}
	}

	der, err := ocsp.CreateRequest(otherLeaf, other, nil)
	if err != nil {
		t.Fatal(err)
	}
	req, err := ocsp.ParseRequest(der)
	if err != nil {
		t.Fatal(err)
	}
	if h.authorized(req) {
		t.Error("request for a foreign issuer authorized")
	}
}

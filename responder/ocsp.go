package main

import (
	"bytes"
	"crypto"
	_ "crypto/sha1" // register the CertID hash algorithms
	_ "crypto/sha256"
	"crypto/x509"
	"crypto/x509/pkix"
	"encoding/asn1"
	"encoding/base64"
	"encoding/hex"
	"fmt"
	"io"
	"log"
	"math/big"
	"net/http"
	"strings"
	"time"

	"golang.org/x/crypto/ocsp"
)

// A CertID request is ~100 bytes; anything bigger is not an OCSP request.
const maxRequestBytes = 4096

// Canned unsigned OCSPResponse errors (RFC 6960 §4.2.1): SEQUENCE holding
// only responseStatus. Error responses are never signed and never cached.
var (
	respMalformed    = []byte{0x30, 0x03, 0x0a, 0x01, 0x01} // malformedRequest
	respInternal     = []byte{0x30, 0x03, 0x0a, 0x01, 0x02} // internalError
	respUnauthorized = []byte{0x30, 0x03, 0x0a, 0x01, 0x06} // unauthorized
)

type issuerHash struct{ name, key []byte }

// ocspHandler answers OCSP requests (POST body / GET base64-in-path, RFC
// 6960 A.1) from the store, signing with the delegated responder key.
type ocspHandler struct {
	store    *Store
	validity time.Duration
	// Precomputed CertID hashes of the one signing CA (single-issuer,
	// phase 1): requests must match or are answered `unauthorized`.
	issuerHashes map[crypto.Hash]issuerHash
}

func newOCSPHandler(store *Store, validity time.Duration) (*ocspHandler, error) {
	// The CertID issuerKeyHash is over the subjectPublicKey BIT STRING
	// contents, not the whole SPKI - unwrap it.
	var spki struct {
		Algorithm        pkix.AlgorithmIdentifier
		SubjectPublicKey asn1.BitString
	}
	if _, err := asn1.Unmarshal(store.Issuer.RawSubjectPublicKeyInfo, &spki); err != nil {
		return nil, fmt.Errorf("issuer SPKI: %w", err)
	}
	h := &ocspHandler{
		store:        store,
		validity:     validity,
		issuerHashes: make(map[crypto.Hash]issuerHash),
	}
	for _, alg := range []crypto.Hash{crypto.SHA1, crypto.SHA256} {
		nh := alg.New()
		nh.Write(store.Issuer.RawSubject)
		kh := alg.New()
		kh.Write(spki.SubjectPublicKey.Bytes)
		h.issuerHashes[alg] = issuerHash{name: nh.Sum(nil), key: kh.Sum(nil)}
	}
	return h, nil
}

func (h *ocspHandler) ServeHTTP(w http.ResponseWriter, r *http.Request) {
	der, ok := requestDER(w, r)
	if !ok {
		replyError(w, respMalformed)
		return
	}
	req, err := ocsp.ParseRequest(der)
	if err != nil {
		replyError(w, respMalformed)
		return
	}
	if !h.authorized(req) {
		replyError(w, respUnauthorized)
		return
	}

	cs, found, err := h.store.Lookup(serialHex(req.SerialNumber))
	if err != nil {
		log.Printf("lookup %s: %v", serialHex(req.SerialNumber), err)
		replyError(w, respInternal)
		return
	}

	creds := h.store.Creds()
	now := time.Now().UTC()
	tmpl := ocsp.Response{
		SerialNumber:       req.SerialNumber,
		ThisUpdate:         now,
		NextUpdate:         now.Add(h.validity),
		IssuerHash:         req.HashAlgorithm, // mirror the request's CertID
		SignatureAlgorithm: x509.ECDSAWithSHA256,
		// Delegated model: embed the responder certificate so verifiers can
		// build the chain (CreateResponse embeds it only when set here).
		Certificate: creds.Cert,
	}
	fillStatus(&tmpl, cs, found)
	resp, err := ocsp.CreateResponse(h.store.Issuer, creds.Cert, tmpl, creds.Key)
	if err != nil {
		log.Printf("sign response: %v", err)
		replyError(w, respInternal)
		return
	}
	w.Header().Set("Content-Type", "application/ocsp-response")
	w.Header().Set("Cache-Control",
		fmt.Sprintf("public, max-age=%d", int(h.validity.Seconds())))
	if _, err := w.Write(resp); err != nil {
		log.Printf("write response: %v", err)
	}
}

// authorized reports whether the request's CertID names our issuer.
func (h *ocspHandler) authorized(req *ocsp.Request) bool {
	hs, ok := h.issuerHashes[req.HashAlgorithm]
	return ok && bytes.Equal(req.IssuerNameHash, hs.name) &&
		bytes.Equal(req.IssuerKeyHash, hs.key)
}

// fillStatus maps a cert_index row to the OCSP certificate status:
// active -> good (expiry is the client's check),
// revoked -> revoked with time and reason, absent -> unknown.
func fillStatus(tmpl *ocsp.Response, cs CertStatus, found bool) {
	switch {
	case !found:
		tmpl.Status = ocsp.Unknown
	case cs.Status == "revoked":
		tmpl.Status = ocsp.Revoked
		tmpl.RevokedAt = time.Unix(cs.RevokedAt, 0).UTC()
		tmpl.RevocationReason = cs.Reason
	default:
		tmpl.Status = ocsp.Good
	}
}

// serialHex normalizes a serial number to the store contract: uppercase hex
// of the minimal big-endian encoding.
func serialHex(sn *big.Int) string {
	return strings.ToUpper(hex.EncodeToString(sn.Bytes()))
}

// requestDER extracts the DER OCSPRequest: the POST body, or on GET the
// path remainder as standard base64 (the mux already percent-decodes; the
// {b64...} wildcard keeps unencoded '/' inside the value too).
func requestDER(w http.ResponseWriter, r *http.Request) ([]byte, bool) {
	if r.Method == http.MethodPost {
		der, err := io.ReadAll(http.MaxBytesReader(w, r.Body, maxRequestBytes))
		return der, err == nil && len(der) > 0
	}
	b64 := r.PathValue("b64")
	if len(b64) > maxRequestBytes {
		return nil, false
	}
	der, err := base64.StdEncoding.DecodeString(b64)
	return der, err == nil && len(der) > 0
}

func replyError(w http.ResponseWriter, resp []byte) {
	w.Header().Set("Content-Type", "application/ocsp-response")
	w.Header().Set("Cache-Control", "no-cache") // errors are never cacheable
	if _, err := w.Write(resp); err != nil {
		log.Printf("write response: %v", err)
	}
}

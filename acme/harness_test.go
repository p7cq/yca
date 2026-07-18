package main

import (
	"bytes"
	"context"
	"crypto"
	"crypto/ecdsa"
	"crypto/elliptic"
	"crypto/rand"
	"encoding/base64"
	"encoding/json"
	"errors"
	"io"
	"net/http"
	"net/http/httptest"
	"path/filepath"
	"testing"

	jose "github.com/go-jose/go-jose/v4"
)

func generateKey() (*ecdsa.PrivateKey, error) {
	return ecdsa.GenerateKey(elliptic.P256(), rand.Reader)
}

// noTXT is the default dns-01 lookup in tests: resolves nothing.
func noTXT(context.Context, string) ([]string, error) {
	return nil, errors.New("no TXT records in this test")
}

// testEnv is a running yca-acme with its own state db, plus one provisioned
// EAB credential and an ACME client key.
type testEnv struct {
	t   *testing.T
	s   *server
	ts  *httptest.Server
	key *ecdsa.PrivateKey // ACME account key
	kid string            // account URL after register()

	eabKid  string
	eabHMAC []byte
}

func newTestEnv(t *testing.T) *testEnv {
	t.Helper()
	db, err := OpenDB(filepath.Join(t.TempDir(), "acme.db"))
	if err != nil {
		t.Fatal(err)
	}
	t.Cleanup(func() { db.Close() })

	s := &server{db: db, http01: &http01Validator{port: 80},
		dns01: &dns01Validator{lookupTXT: noTXT}}
	mux := http.NewServeMux()
	mux.HandleFunc("GET /acme/directory", s.handleDirectory)
	mux.HandleFunc("HEAD /acme/new-nonce", s.handleNewNonce)
	mux.HandleFunc("GET /acme/new-nonce", s.handleNewNonce)
	mux.HandleFunc("POST /acme/new-account", s.handleNewAccount)
	mux.HandleFunc("POST /acme/acct/{id}", s.handleAccount)
	mux.HandleFunc("POST /acme/new-order", s.handleNewOrder)
	mux.HandleFunc("POST /acme/order/{id}", s.handleOrder)
	mux.HandleFunc("POST /acme/order/{id}/finalize", s.handleFinalize)
	mux.HandleFunc("POST /acme/authz/{id}", s.handleAuthz)
	mux.HandleFunc("POST /acme/chall/{id}", s.handleChallenge)
	mux.HandleFunc("POST /acme/cert/{id}", s.handleCert)
	mux.HandleFunc("POST /acme/revoke-cert", s.handleRevokeCert)
	mux.HandleFunc("POST /acme/key-change", s.handleKeyChange)
	mux.HandleFunc("GET /acme/renewal-info/{id}", s.handleRenewalInfo)
	ts := httptest.NewServer(mux)
	t.Cleanup(ts.Close)
	s.baseURL = ts.URL

	key, err := generateKey()
	if err != nil {
		t.Fatal(err)
	}
	env := &testEnv{t: t, s: s, ts: ts, key: key,
		eabKid: newID(), eabHMAC: make([]byte, 32)}
	if _, err := rand.Read(env.eabHMAC); err != nil {
		t.Fatal(err)
	}
	if err := db.InsertEAB(env.eabKid, env.eabHMAC, "localhost,*.test.ca"); err != nil {
		t.Fatal(err)
	}
	return env
}

func (e *testEnv) nonce() string {
	e.t.Helper()
	resp, err := http.Head(e.ts.URL + "/acme/new-nonce")
	if err != nil {
		e.t.Fatal(err)
	}
	resp.Body.Close()
	n := resp.Header.Get("Replay-Nonce")
	if n == "" {
		e.t.Fatal("no Replay-Nonce header")
	}
	return n
}

type fixedNonce string

func (f fixedNonce) Nonce() (string, error) { return string(f), nil }

// signWith produces the ACME JWS for `payload` bound to path, signed by
// an arbitrary key. kid=="" embeds the JWK (newAccount / by-cert-key
// style); otherwise the kid header references the account URL. nonce==""
// fetches a fresh one.
func (e *testEnv) signWith(key any, path string, payload []byte,
	kid, nonce string) string {
	e.t.Helper()
	if payload == nil { // POST-as-GET: empty payload, not absent
		payload = []byte{}
	}
	if nonce == "" {
		nonce = e.nonce()
	}
	var sk jose.SigningKey
	opts := &jose.SignerOptions{NonceSource: fixedNonce(nonce)}
	opts.WithHeader(jose.HeaderKey("url"), e.ts.URL+path)
	if kid == "" {
		sk = jose.SigningKey{Algorithm: jose.ES256, Key: key}
		opts.EmbedJWK = true
	} else {
		sk = jose.SigningKey{Algorithm: jose.ES256,
			Key: jose.JSONWebKey{Key: key, KeyID: kid}}
	}
	signer, err := jose.NewSigner(sk, opts)
	if err != nil {
		e.t.Fatal(err)
	}
	jws, err := signer.Sign(payload)
	if err != nil {
		e.t.Fatal(err)
	}
	return jws.FullSerialize()
}

func (e *testEnv) sign(path string, payload []byte, kid, nonce string) string {
	return e.signWith(e.key, path, payload, kid, nonce)
}

// post sends a signed ACME POST and decodes the JSON body.
func (e *testEnv) post(path string, payload []byte, kid, nonce string) (*http.Response, map[string]any) {
	e.t.Helper()
	body := e.sign(path, payload, kid, nonce)
	resp, err := http.Post(e.ts.URL+path, "application/jose+json",
		bytes.NewReader([]byte(body)))
	if err != nil {
		e.t.Fatal(err)
	}
	defer resp.Body.Close()
	raw, _ := io.ReadAll(resp.Body)
	var v map[string]any
	if len(raw) > 0 && resp.Header.Get("Content-Type") != "application/pem-certificate-chain" {
		if err := json.Unmarshal(raw, &v); err != nil {
			e.t.Fatalf("non-JSON response (%d): %s", resp.StatusCode, raw)
		}
	} else {
		v = map[string]any{"pem": string(raw)}
	}
	return resp, v
}

// eab builds the inner EAB JWS over the account public key.
func (e *testEnv) eab() json.RawMessage {
	e.t.Helper()
	pubJWK := jose.JSONWebKey{Key: e.key.Public()}
	payload, err := pubJWK.MarshalJSON()
	if err != nil {
		e.t.Fatal(err)
	}
	opts := &jose.SignerOptions{}
	opts.WithHeader(jose.HeaderKey("url"), e.ts.URL+"/acme/new-account")
	// go-jose does not emit kid for symmetric keys; set it explicitly,
	// as real ACME clients do for the EAB inner JWS.
	opts.WithHeader(jose.HeaderKey("kid"), e.eabKid)
	signer, err := jose.NewSigner(jose.SigningKey{Algorithm: jose.HS256,
		Key: e.eabHMAC}, opts)
	if err != nil {
		e.t.Fatal(err)
	}
	jws, err := signer.Sign(payload)
	if err != nil {
		e.t.Fatal(err)
	}
	return json.RawMessage(jws.FullSerialize())
}

// register creates the account and remembers its URL (kid).
func (e *testEnv) register() {
	e.t.Helper()
	body, _ := json.Marshal(map[string]any{
		"termsOfServiceAgreed":   true,
		"externalAccountBinding": e.eab(),
	})
	resp, v := e.post("/acme/new-account", body, "", "")
	if resp.StatusCode != http.StatusCreated {
		e.t.Fatalf("register: %d %v", resp.StatusCode, v)
	}
	e.kid = resp.Header.Get("Location")
	if e.kid == "" {
		e.t.Fatal("register: no Location header")
	}
}

func (e *testEnv) thumbprint() string {
	e.t.Helper()
	tp, err := (&jose.JSONWebKey{Key: e.key.Public()}).Thumbprint(crypto.SHA256)
	if err != nil {
		e.t.Fatal(err)
	}
	return base64.RawURLEncoding.EncodeToString(tp)
}

// order creates a pending order for names and returns its JSON + URL.
func (e *testEnv) order(names ...string) (map[string]any, string) {
	e.t.Helper()
	ids := make([]map[string]string, len(names))
	for i, n := range names {
		ids[i] = map[string]string{"type": "dns", "value": n}
	}
	body, _ := json.Marshal(map[string]any{"identifiers": ids})
	resp, v := e.post("/acme/new-order", body, e.kid, "")
	if resp.StatusCode != http.StatusCreated {
		e.t.Fatalf("newOrder: %d %v", resp.StatusCode, v)
	}
	return v, resp.Header.Get("Location")
}

// problemType extracts the short ACME error code from a problem document.
func problemType(v map[string]any) string {
	t, _ := v["type"].(string)
	const ns = "urn:ietf:params:acme:error:"
	if len(t) > len(ns) {
		return t[len(ns):]
	}
	return t
}

// path strips the test server base URL from an absolute resource URL.
func (e *testEnv) path(absURL string) string {
	e.t.Helper()
	if len(absURL) <= len(e.ts.URL) || absURL[:len(e.ts.URL)] != e.ts.URL {
		e.t.Fatalf("URL %q is not under the test server", absURL)
	}
	return absURL[len(e.ts.URL):]
}

package main

import (
	"crypto/ecdsa"
	"encoding/json"
	"net/http"
	"testing"

	jose "github.com/go-jose/go-jose/v4"
)

// innerKeyChange builds the §7.3.5 inner JWS: signed by newKey (embedded
// jwk), same url, NO nonce, over {"account", "oldKey"}. oldKey and the
// nonce are parameters so tests can bend each rule.
func (e *testEnv) innerKeyChange(newKey *ecdsa.PrivateKey, oldPub any,
	withNonce bool) []byte {
	e.t.Helper()
	oldJWK, err := (&jose.JSONWebKey{Key: oldPub}).MarshalJSON()
	if err != nil {
		e.t.Fatal(err)
	}
	payload, _ := json.Marshal(map[string]any{
		"account": e.kid,
		"oldKey":  json.RawMessage(oldJWK),
	})
	opts := &jose.SignerOptions{EmbedJWK: true}
	opts.WithHeader(jose.HeaderKey("url"), e.ts.URL+"/acme/key-change")
	if withNonce {
		opts.NonceSource = fixedNonce(e.nonce())
	}
	signer, err := jose.NewSigner(
		jose.SigningKey{Algorithm: jose.ES256, Key: newKey}, opts)
	if err != nil {
		e.t.Fatal(err)
	}
	jws, err := signer.Sign(payload)
	if err != nil {
		e.t.Fatal(err)
	}
	return []byte(jws.FullSerialize())
}

func TestKeyChange(t *testing.T) {
	e := newTestEnv(t)
	e.register()
	newKey, err := generateKey()
	if err != nil {
		t.Fatal(err)
	}

	inner := e.innerKeyChange(newKey, e.key.Public(), false)
	resp, v := e.post("/acme/key-change", inner, e.kid, "")
	if resp.StatusCode != http.StatusOK {
		t.Fatalf("keyChange: %d %v", resp.StatusCode, v)
	}

	// The old key no longer authenticates...
	resp, v = e.post(e.path(e.kid), nil, e.kid, "")
	if resp.StatusCode != http.StatusUnauthorized {
		t.Fatalf("old key still accepted: %d %v", resp.StatusCode, v)
	}
	// ...and the new key does.
	e.key = newKey
	if _, url := e.order("localhost"); url == "" {
		t.Fatal("no order URL after rollover")
	}
}

func TestKeyChangeRejectsWrongOldKey(t *testing.T) {
	e := newTestEnv(t)
	e.register()
	newKey, _ := generateKey()
	bogus, _ := generateKey()

	inner := e.innerKeyChange(newKey, bogus.Public(), false)
	resp, v := e.post("/acme/key-change", inner, e.kid, "")
	if resp.StatusCode != http.StatusBadRequest || problemType(v) != "malformed" {
		t.Fatalf("wrong oldKey accepted: %d %v", resp.StatusCode, v)
	}
}

func TestKeyChangeRejectsInnerNonce(t *testing.T) {
	e := newTestEnv(t)
	e.register()
	newKey, _ := generateKey()

	inner := e.innerKeyChange(newKey, e.key.Public(), true /* nonce */)
	resp, v := e.post("/acme/key-change", inner, e.kid, "")
	if resp.StatusCode != http.StatusBadRequest {
		t.Fatalf("inner nonce accepted: %d %v", resp.StatusCode, v)
	}
}

func TestKeyChangeConflictOnBoundKey(t *testing.T) {
	e := newTestEnv(t)
	e.register()

	// Second account with its own key.
	other := &testEnv{t: t, s: e.s, ts: e.ts,
		eabKid: e.eabKid, eabHMAC: e.eabHMAC}
	other.key, _ = generateKey()
	other.register()

	// Rolling the first account onto the second account's key: 409 +
	// Location naming the holder.
	inner := e.innerKeyChange(other.key, e.key.Public(), false)
	resp, v := e.post("/acme/key-change", inner, e.kid, "")
	if resp.StatusCode != http.StatusConflict {
		t.Fatalf("bound key accepted: %d %v", resp.StatusCode, v)
	}
	if resp.Header.Get("Location") != other.kid {
		t.Fatalf("Location %q, want %q", resp.Header.Get("Location"), other.kid)
	}
}

func TestKeyChangeRejectsSameKey(t *testing.T) {
	e := newTestEnv(t)
	e.register()

	inner := e.innerKeyChange(e.key, e.key.Public(), false)
	resp, v := e.post("/acme/key-change", inner, e.kid, "")
	if resp.StatusCode != http.StatusBadRequest {
		t.Fatalf("same-key rollover accepted: %d %v", resp.StatusCode, v)
	}
}

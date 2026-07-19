package main

import (
	"context"
	"crypto/sha256"
	"encoding/base64"
	"net/http"
	"testing"
)

// txtFor computes the expected dns-01 record for a key authorization.
func txtFor(keyAuth string) string {
	d := sha256.Sum256([]byte(keyAuth))
	return base64.RawURLEncoding.EncodeToString(d[:])
}

// fakeTXT answers only for the given fqdn.
func fakeTXT(fqdn string, records ...string) func(context.Context, string) ([]string, error) {
	return func(_ context.Context, name string) ([]string, error) {
		if name == fqdn {
			return records, nil
		}
		return nil, nil
	}
}

func TestWildcardOrderShape(t *testing.T) {
	e := newTestEnv(t)
	e.register()
	order, _ := e.order("*.test.ca")
	authzURL := order["authorizations"].([]any)[0].(string)
	_, authz := e.post(e.path(authzURL), nil, e.kid, "")

	// RFC 8555 §7.1.4: base identifier + wildcard flag, dns-01 only.
	id := authz["identifier"].(map[string]any)
	if id["value"] != "test.ca" || authz["wildcard"] != true {
		t.Fatalf("authz: %v", authz)
	}
	challs := authz["challenges"].([]any)
	if len(challs) != 1 ||
		challs[0].(map[string]any)["type"] != "dns-01" {
		t.Fatalf("wildcard challenges: %v", challs)
	}
}

func TestNonWildcardOffersBoth(t *testing.T) {
	e := newTestEnv(t)
	e.register()
	order, _ := e.order("localhost")
	authzURL := order["authorizations"].([]any)[0].(string)
	_, authz := e.post(e.path(authzURL), nil, e.kid, "")
	types := map[string]bool{}
	for _, c := range authz["challenges"].([]any) {
		types[c.(map[string]any)["type"].(string)] = true
	}
	if !types["http-01"] || !types["dns-01"] {
		t.Fatalf("challenge types: %v", types)
	}
}

// dnsChallenge extracts the dns-01 challenge of the first authz.
func dnsChallenge(t *testing.T, e *testEnv, order map[string]any) (challURL string, token string) {
	t.Helper()
	authzURL := order["authorizations"].([]any)[0].(string)
	_, authz := e.post(e.path(authzURL), nil, e.kid, "")
	for _, c := range authz["challenges"].([]any) {
		ch := c.(map[string]any)
		if ch["type"] == "dns-01" {
			return ch["url"].(string), ch["token"].(string)
		}
	}
	t.Fatalf("no dns-01 challenge: %v", authz)
	return "", ""
}

func TestDNS01Valid(t *testing.T) {
	e := newTestEnv(t)
	e.register()
	order, loc := e.order("*.test.ca")
	challURL, token := dnsChallenge(t, e, order)

	keyAuth := token + "." + e.thumbprint()
	e.s.dns01.lookupTXT = fakeTXT("_acme-challenge.test.ca",
		"unrelated-record", txtFor(keyAuth))

	resp, v := e.post(e.path(challURL), []byte("{}"), e.kid, "")
	if resp.StatusCode != http.StatusOK || v["status"] != "valid" {
		t.Fatalf("dns-01: %d %v", resp.StatusCode, v)
	}
	_, o := e.post(e.path(loc), nil, e.kid, "")
	if o["status"] != "ready" {
		t.Fatalf("order: %v", o)
	}
}

func TestDNS01WrongRecord(t *testing.T) {
	e := newTestEnv(t)
	e.register()
	order, loc := e.order("*.test.ca")
	challURL, _ := dnsChallenge(t, e, order)

	e.s.dns01.lookupTXT = fakeTXT("_acme-challenge.test.ca", "not-the-digest")
	resp, v := e.post(e.path(challURL), []byte("{}"), e.kid, "")
	if resp.StatusCode != http.StatusOK || v["status"] != "invalid" {
		t.Fatalf("dns-01 wrong record: %d %v", resp.StatusCode, v)
	}
	_, o := e.post(e.path(loc), nil, e.kid, "")
	if o["status"] != "invalid" {
		t.Fatalf("order: %v", o)
	}
}

func TestWildcardFinalize(t *testing.T) {
	e := newTestEnv(t)
	e.register()
	e.s.yca = newYcaRunner(stubYca(t), "", "", "acme", "")
	order, loc := e.order("*.test.ca")
	challURL, token := dnsChallenge(t, e, order)
	keyAuth := token + "." + e.thumbprint()
	e.s.dns01.lookupTXT = fakeTXT("_acme-challenge.test.ca", txtFor(keyAuth))
	if _, v := e.post(e.path(challURL), []byte("{}"), e.kid, ""); v["status"] != "valid" {
		t.Fatalf("challenge: %v", v)
	}

	_, o := e.post(e.path(loc), nil, e.kid, "")
	fin := e.path(o["finalize"].(string))
	// The CSR must carry the literal wildcard name.
	resp, v := e.post(fin,
		finalizeBody(t, csrFor(t, "", []string{"*.test.ca"})), e.kid, "")
	if resp.StatusCode != http.StatusOK || v["status"] != "valid" {
		t.Fatalf("wildcard finalize: %d %v", resp.StatusCode, v)
	}
	// The base name alone does not satisfy a wildcard order.
	_, loc2 := e.order("*.test.ca")
	challURL2, token2 := dnsChallenge(t, e,
		func() map[string]any { _, o := e.post(e.path(loc2), nil, e.kid, ""); return o }())
	keyAuth2 := token2 + "." + e.thumbprint()
	e.s.dns01.lookupTXT = fakeTXT("_acme-challenge.test.ca", txtFor(keyAuth2))
	e.post(e.path(challURL2), []byte("{}"), e.kid, "")
	_, o2 := e.post(e.path(loc2), nil, e.kid, "")
	resp, v = e.post(e.path(o2["finalize"].(string)),
		finalizeBody(t, csrFor(t, "", []string{"test.ca"})), e.kid, "")
	if problemType(v) != "badCSR" {
		t.Fatalf("base-name CSR on wildcard order: %d %v", resp.StatusCode, v)
	}
}

func TestBogusWildcardForms(t *testing.T) {
	e := newTestEnv(t)
	e.register()
	for _, name := range []string{"foo.*.test.ca", "**.test.ca", "*.", "*"} {
		body := []byte(`{"identifiers":[{"type":"dns","value":"` + name + `"}]}`)
		resp, v := e.post("/acme/new-order", body, e.kid, "")
		if problemType(v) != "malformed" {
			t.Errorf("%q: got %d %v, want malformed", name, resp.StatusCode, v)
		}
	}
}

// Copyright 2026 p7cq <707c71@gmail.com>
// SPDX-License-Identifier: Apache-2.0

package main

import (
	"encoding/json"
	"fmt"
	"net"
	"net/http"
	"net/http/httptest"
	"strconv"
	"strings"
	"testing"
	"time"
)

func TestNewOrderPolicy(t *testing.T) {
	e := newTestEnv(t)
	e.register()

	cases := []struct {
		idType, value, wantErr string
	}{
		{"dns", "evil.example.org", "rejectedIdentifier"}, // outside --allow
		{"dns", "*.example.org", "rejectedIdentifier"},    // wildcard outside --allow
		{"ip", "10.0.0.1", "unsupportedIdentifier"},
		{"dns", "bad_host.test.ca", "malformed"},
		{"dns", "", "malformed"},
	}
	for _, c := range cases {
		body, _ := json.Marshal(map[string]any{"identifiers": []map[string]string{
			{"type": c.idType, "value": c.value}}})
		resp, v := e.post("/acme/new-order", body, e.kid, "")
		if problemType(v) != c.wantErr {
			t.Errorf("%s %q: got %d %v, want %s", c.idType, c.value,
				resp.StatusCode, v, c.wantErr)
		}
	}
}

func TestNewOrderShape(t *testing.T) {
	e := newTestEnv(t)
	e.register()
	order, loc := e.order("host.test.ca", "localhost")
	if loc == "" || order["status"] != "pending" {
		t.Fatalf("order: %v (loc %q)", order, loc)
	}
	authzs := order["authorizations"].([]any)
	if len(authzs) != 2 {
		t.Fatalf("want 2 authorizations, got %v", authzs)
	}
	// The authz exposes an http-01 challenge with a token.
	resp, authz := e.post(e.path(authzs[0].(string)), nil, e.kid, "")
	if resp.StatusCode != http.StatusOK || authz["status"] != "pending" {
		t.Fatalf("authz: %d %v", resp.StatusCode, authz)
	}
	ch := authz["challenges"].([]any)[0].(map[string]any)
	if ch["type"] != "http-01" || ch["token"] == "" {
		t.Fatalf("challenge: %v", ch)
	}
}

// challengeHost serves the http-01 well-known path for `token` and returns
// the port it listens on.
func challengeHost(t *testing.T, token, keyAuth string) int {
	t.Helper()
	mux := http.NewServeMux()
	mux.HandleFunc("GET /.well-known/acme-challenge/"+token,
		func(w http.ResponseWriter, _ *http.Request) {
			fmt.Fprint(w, keyAuth)
		})
	ts := httptest.NewServer(mux)
	t.Cleanup(ts.Close)
	_, portStr, err := net.SplitHostPort(ts.Listener.Addr().String())
	if err != nil {
		t.Fatal(err)
	}
	port, _ := strconv.Atoi(portStr)
	return port
}

// runChallenge drives an order for "localhost" through http-01 and returns
// the order URL path. serveWrongContent makes the validation fail.
func runChallenge(t *testing.T, e *testEnv, serveWrong bool) (orderPath string) {
	t.Helper()
	order, loc := e.order("localhost")
	authzURL := order["authorizations"].([]any)[0].(string)
	resp0, authz := e.post(e.path(authzURL), nil, e.kid, "")
	if resp0.StatusCode != http.StatusOK {
		t.Fatalf("authz fetch: %d %v", resp0.StatusCode, authz)
	}
	ch := authz["challenges"].([]any)[0].(map[string]any)
	token := ch["token"].(string)

	keyAuth := token + "." + e.thumbprint()
	if serveWrong {
		keyAuth = "not-the-key-authorization"
	}
	e.s.http01.port = challengeHost(t, token, keyAuth)

	// POST {} triggers validation.
	resp, v := e.post(e.path(ch["url"].(string)), []byte("{}"), e.kid, "")
	if resp.StatusCode != http.StatusOK {
		t.Fatalf("challenge POST: %d %v", resp.StatusCode, v)
	}
	return e.path(loc)
}

func TestHTTP01Valid(t *testing.T) {
	e := newTestEnv(t)
	e.register()
	orderPath := runChallenge(t, e, false)
	_, order := e.post(orderPath, nil, e.kid, "")
	if order["status"] != "ready" {
		t.Fatalf("order after valid challenge: %v", order)
	}
}

func TestHTTP01WrongContent(t *testing.T) {
	e := newTestEnv(t)
	e.register()
	orderPath := runChallenge(t, e, true)
	_, order := e.post(orderPath, nil, e.kid, "")
	if order["status"] != "invalid" {
		t.Fatalf("order after failed challenge: %v", order)
	}
}

func TestHTTP01Unreachable(t *testing.T) {
	e := newTestEnv(t)
	e.register()
	order, loc := e.order("localhost")
	authzURL := order["authorizations"].([]any)[0].(string)
	_, authz := e.post(e.path(authzURL), nil, e.kid, "")
	ch := authz["challenges"].([]any)[0].(map[string]any)

	e.s.http01.port = 1 // nothing listens there
	resp, v := e.post(e.path(ch["url"].(string)), []byte("{}"), e.kid, "")
	if resp.StatusCode != http.StatusOK || v["status"] != "invalid" {
		t.Fatalf("unreachable host: %d %v", resp.StatusCode, v)
	}
	_, o := e.post(e.path(loc), nil, e.kid, "")
	if o["status"] != "invalid" {
		t.Fatalf("order: %v", o)
	}
}

func TestOrderOwnership(t *testing.T) {
	e := newTestEnv(t)
	e.register()
	_, loc := e.order("localhost")

	// A second account (own key and credential) cannot see it.
	e2 := e.otherAccount()
	resp, v := e2.post(e.path(loc), nil, e2.kid, "")
	if resp.StatusCode != http.StatusNotFound {
		t.Fatalf("foreign order visible: %d %v", resp.StatusCode, v)
	}
}

// challengeOfType returns the URL of the first authz's challenge of `typ`.
func challengeOfType(t *testing.T, e *testEnv, order map[string]any,
	typ string) string {
	t.Helper()
	authzURL := order["authorizations"].([]any)[0].(string)
	_, authz := e.post(e.path(authzURL), nil, e.kid, "")
	for _, c := range authz["challenges"].([]any) {
		if ch := c.(map[string]any); ch["type"] == typ {
			return ch["url"].(string)
		}
	}
	t.Fatalf("no %s challenge: %v", typ, authz)
	return ""
}

// A failed challenge invalidates the authorization (RFC 8555 7.5.1): a
// sibling challenge must not bring the order back to ready.
func TestChallengeAfterFailedSibling(t *testing.T) {
	e := newTestEnv(t)
	e.register()
	order, loc := e.order("localhost")

	e.s.http01.port = 1 // nothing listens there
	resp, v := e.post(e.path(challengeOfType(t, e, order, "http-01")),
		[]byte("{}"), e.kid, "")
	if resp.StatusCode != http.StatusOK || v["status"] != "invalid" {
		t.Fatalf("http-01: %d %v", resp.StatusCode, v)
	}

	challURL, token := dnsChallenge(t, e, order)
	e.s.dns01.lookupTXT = fakeTXT("_acme-challenge.localhost",
		txtFor(token+"."+e.thumbprint()))
	e.post(e.path(challURL), []byte("{}"), e.kid, "")

	if _, o := e.post(e.path(loc), nil, e.kid, ""); o["status"] != "invalid" {
		t.Fatalf("order after a failed challenge: %v", o)
	}
}

// A valid order stays valid: validating a leftover challenge must not
// reopen it and drop its certificate.
func TestChallengeCannotReopenValidOrder(t *testing.T) {
	e := newTestEnv(t)
	e.register()
	st := newStub(t, "localhost")
	e.s.yca = newYcaRunner(st.bin, "", "", "acme", "")
	orderPath := runChallenge(t, e, false)
	_, order := e.post(orderPath, nil, e.kid, "")
	fin := e.path(order["finalize"].(string))
	if resp, v := e.post(fin, finalizeBody(t, csrFor(t, "",
		[]string{"localhost"})), e.kid, ""); v["status"] != "valid" {
		t.Fatalf("finalize: %d %v", resp.StatusCode, v)
	}

	challURL, token := dnsChallenge(t, e, order)
	e.s.dns01.lookupTXT = fakeTXT("_acme-challenge.localhost",
		txtFor(token+"."+e.thumbprint()))
	e.post(e.path(challURL), []byte("{}"), e.kid, "")

	_, o := e.post(orderPath, nil, e.kid, "")
	if o["status"] != "valid" || o["certificate"] == nil {
		t.Fatalf("valid order reopened: %v", o)
	}
}

// Identifiers must be DNS hostnames, not just hostname characters: an
// empty or oversized label, an edge hyphen, or an all-numeric last label
// (which covers IPv4 literals) would otherwise land in a dNSName SAN.
func TestNewOrderRejectsNonHostnames(t *testing.T) {
	e := newTestEnv(t)
	e.register()
	// A second account on a credential without --allow (any name), so the
	// IP-shaped names reach the syntax check instead of the policy one.
	open := e.newEAB("", false, time.Time{})
	open.register()

	newOrder := func(env *testEnv, name string) (int, string) {
		body, _ := json.Marshal(map[string]any{"identifiers": []map[string]string{
			{"type": "dns", "value": name}}})
		resp, v := env.post("/acme/new-order", body, env.kid, "")
		return resp.StatusCode, problemType(v)
	}
	// Inside --allow "*.test.ca", so only the syntax check can refuse them.
	for _, name := range []string{"a..test.ca", "-a.test.ca", "a-.test.ca",
		"test.ca.", strings.Repeat("a", 64) + ".test.ca"} {
		if code, typ := newOrder(e, name); typ != "malformed" {
			t.Errorf("%q: got %d %q, want malformed", name, code, typ)
		}
	}
	for _, name := range []string{"10.0.0.5", "127.0.0.1", "1.2.3"} {
		if code, typ := newOrder(open, name); typ != "malformed" {
			t.Errorf("%q: got %d %q, want malformed", name, code, typ)
		}
	}
	// Single-label and ordinary names stay acceptable.
	for _, name := range []string{"localhost", "host.lan", "a1.test.ca"} {
		if code, typ := newOrder(open, name); code != http.StatusCreated {
			t.Errorf("%q: got %d %q, want 201", name, code, typ)
		}
	}
}

// http-01 redirects stay on the web ports: a redirect to another port
// would turn the validator into a probe of internal services.
func TestHTTP01RedirectToOtherPortRefused(t *testing.T) {
	e := newTestEnv(t)
	e.register()
	order, loc := e.order("localhost")
	authzURL := order["authorizations"].([]any)[0].(string)
	_, authz := e.post(e.path(authzURL), nil, e.kid, "")
	ch := authz["challenges"].([]any)[0].(map[string]any)
	token := ch["token"].(string)

	// The target answers correctly; only the redirect to it is at issue.
	target := challengeHost(t, token, token+"."+e.thumbprint())
	front := httptest.NewServer(http.HandlerFunc(
		func(w http.ResponseWriter, r *http.Request) {
			http.Redirect(w, r, fmt.Sprintf("http://127.0.0.1:%d%s", target,
				r.URL.Path), http.StatusFound)
		}))
	t.Cleanup(front.Close)
	_, portStr, _ := net.SplitHostPort(front.Listener.Addr().String())
	e.s.http01.port, _ = strconv.Atoi(portStr)

	resp, v := e.post(e.path(ch["url"].(string)), []byte("{}"), e.kid, "")
	if resp.StatusCode != http.StatusOK || v["status"] != "invalid" {
		t.Fatalf("redirect to port %d followed: %d %v", target,
			resp.StatusCode, v)
	}
	if detail, _ := v["error"].(map[string]any)["detail"].(string); !strings.Contains(detail, "refused") {
		t.Fatalf("error detail: %q", detail)
	}
	if _, o := e.post(e.path(loc), nil, e.kid, ""); o["status"] != "invalid" {
		t.Fatalf("order: %v", o)
	}
}

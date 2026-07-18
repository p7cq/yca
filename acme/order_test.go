package main

import (
	"encoding/json"
	"fmt"
	"net"
	"net/http"
	"net/http/httptest"
	"strconv"
	"testing"
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

	// A second account (own key, same EAB cred for simplicity) cannot see it.
	e2 := &testEnv{t: t, s: e.s, ts: e.ts, eabKid: e.eabKid, eabHMAC: e.eabHMAC}
	var err error
	if e2.key, err = generateKey(); err != nil {
		t.Fatal(err)
	}
	e2.register()
	resp, v := e2.post(e.path(loc), nil, e2.kid, "")
	if resp.StatusCode != http.StatusNotFound {
		t.Fatalf("foreign order visible: %d %v", resp.StatusCode, v)
	}
}

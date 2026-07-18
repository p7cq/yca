package main

import (
	"bytes"
	"encoding/base64"
	"encoding/json"
	"net/http"
	"os"
	"strings"
	"testing"
)

// issueViaStub drives a full happy flow and returns the stub (which holds
// the "issued" leaf certificate and key).
func issueViaStub(t *testing.T, e *testEnv) *stub {
	t.Helper()
	st := newStub(t, "localhost")
	e.s.yca = newYcaRunner(st.bin, "", "", "acme")
	orderPath := runChallenge(t, e, false)
	_, order := e.post(orderPath, nil, e.kid, "")
	fin := e.path(order["finalize"].(string))
	resp, v := e.post(fin, finalizeBody(t, csrFor(t, "", []string{"localhost"})),
		e.kid, "")
	if resp.StatusCode != http.StatusOK || v["status"] != "valid" {
		t.Fatalf("issue: %d %v", resp.StatusCode, v)
	}
	return st
}

func revokeBody(t *testing.T, st *stub, reason int) []byte {
	t.Helper()
	body, err := json.Marshal(map[string]any{
		"certificate": base64.RawURLEncoding.EncodeToString(st.leaf.Raw),
		"reason":      reason,
	})
	if err != nil {
		t.Fatal(err)
	}
	return body
}

func TestRevokeByAccount(t *testing.T) {
	e := newTestEnv(t)
	e.register()
	st := issueViaStub(t, e)

	resp, v := e.post("/acme/revoke-cert", revokeBody(t, st, 1), e.kid, "")
	if resp.StatusCode != http.StatusOK {
		t.Fatalf("revoke: %d %v", resp.StatusCode, v)
	}
	// The exact serial went to the CLI, with the mapped reason name.
	args, _ := os.ReadFile(st.args)
	wantSerial := strings.ToUpper(st.leaf.SerialNumber.Text(16))
	if !bytes.Contains(args,
		[]byte("revoke server --serial "+wantSerial+" --reason keyCompromise")) {
		t.Fatalf("CLI args:\n%s", args)
	}
	// Second attempt: the stub answers like the CA - already revoked.
	resp, v = e.post("/acme/revoke-cert", revokeBody(t, st, 0), e.kid, "")
	if resp.StatusCode != http.StatusBadRequest ||
		problemType(v) != "alreadyRevoked" {
		t.Fatalf("re-revoke: %d %v", resp.StatusCode, v)
	}
}

func TestRevokeByCertKey(t *testing.T) {
	e := newTestEnv(t)
	e.register()
	st := issueViaStub(t, e)

	// Signed with the CERTIFICATE key, jwk embedded - no account involved.
	body := e.signWith(st.leafKey, "/acme/revoke-cert",
		revokeBody(t, st, 0), "", "")
	resp, err := http.Post(e.ts.URL+"/acme/revoke-cert",
		"application/jose+json", strings.NewReader(body))
	if err != nil {
		t.Fatal(err)
	}
	defer resp.Body.Close()
	if resp.StatusCode != http.StatusOK {
		t.Fatalf("revoke by cert key: %d", resp.StatusCode)
	}
}

func TestRevokeWrongKey(t *testing.T) {
	e := newTestEnv(t)
	e.register()
	st := issueViaStub(t, e)

	other, _ := generateKey() // neither the account nor the cert key
	body := e.signWith(other, "/acme/revoke-cert", revokeBody(t, st, 0), "", "")
	resp, err := http.Post(e.ts.URL+"/acme/revoke-cert",
		"application/jose+json", strings.NewReader(body))
	if err != nil {
		t.Fatal(err)
	}
	defer resp.Body.Close()
	if resp.StatusCode != http.StatusForbidden {
		t.Fatalf("foreign jwk accepted: %d", resp.StatusCode)
	}
}

func TestRevokeForeignAccount(t *testing.T) {
	e := newTestEnv(t)
	e.register()
	st := issueViaStub(t, e)

	e2 := &testEnv{t: t, s: e.s, ts: e.ts, eabKid: e.eabKid, eabHMAC: e.eabHMAC}
	var err error
	if e2.key, err = generateKey(); err != nil {
		t.Fatal(err)
	}
	e2.register()
	resp, v := e2.post("/acme/revoke-cert", revokeBody(t, st, 0), e2.kid, "")
	if resp.StatusCode != http.StatusForbidden || problemType(v) != "unauthorized" {
		t.Fatalf("foreign account revoke: %d %v", resp.StatusCode, v)
	}
}

func TestRevokeBadReason(t *testing.T) {
	e := newTestEnv(t)
	e.register()
	st := issueViaStub(t, e)
	resp, v := e.post("/acme/revoke-cert", revokeBody(t, st, 2), e.kid, "")
	if resp.StatusCode != http.StatusBadRequest ||
		problemType(v) != "badRevocationReason" {
		t.Fatalf("cACompromise from an EE holder: %d %v", resp.StatusCode, v)
	}
}

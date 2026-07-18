package main

import (
	"crypto/rand"
	"encoding/json"
	"net/http"
	"testing"
)

func TestNewAccountRequiresEAB(t *testing.T) {
	e := newTestEnv(t)
	body, _ := json.Marshal(map[string]any{"termsOfServiceAgreed": true})
	resp, v := e.post("/acme/new-account", body, "", "")
	if resp.StatusCode != http.StatusBadRequest ||
		problemType(v) != "externalAccountRequired" {
		t.Fatalf("no EAB: %d %v", resp.StatusCode, v)
	}
}

func TestNewAccountWrongHMAC(t *testing.T) {
	e := newTestEnv(t)
	if _, err := rand.Read(e.eabHMAC); err != nil { // desync from the DB copy
		t.Fatal(err)
	}
	body, _ := json.Marshal(map[string]any{
		"externalAccountBinding": e.eab()})
	resp, v := e.post("/acme/new-account", body, "", "")
	if resp.StatusCode != http.StatusUnauthorized {
		t.Fatalf("wrong HMAC: %d %v", resp.StatusCode, v)
	}
}

func TestNewAccountUnknownEABKid(t *testing.T) {
	e := newTestEnv(t)
	e.eabKid = newID() // not provisioned
	body, _ := json.Marshal(map[string]any{
		"externalAccountBinding": e.eab()})
	resp, v := e.post("/acme/new-account", body, "", "")
	if resp.StatusCode != http.StatusUnauthorized {
		t.Fatalf("unknown EAB kid: %d %v", resp.StatusCode, v)
	}
}

func TestNewAccountIdempotent(t *testing.T) {
	e := newTestEnv(t)
	e.register()
	first := e.kid
	body, _ := json.Marshal(map[string]any{
		"externalAccountBinding": e.eab()})
	resp, _ := e.post("/acme/new-account", body, "", "")
	if resp.StatusCode != http.StatusOK ||
		resp.Header.Get("Location") != first {
		t.Fatalf("re-register: %d %q vs %q", resp.StatusCode,
			resp.Header.Get("Location"), first)
	}
}

func TestOnlyReturnExistingUnknown(t *testing.T) {
	e := newTestEnv(t)
	body, _ := json.Marshal(map[string]any{"onlyReturnExisting": true})
	resp, v := e.post("/acme/new-account", body, "", "")
	if problemType(v) != "accountDoesNotExist" {
		t.Fatalf("onlyReturnExisting: %d %v", resp.StatusCode, v)
	}
}

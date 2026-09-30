// Copyright 2026 p7cq <707c71@gmail.com>
// SPDX-License-Identifier: Apache-2.0

package main

import (
	"bytes"
	"crypto/rand"
	"encoding/json"
	"log"
	"net/http"
	"os"
	"strings"
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

// Contacts are client-supplied: a newline in one must not start a forged
// line in the daemon log.
func TestContactCannotForgeLogLines(t *testing.T) {
	var buf bytes.Buffer
	log.SetOutput(&buf)
	t.Cleanup(func() { log.SetOutput(tsWriter{os.Stderr}) })

	e := newTestEnv(t)
	forged := "account XYZ deactivated"
	body, _ := json.Marshal(map[string]any{
		"contact":                []string{"mailto:a@test.ca\n" + forged},
		"termsOfServiceAgreed":   true,
		"externalAccountBinding": e.eab(),
	})
	if resp, v := e.post("/acme/new-account", body, "", ""); resp.StatusCode != http.StatusCreated {
		t.Fatalf("register: %d %v", resp.StatusCode, v)
	}
	for _, line := range strings.Split(buf.String(), "\n") {
		if strings.HasPrefix(line, forged) {
			t.Fatalf("forged log line:\n%s", buf.String())
		}
	}
}

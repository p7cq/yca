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
	"sync"
	"testing"
	"time"
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

// detail extracts a problem document's detail.
func detail(v map[string]any) string {
	d, _ := v["detail"].(string)
	return d
}

// A credential binds the first account registered with it: a leaked
// kid/HMAC cannot mint further accounts, not even once that account is
// deactivated. Re-registering the bound key still returns its account.
func TestEABSingleUse(t *testing.T) {
	e := newTestEnv(t)
	e.register()

	resp, v := e.sameEAB().tryRegister()
	if resp.StatusCode != http.StatusUnauthorized ||
		!strings.Contains(detail(v), "already used") {
		t.Fatalf("second account on a single-use credential: %d %v",
			resp.StatusCode, v)
	}
	if bound, _ := e.s.db.AccountsByEAB(e.eabKid); len(bound) != 1 {
		t.Fatalf("accounts on the credential: %v", bound)
	}

	// Idempotent re-registration is resolved before the credential.
	if resp, v := e.tryRegister(); resp.StatusCode != http.StatusOK ||
		resp.Header.Get("Location") != e.kid {
		t.Fatalf("re-register: %d %v", resp.StatusCode, v)
	}

	body, _ := json.Marshal(map[string]string{"status": "deactivated"})
	if resp, v := e.post(e.path(e.kid), body, e.kid, ""); resp.StatusCode != http.StatusOK {
		t.Fatalf("deactivate: %d %v", resp.StatusCode, v)
	}
	if resp, v := e.sameEAB().tryRegister(); resp.StatusCode != http.StatusUnauthorized {
		t.Fatalf("credential freed by deactivation: %d %v", resp.StatusCode, v)
	}
}

// Concurrent registrations on one single-use credential: exactly one wins.
func TestEABSingleUseConcurrent(t *testing.T) {
	e := newTestEnv(t)
	const n = 10
	bodies := make([]string, n)
	for i := range bodies { // signed up front: each needs its own nonce
		o := e.sameEAB()
		bodies[i] = o.sign("/acme/new-account", o.registerBody(), "", "")
	}
	codes := make([]int, n)
	var wg sync.WaitGroup
	for i := range bodies {
		wg.Add(1)
		go func() {
			defer wg.Done()
			resp, err := http.Post(e.ts.URL+"/acme/new-account",
				"application/jose+json", strings.NewReader(bodies[i]))
			if err != nil {
				return
			}
			resp.Body.Close()
			codes[i] = resp.StatusCode
		}()
	}
	wg.Wait()
	created := 0
	for _, c := range codes {
		if c == http.StatusCreated {
			created++
		}
	}
	bound, _ := e.s.db.AccountsByEAB(e.eabKid)
	if created != 1 || len(bound) != 1 {
		t.Fatalf("%d registrations succeeded, %d accounts bound (codes %v)",
			created, len(bound), codes)
	}
}

// A reusable credential registers any number of accounts.
func TestEABReusable(t *testing.T) {
	e := newTestEnv(t)
	fleet := e.newEAB(testAllow, true, time.Time{})
	fleet.register()
	fleet.sameEAB().register()
	fleet.sameEAB().register()
	if bound, _ := e.s.db.AccountsByEAB(fleet.eabKid); len(bound) != 3 {
		t.Fatalf("accounts on the reusable credential: %v", bound)
	}
}

// An expired credential registers nothing more; accounts registered
// before its expiry keep ordering.
func TestEABExpired(t *testing.T) {
	e := newTestEnv(t)
	ticket := e.newEAB(testAllow, false, time.Now().Add(-time.Minute))
	if resp, v := ticket.tryRegister(); resp.StatusCode != http.StatusUnauthorized ||
		!strings.Contains(detail(v), "expired") {
		t.Fatalf("expired single-use credential: %d %v", resp.StatusCode, v)
	}

	fleet := e.newEAB(testAllow, true, time.Now().Add(time.Hour))
	fleet.register()
	if _, err := e.s.db.sql.Exec("UPDATE eab_creds SET expires = ? "+
		"WHERE kid = ?", time.Now().Add(-time.Minute).Unix(),
		fleet.eabKid); err != nil {
		t.Fatal(err)
	}
	if resp, v := fleet.sameEAB().tryRegister(); resp.StatusCode != http.StatusUnauthorized ||
		!strings.Contains(detail(v), "expired") {
		t.Fatalf("expired reusable credential: %d %v", resp.StatusCode, v)
	}
	fleet.order("localhost") // fails the test unless 201
}

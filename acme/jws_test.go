// Copyright 2026 p7cq <707c71@gmail.com>
// SPDX-License-Identifier: Apache-2.0

package main

import (
	"encoding/json"
	"fmt"
	"net/http"
	"strings"
	"testing"
	"time"
)

func TestNonceSingleUse(t *testing.T) {
	e := newTestEnv(t)
	e.register()
	nonce := e.nonce()
	body, _ := json.Marshal(map[string]any{"identifiers": []map[string]string{
		{"type": "dns", "value": "localhost"}}})

	resp, _ := e.post("/acme/new-order", body, e.kid, nonce)
	if resp.StatusCode != http.StatusCreated {
		t.Fatalf("first use: %d", resp.StatusCode)
	}
	resp, v := e.post("/acme/new-order", body, e.kid, nonce) // replay
	if resp.StatusCode != http.StatusBadRequest || problemType(v) != "badNonce" {
		t.Fatalf("replay: %d %v", resp.StatusCode, v)
	}
}

func TestMadeUpNonceRejected(t *testing.T) {
	e := newTestEnv(t)
	e.register()
	resp, v := e.post("/acme/new-order", []byte("{}"), e.kid, newID())
	if resp.StatusCode != http.StatusBadRequest || problemType(v) != "badNonce" {
		t.Fatalf("made-up nonce: %d %v", resp.StatusCode, v)
	}
}

func TestURLBinding(t *testing.T) {
	e := newTestEnv(t)
	e.register()
	// Signed for new-order, sent to new-account: must be rejected.
	body := e.sign("/acme/new-order", []byte("{}"), e.kid, "")
	resp, err := http.Post(e.ts.URL+"/acme/new-account", "application/jose+json",
		strings.NewReader(body))
	if err != nil {
		t.Fatal(err)
	}
	defer resp.Body.Close()
	if resp.StatusCode != http.StatusUnauthorized {
		t.Fatalf("url mismatch accepted: %d", resp.StatusCode)
	}
}

func TestTamperedPayloadRejected(t *testing.T) {
	e := newTestEnv(t)
	e.register()
	body := e.sign("/acme/new-order", []byte(`{"identifiers":[]}`), e.kid, "")
	var jws map[string]any
	if err := json.Unmarshal([]byte(body), &jws); err != nil {
		t.Fatal(err)
	}
	jws["payload"] = "eyJmYWtlIjp0cnVlfQ" // {"fake":true}
	tampered, _ := json.Marshal(jws)
	resp, err := http.Post(e.ts.URL+"/acme/new-order", "application/jose+json",
		strings.NewReader(string(tampered)))
	if err != nil {
		t.Fatal(err)
	}
	defer resp.Body.Close()
	if resp.StatusCode != http.StatusUnauthorized {
		t.Fatalf("tampered payload accepted: %d", resp.StatusCode)
	}
}

func TestKidRequiredAfterRegistration(t *testing.T) {
	e := newTestEnv(t)
	e.register()
	// newOrder with an embedded jwk instead of kid: malformed.
	resp, v := e.post("/acme/new-order", []byte("{}"), "", "")
	if resp.StatusCode != http.StatusBadRequest || problemType(v) != "malformed" {
		t.Fatalf("jwk on kid-only resource: %d %v", resp.StatusCode, v)
	}
}

func TestUnknownAccountKid(t *testing.T) {
	e := newTestEnv(t)
	e.register()
	resp, v := e.post("/acme/new-order", []byte("{}"),
		e.ts.URL+"/acme/acct/does-not-exist", "")
	if resp.StatusCode != http.StatusUnauthorized ||
		problemType(v) != "accountDoesNotExist" {
		t.Fatalf("unknown kid: %d %v", resp.StatusCode, v)
	}
}

// new-nonce is unauthenticated: a flood must not grow the table past
// maxNonces, and the newest nonce must survive the trim.
func TestNonceTableBounded(t *testing.T) {
	e := newTestEnv(t)
	// The flood, compressed: maxNonces+500 live nonces in one transaction
	// (the same rows a burst of HEAD /acme/new-nonce would leave).
	tx, err := e.s.db.sql.Begin()
	if err != nil {
		t.Fatal(err)
	}
	now := time.Now().Unix()
	for i := 0; i < maxNonces+500; i++ {
		if _, err := tx.Exec("INSERT INTO nonces (value, created) "+
			"VALUES (?, ?)", fmt.Sprintf("flood-%d", i), now); err != nil {
			t.Fatal(err)
		}
	}
	if err := tx.Commit(); err != nil {
		t.Fatal(err)
	}

	fresh := e.nonce() // HEAD /acme/new-nonce
	var n int
	if err := e.s.db.sql.QueryRow("SELECT COUNT(*) FROM nonces").
		Scan(&n); err != nil {
		t.Fatal(err)
	}
	if n > maxNonces {
		t.Fatalf("%d live nonces, cap is %d", n, maxNonces)
	}
	// The newest is usable; the oldest flood entry is gone.
	if ok, _ := e.s.db.ConsumeNonce(fresh); !ok {
		t.Fatal("fresh nonce trimmed")
	}
	if ok, _ := e.s.db.ConsumeNonce("flood-0"); ok {
		t.Fatal("oldest nonce survived the trim")
	}
}

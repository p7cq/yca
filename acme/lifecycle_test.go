// Account deactivation (§7.3.6), EAB credential deletion, and protocol GC.
package main

import (
	"net/http"
	"strconv"
	"testing"
	"time"
)

func TestDeactivateAccount(t *testing.T) {
	e := newTestEnv(t)
	e.register()

	resp, v := e.post(e.path(e.kid), []byte(`{"status":"deactivated"}`),
		e.kid, "")
	if resp.StatusCode != http.StatusOK || v["status"] != "deactivated" {
		t.Fatalf("deactivate: %d %v", resp.StatusCode, v)
	}

	// Every later request from the account dies with unauthorized -
	// including a second deactivation and a new order.
	resp, v = e.post(e.path(e.kid), nil, e.kid, "")
	if resp.StatusCode != http.StatusUnauthorized || problemType(v) != "unauthorized" {
		t.Fatalf("deactivated account still accepted: %d %v", resp.StatusCode, v)
	}
}

func TestAccountUpdateRejectsOtherStatus(t *testing.T) {
	e := newTestEnv(t)
	e.register()

	resp, v := e.post(e.path(e.kid), []byte(`{"status":"revoked"}`), e.kid, "")
	if resp.StatusCode != http.StatusBadRequest {
		t.Fatalf("bogus status accepted: %d %v", resp.StatusCode, v)
	}
	// Contact-style updates stay a no-op on a valid account.
	resp, v = e.post(e.path(e.kid),
		[]byte(`{"contact":["mailto:a@example.ca"]}`), e.kid, "")
	if resp.StatusCode != http.StatusOK || v["status"] != "valid" {
		t.Fatalf("contact update: %d %v", resp.StatusCode, v)
	}
}

func TestEABDeleteDisablesOrders(t *testing.T) {
	e := newTestEnv(t)
	e.register()
	e.order("localhost") // sanity: the credential permits this

	bound, err := e.s.db.AccountsByEAB(e.eabKid)
	if err != nil || len(bound) != 1 {
		t.Fatalf("bound accounts: %v %v", bound, err)
	}
	if ok, err := e.s.db.DeleteEAB(e.eabKid); !ok || err != nil {
		t.Fatalf("delete: %v %v", ok, err)
	}
	if ok, _ := e.s.db.DeleteEAB(e.eabKid); ok {
		t.Fatal("second delete reported success")
	}

	// The account authenticates fine but can no longer order anything.
	body := []byte(`{"identifiers":[{"type":"dns","value":"localhost"}]}`)
	resp, v := e.post("/acme/new-order", body, e.kid, "")
	if resp.StatusCode != http.StatusForbidden || problemType(v) != "rejectedIdentifier" {
		t.Fatalf("order after eab delete: %d %v", resp.StatusCode, v)
	}
}

func TestGC(t *testing.T) {
	e := newTestEnv(t)
	e.register()
	_, orderURL := e.order("localhost")

	// Fresh objects survive a GC pass (their expiry is 24 h out).
	if n, err := e.s.db.GC(24 * time.Hour); n != 0 || err != nil {
		t.Fatalf("gc on fresh state: %d %v", n, err)
	}

	// Age the order and its authz one hour past expiry: with a 30-minute
	// grace all three object kinds must go (the challenge via its
	// orphaned authz).
	expired := strconv.FormatInt(time.Now().Add(-time.Hour).Unix(), 10)
	for _, q := range []string{
		"UPDATE orders SET expires = " + expired,
		"UPDATE authzs SET expires = " + expired,
	} {
		if _, err := e.s.db.sql.Exec(q); err != nil {
			t.Fatal(err)
		}
	}
	n, err := e.s.db.GC(30 * time.Minute)
	if err != nil || n < 3 {
		t.Fatalf("gc removed %d objects (%v), want >= 3", n, err)
	}

	resp, _ := e.post(e.path(orderURL), nil, e.kid, "")
	if resp.StatusCode != http.StatusNotFound {
		t.Fatalf("gc'd order still served: %d", resp.StatusCode)
	}
}

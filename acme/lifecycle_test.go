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

// A kid is base64url, so one in sixty-four would start with '-'. Such a
// kid reads as an option to Go's flag package and to every shell tool it
// is pasted into: `eab delete` used to be unable to remove those
// credentials at all. newID no longer produces that shape, and the parser
// no longer depends on it either.
func TestNewIDShapeAndEntropy(t *testing.T) {
	seen := map[string]bool{}
	for i := 0; i < 4096; i++ {
		id := newID()
		if id == "" || id[0] == '-' {
			t.Fatalf("newID produced an option-shaped id: %q", id)
		}
		// 17 bytes: rejecting a leading '-' must not push a challenge
		// token under the 128 bits RFC 8555 8.3 requires, and 16 bytes
		// would sit exactly on that floor.
		if len(id) != 23 {
			t.Fatalf("id %q is %d chars, want 23 (17 bytes of entropy)",
				id, len(id))
		}
		for _, c := range id {
			ok := c >= 'A' && c <= 'Z' || c >= 'a' && c <= 'z' ||
				c >= '0' && c <= '9' || c == '-' || c == '_'
			if !ok {
				t.Fatalf("id %q leaves the base64url alphabet", id)
			}
		}
		if seen[id] {
			t.Fatalf("newID repeated %q within %d draws", id, i)
		}
		seen[id] = true
	}
}

func TestSplitDeleteKid(t *testing.T) {
	cases := []struct {
		verb     string
		args     []string
		wantKid  string
		wantRest []string
	}{
		// The kid comes last and is lifted off before flag parsing, so a
		// leading '-' cannot be mistaken for an option.
		{"delete", []string{"--state", "db", "-DashKid"}, "-DashKid",
			[]string{"--state", "db"}},
		{"delete", []string{"--state", "db", "PlainKid"}, "PlainKid",
			[]string{"--state", "db"}},
		{"delete", []string{"OnlyKid"}, "OnlyKid", []string{}},
		// Nothing to lift: the verb reports its own usage.
		{"delete", []string{}, "", []string{}},
		// Other verbs keep every argument.
		{"list", []string{"--state", "db"}, "", []string{"--state", "db"}},
		{"new", []string{"--allow", "*.example.ca"}, "",
			[]string{"--allow", "*.example.ca"}},
	}
	for _, c := range cases {
		kid, rest := splitDeleteKid(c.verb, c.args)
		if kid != c.wantKid || len(rest) != len(c.wantRest) {
			t.Errorf("%s %v: got (%q, %v), want (%q, %v)", c.verb, c.args,
				kid, rest, c.wantKid, c.wantRest)
			continue
		}
		for i := range rest {
			if rest[i] != c.wantRest[i] {
				t.Errorf("%s %v: rest %v, want %v", c.verb, c.args, rest,
					c.wantRest)
				break
			}
		}
	}
}

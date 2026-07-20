package main

import (
	"crypto/x509"
	"encoding/json"
	"io"
	"math/big"
	"net/http"
	"strconv"
	"strings"
	"testing"
	"time"
)

func TestDERIntBytes(t *testing.T) {
	cases := []struct {
		n    int64
		want []byte
	}{
		{0, []byte{0}},
		{1, []byte{1}},
		{127, []byte{0x7f}},
		{128, []byte{0x00, 0x80}}, // high bit: two's complement padding
		{255, []byte{0x00, 0xff}},
		{256, []byte{0x01, 0x00}},
	}
	for _, c := range cases {
		got := derIntBytes(big.NewInt(c.n))
		if len(got) != len(c.want) {
			t.Errorf("%d: got %x, want %x", c.n, got, c.want)
			continue
		}
		for i := range got {
			if got[i] != c.want[i] {
				t.Errorf("%d: got %x, want %x", c.n, got, c.want)
				break
			}
		}
	}
}

func TestRenewalInfo(t *testing.T) {
	e := newTestEnv(t)
	e.register()
	st := issueViaStub(t, e)

	id, err := ariID(st.leaf)
	if err != nil {
		t.Fatal(err)
	}
	resp, err := http.Get(e.ts.URL + "/acme/renewal-info/" + id)
	if err != nil {
		t.Fatal(err)
	}
	defer resp.Body.Close()
	if resp.StatusCode != http.StatusOK {
		t.Fatalf("renewal-info: %d", resp.StatusCode)
	}
	if resp.Header.Get("Retry-After") == "" {
		t.Fatal("no Retry-After header")
	}
	var v struct {
		SuggestedWindow struct {
			Start, End time.Time
		} `json:"suggestedWindow"`
	}
	raw, _ := io.ReadAll(resp.Body)
	if err := json.Unmarshal(raw, &v); err != nil {
		t.Fatalf("body: %s", raw)
	}
	// The window mirrors the CA policy: opens when 33% of the lifetime
	// remains, closes at 10% (second-level truncation tolerated).
	life := st.leaf.NotAfter.Sub(st.leaf.NotBefore)
	wantStart := st.leaf.NotAfter.Add(-life * renewPct / 100)
	wantEnd := st.leaf.NotAfter.Add(-life * closePct / 100)
	if d := v.SuggestedWindow.Start.Sub(wantStart); d > time.Second || d < -time.Second {
		t.Errorf("start: got %v, want %v", v.SuggestedWindow.Start, wantStart)
	}
	if d := v.SuggestedWindow.End.Sub(wantEnd); d > time.Second || d < -time.Second {
		t.Errorf("end: got %v, want %v", v.SuggestedWindow.End, wantEnd)
	}
	if !v.SuggestedWindow.Start.Before(v.SuggestedWindow.End) {
		t.Error("window is empty or inverted")
	}
}

// ariWindow fetches the ARI resource for a leaf and returns the suggested
// window with the Retry-After the server asked for.
func ariWindow(t *testing.T, e *testEnv, leaf *x509.Certificate) (time.Time,
	time.Time, int) {
	t.Helper()
	id, err := ariID(leaf)
	if err != nil {
		t.Fatal(err)
	}
	resp, err := http.Get(e.ts.URL + "/acme/renewal-info/" + id)
	if err != nil {
		t.Fatal(err)
	}
	defer resp.Body.Close()
	if resp.StatusCode != http.StatusOK {
		t.Fatalf("renewal-info: %d", resp.StatusCode)
	}
	var v struct {
		SuggestedWindow struct{ Start, End time.Time } `json:"suggestedWindow"`
	}
	if err := json.NewDecoder(resp.Body).Decode(&v); err != nil {
		t.Fatal(err)
	}
	retry, _ := strconv.Atoi(resp.Header.Get("Retry-After"))
	return v.SuggestedWindow.Start, v.SuggestedWindow.End, retry
}

// A revoked certificate, and one whose issuer an operator accelerated, are
// both told to replace now instead of at their policy age.
func TestRenewalInfoReplaceNow(t *testing.T) {
	e := newTestEnv(t)
	e.register()
	st := issueViaStub(t, e)

	policyStart, _, policyRetry := ariWindow(t, e, st.leaf)
	if !policyStart.After(time.Now().Add(24 * time.Hour)) {
		t.Fatalf("policy window opens too soon: %v", policyStart)
	}
	if policyRetry < int(time.Hour.Seconds()) {
		t.Errorf("policy Retry-After %ds, want at least an hour", policyRetry)
	}

	// Accelerating the issuer moves every certificate it signed.
	if err := e.s.db.SetAccel(st.leaf.Issuer.CommonName, 2*time.Hour); err != nil {
		t.Fatal(err)
	}
	start, end, retry := ariWindow(t, e, st.leaf)
	if start.After(time.Now().Add(time.Minute)) {
		t.Errorf("accelerated window has not opened: %v", start)
	}
	if d := end.Sub(start); d < 2*time.Hour-time.Minute ||
		d > 2*time.Hour+time.Minute {
		t.Errorf("accelerated window is %v, want the 2h asked for", d)
	}
	if retry > int(time.Hour.Seconds()) {
		t.Errorf("accelerated Retry-After %ds, want an hour at most", retry)
	}

	// An issuer nobody accelerated is unaffected, so the directive stops
	// applying the moment a certificate is re-issued by the successor.
	if err := e.s.db.SetAccel("Some Other CA", time.Hour); err != nil {
		t.Fatal(err)
	}
	if _, err := e.s.db.ClearAccel(st.leaf.Issuer.CommonName); err != nil {
		t.Fatal(err)
	}
	back, _, _ := ariWindow(t, e, st.leaf)
	if d := back.Sub(policyStart); d > time.Second || d < -time.Second {
		t.Errorf("window did not return to policy: %v vs %v", back, policyStart)
	}

	// Revocation alone is enough, with no directive in play.
	if err := e.s.db.MarkCertRevoked(
		strings.ToUpper(st.leaf.SerialNumber.Text(16)), "keyCompromise",
		time.Now()); err != nil {
		t.Fatal(err)
	}
	start, end, _ = ariWindow(t, e, st.leaf)
	if start.After(time.Now().Add(time.Minute)) {
		t.Errorf("revoked certificate not told to replace now: %v", start)
	}
	if d := end.Sub(start); d < revokedWindow-time.Minute {
		t.Errorf("revoked window is %v, want %v", d, revokedWindow)
	}
}

func TestRenewalInfoUnknown(t *testing.T) {
	e := newTestEnv(t)
	resp, err := http.Get(e.ts.URL + "/acme/renewal-info/bm9wZQ.AQAB")
	if err != nil {
		t.Fatal(err)
	}
	defer resp.Body.Close()
	if resp.StatusCode != http.StatusNotFound {
		t.Fatalf("unknown cert: %d", resp.StatusCode)
	}
}

func TestDirectoryAdvertisesPhase3(t *testing.T) {
	e := newTestEnv(t)
	resp, err := http.Get(e.ts.URL + "/acme/directory")
	if err != nil {
		t.Fatal(err)
	}
	defer resp.Body.Close()
	var dir map[string]any
	if err := json.NewDecoder(resp.Body).Decode(&dir); err != nil {
		t.Fatal(err)
	}
	for _, k := range []string{"revokeCert", "renewalInfo"} {
		if dir[k] == nil {
			t.Errorf("directory lacks %s: %v", k, dir)
		}
	}
}

package main

import (
	"encoding/json"
	"io"
	"math/big"
	"net/http"
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

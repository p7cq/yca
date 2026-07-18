// Challenge validation (RFC 8555 §8.3 http-01, §8.4 dns-01). Validation
// runs inline in the challenge POST - at private-CA scale there is nothing
// to queue, and clients poll the challenge/authz anyway.
package main

import (
	"context"
	"crypto/sha256"
	"encoding/base64"
	"encoding/json"
	"fmt"
	"io"
	"log"
	"net/http"
	"slices"
	"strings"
	"time"
)

type http01Validator struct {
	port int // where the identifier is expected to answer (80 in production)
}

// validate fetches http://<name>:<port>/.well-known/acme-challenge/<token>
// and compares the body with the expected key authorization.
func (v *http01Validator) validate(name, token, keyAuth string) error {
	client := &http.Client{Timeout: 10 * time.Second} // follows ≤10 redirects
	url := fmt.Sprintf("http://%s:%d/.well-known/acme-challenge/%s",
		name, v.port, token)
	resp, err := client.Get(url)
	if err != nil {
		return fmt.Errorf("fetch %s: %w", url, err)
	}
	defer resp.Body.Close()
	if resp.StatusCode != http.StatusOK {
		return fmt.Errorf("%s: HTTP %d", url, resp.StatusCode)
	}
	body, err := io.ReadAll(io.LimitReader(resp.Body, 4096))
	if err != nil {
		return fmt.Errorf("read %s: %w", url, err)
	}
	if got := strings.TrimSpace(string(body)); got != keyAuth {
		return fmt.Errorf("%s: key authorization mismatch", url)
	}
	return nil
}

// dns01Validator resolves TXT records; the lookup is injectable for tests
// and configurable (--dns) to point at the operator's own resolver.
type dns01Validator struct {
	lookupTXT func(ctx context.Context, name string) ([]string, error)
}

// validate checks _acme-challenge.<name> TXT against
// base64url(SHA-256(key authorization)) (RFC 8555 §8.4). For a wildcard
// authorization `name` is the base domain - the same record proves both.
func (v *dns01Validator) validate(name, keyAuth string) error {
	ctx, cancel := context.WithTimeout(context.Background(), 10*time.Second)
	defer cancel()
	digest := sha256.Sum256([]byte(keyAuth))
	want := base64.RawURLEncoding.EncodeToString(digest[:])
	fqdn := "_acme-challenge." + name
	records, err := v.lookupTXT(ctx, fqdn)
	if err != nil {
		return fmt.Errorf("TXT %s: %w", fqdn, err)
	}
	if slices.Contains(records, want) {
		return nil
	}
	return fmt.Errorf("TXT %s: no record matches the key authorization "+
		"digest (%d records seen)", fqdn, len(records))
}

func (s *server) handleChallenge(w http.ResponseWriter, r *http.Request) {
	req, p := s.verifyJWS(r, false)
	if p != nil {
		s.writeProblem(w, p)
		return
	}
	chall, err := s.db.ChallengeByID(r.PathValue("id"))
	if err != nil {
		s.writeProblem(w, problem(http.StatusInternalServerError,
			"serverInternal", "challenge lookup"))
		return
	}
	var authz *Authz
	if chall != nil {
		if authz, err = s.db.AuthzByID(chall.AuthzID); err != nil {
			s.writeProblem(w, problem(http.StatusInternalServerError,
				"serverInternal", "authz lookup"))
			return
		}
	}
	if chall == nil || authz == nil || authz.AccountID != req.account.ID {
		s.writeProblem(w, problem(http.StatusNotFound, "malformed",
			"no such challenge"))
		return
	}
	// RFC 8555 §7.5.1: challenge responses MUST link the parent authz
	// (certbot refuses to proceed without it; acme.sh does not care).
	w.Header().Set("Link",
		fmt.Sprintf(`<%s>;rel="up"`, s.url("/acme/authz/"+authz.ID)))

	// POST-as-GET polls the state; POST {} triggers validation (once).
	if req.postAsGet || chall.Status != "pending" {
		s.writeJSON(w, http.StatusOK, s.challengeJSON(chall))
		return
	}
	if time.Now().After(authz.Expires) {
		s.writeProblem(w, problem(http.StatusForbidden, "malformed",
			"authorization expired; submit a new order"))
		return
	}

	keyAuth := keyAuthorization(chall.Token, req.account)
	var verr error
	switch chall.Type {
	case "http-01":
		verr = s.http01.validate(authz.Identifier, chall.Token, keyAuth)
	case "dns-01":
		verr = s.dns01.validate(authz.Identifier, keyAuth)
	default:
		verr = fmt.Errorf("unsupported challenge type %q", chall.Type)
	}
	if err := verr; err != nil {
		log.Printf("challenge %s: %s invalid: %v", chall.ID, authz.Identifier, err)
		perr, _ := json.Marshal(problem(http.StatusForbidden,
			"unauthorized", err.Error()))
		_ = s.db.SetChallengeStatus(chall.ID, "invalid", string(perr))
		_ = s.db.SetAuthzStatus(authz.ID, "invalid")
		_ = s.db.SetOrderStatus(authz.OrderID, "invalid", "", string(perr))
		chall, _ = s.db.ChallengeByID(chall.ID)
		s.writeJSON(w, http.StatusOK, s.challengeJSON(chall))
		return
	}

	log.Printf("challenge %s: %s valid", chall.ID, authz.Identifier)
	_ = s.db.SetChallengeStatus(chall.ID, "valid", "")
	_ = s.db.SetAuthzStatus(authz.ID, "valid")
	// Order becomes ready once every authorization is valid.
	authzs, err := s.db.AuthzsByOrder(authz.OrderID)
	if err == nil {
		ready := true
		for _, a := range authzs {
			if a.Status != "valid" {
				ready = false
				break
			}
		}
		if ready {
			_ = s.db.SetOrderStatus(authz.OrderID, "ready", "", "")
		}
	}
	chall, _ = s.db.ChallengeByID(chall.ID)
	s.writeJSON(w, http.StatusOK, s.challengeJSON(chall))
}

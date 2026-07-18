// newAccount (RFC 8555 §7.3): registration is EAB-gated - no anonymous
// accounts. The EAB credential's --allow patterns become the account's
// identifier policy.
package main

import (
	"encoding/json"
	"io"
	"log"
	"net/http"
	"strings"
)

func writeIndentedJSON(w io.Writer, v any) error {
	enc := json.NewEncoder(w)
	enc.SetIndent("", "  ")
	return enc.Encode(v)
}

type newAccountPayload struct {
	Contact                []string        `json:"contact"`
	TermsOfServiceAgreed   bool            `json:"termsOfServiceAgreed"`
	OnlyReturnExisting     bool            `json:"onlyReturnExisting"`
	ExternalAccountBinding json.RawMessage `json:"externalAccountBinding"`
}

// accountJSON is the resource representation (RFC 8555 §7.1.2).
func (s *server) accountJSON(a *Account) map[string]any {
	return map[string]any{
		"status": a.Status,
		"orders": s.url("/acme/acct/" + a.ID + "/orders"), // not implemented; informative
	}
}

func (s *server) handleNewAccount(w http.ResponseWriter, r *http.Request) {
	req, p := s.verifyJWS(r, true /* jwk allowed: no account exists yet */)
	if p != nil {
		s.writeProblem(w, p)
		return
	}
	var payload newAccountPayload
	if err := json.Unmarshal(req.payload, &payload); err != nil {
		s.writeProblem(w, problem(http.StatusBadRequest, "malformed",
			"newAccount payload is not JSON"))
		return
	}

	tp, err := thumbprint(req.jwk)
	if err != nil {
		s.writeProblem(w, problem(http.StatusInternalServerError,
			"serverInternal", "thumbprint"))
		return
	}
	existing, err := s.db.AccountByThumbprint(tp)
	if err != nil {
		s.writeProblem(w, problem(http.StatusInternalServerError,
			"serverInternal", "account lookup"))
		return
	}
	if existing != nil { // idempotent re-registration
		w.Header().Set("Location", s.url("/acme/acct/"+existing.ID))
		s.writeJSON(w, http.StatusOK, s.accountJSON(existing))
		return
	}
	if payload.OnlyReturnExisting {
		s.writeProblem(w, problem(http.StatusBadRequest, "accountDoesNotExist",
			"no account is registered for this key"))
		return
	}

	cred, p := s.verifyEAB(payload.ExternalAccountBinding,
		s.url("/acme/new-account"), req.jwk)
	if p != nil {
		s.writeProblem(w, p)
		return
	}

	jwkJSON, err := json.Marshal(req.jwk)
	if err != nil {
		s.writeProblem(w, problem(http.StatusInternalServerError,
			"serverInternal", "account key"))
		return
	}
	acct := &Account{
		ID:         newID(),
		Thumbprint: tp,
		JWK:        string(jwkJSON),
		EABKid:     cred.KID,
		Status:     "valid",
	}
	if err := s.db.InsertAccount(acct); err != nil {
		s.writeProblem(w, problem(http.StatusInternalServerError,
			"serverInternal", "account insert"))
		return
	}
	log.Printf("account %s registered (eab %s, contact %s)", acct.ID,
		cred.KID, strings.Join(payload.Contact, " "))
	w.Header().Set("Location", s.url("/acme/acct/"+acct.ID))
	s.writeJSON(w, http.StatusCreated, s.accountJSON(acct))
}

// handleAccount serves POST-as-GET, deactivation (RFC 8555 §7.3.6) and
// no-op updates on the account object.
func (s *server) handleAccount(w http.ResponseWriter, r *http.Request) {
	req, p := s.verifyJWS(r, false)
	if p != nil {
		s.writeProblem(w, p)
		return
	}
	if req.account.ID != r.PathValue("id") {
		s.writeProblem(w, problem(http.StatusUnauthorized, "unauthorized",
			"account mismatch"))
		return
	}
	if !req.postAsGet {
		var payload struct {
			Status string `json:"status"`
		}
		if err := json.Unmarshal(req.payload, &payload); err != nil {
			s.writeProblem(w, problem(http.StatusBadRequest, "malformed",
				"account update payload is not JSON"))
			return
		}
		switch payload.Status {
		case "", "valid": // contact-style updates: nothing stored, no-op
		case "deactivated": // irreversible; every later JWS is refused
			if err := s.db.SetAccountStatus(req.account.ID, "deactivated"); err != nil {
				s.writeProblem(w, problem(http.StatusInternalServerError,
					"serverInternal", "account update"))
				return
			}
			req.account.Status = "deactivated"
			log.Printf("account %s deactivated", req.account.ID)
		default:
			s.writeProblem(w, problem(http.StatusBadRequest, "malformed",
				"status can only be set to 'deactivated'"))
			return
		}
	}
	s.writeJSON(w, http.StatusOK, s.accountJSON(req.account))
}

// allowed says whether an account's EAB policy permits ordering `name`.
// Patterns: exact name, or "*.suffix" (one leading wildcard label).
func (s *server) allowed(acct *Account, name string) bool {
	cred, err := s.db.GetEAB(acct.EABKid)
	if err != nil || cred == nil {
		return false
	}
	if len(cred.Allow) == 0 {
		return true
	}
	for _, pat := range cred.Allow {
		if pat == name {
			return true
		}
		if suffix, ok := strings.CutPrefix(pat, "*."); ok {
			// "*.example.ca" covers any depth under the suffix, but not
			// the bare suffix itself.
			if strings.HasSuffix(name, "."+suffix) {
				return true
			}
		}
	}
	return false
}

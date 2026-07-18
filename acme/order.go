// Orders and authorizations (RFC 8555 §7.4, §7.5). dns identifiers only,
// no wildcards (they require dns-01 - a later phase), and every identifier
// must pass the account's EAB --allow policy.
package main

import (
	"encoding/json"
	"log"
	"net/http"
	"strings"
	"time"
)

// orderTTL bounds how long a pending order (and its authorizations) may be
// worked on before the client must start over.
const orderTTL = 24 * time.Hour

type identifier struct {
	Type  string `json:"type"`
	Value string `json:"value"`
}

func (s *server) orderJSON(o *Order, authzs []*Authz) map[string]any {
	ids := make([]identifier, len(o.Identifiers))
	for i, v := range o.Identifiers {
		ids[i] = identifier{Type: "dns", Value: v}
	}
	urls := make([]string, len(authzs))
	for i, a := range authzs {
		urls[i] = s.url("/acme/authz/" + a.ID)
	}
	j := map[string]any{
		"status":         o.Status,
		"expires":        o.Expires.UTC().Format(time.RFC3339),
		"identifiers":    ids,
		"authorizations": urls,
		"finalize":       s.url("/acme/order/" + o.ID + "/finalize"),
	}
	if o.Status == "valid" && o.CertID != "" {
		j["certificate"] = s.url("/acme/cert/" + o.CertID)
	}
	if o.Error != "" {
		j["error"] = json.RawMessage(o.Error)
	}
	return j
}

func (s *server) handleNewOrder(w http.ResponseWriter, r *http.Request) {
	req, p := s.verifyJWS(r, false)
	if p != nil {
		s.writeProblem(w, p)
		return
	}
	var payload struct {
		Identifiers []identifier `json:"identifiers"`
	}
	if err := json.Unmarshal(req.payload, &payload); err != nil ||
		len(payload.Identifiers) == 0 {
		s.writeProblem(w, problem(http.StatusBadRequest, "malformed",
			"newOrder needs a non-empty identifiers array"))
		return
	}

	seen := map[string]bool{}
	var names []string
	for _, id := range payload.Identifiers {
		name := strings.ToLower(id.Value)
		// A single leading "*." label is the only wildcard form (RFC 8555
		// §7.1.3); it restricts the authz to dns-01.
		base, wildcard := strings.CutPrefix(name, "*.")
		switch {
		case id.Type != "dns":
			s.writeProblem(w, problem(http.StatusBadRequest,
				"unsupportedIdentifier", "only dns identifiers are supported"))
			return
		case base == "" || !hostSafe(base) || strings.Contains(base, "*"):
			s.writeProblem(w, problem(http.StatusBadRequest, "malformed",
				"invalid dns identifier: "+id.Value))
			return
		case wildcard && s.dns01 == nil:
			s.writeProblem(w, problem(http.StatusBadRequest,
				"unsupportedIdentifier", "wildcards require dns-01"))
			return
		case !s.allowed(req.account, name):
			s.writeProblem(w, problem(http.StatusForbidden,
				"rejectedIdentifier",
				name+" is outside this account's allowed identifiers"))
			return
		}
		if !seen[name] {
			seen[name] = true
			names = append(names, name)
		}
	}

	order := &Order{
		ID:          newID(),
		AccountID:   req.account.ID,
		Status:      "pending",
		Identifiers: names,
		Expires:     time.Now().Add(orderTTL),
	}
	if err := s.db.InsertOrder(order); err != nil {
		s.writeProblem(w, problem(http.StatusInternalServerError,
			"serverInternal", "order insert"))
		return
	}
	var authzs []*Authz
	for _, name := range names {
		// The wildcard authz carries the BASE domain plus a flag (RFC 8555
		// §7.1.4); dns-01 validates _acme-challenge.<base> either way.
		base, wildcard := strings.CutPrefix(name, "*.")
		a := &Authz{
			ID:         newID(),
			OrderID:    order.ID,
			AccountID:  req.account.ID,
			Identifier: base,
			Status:     "pending",
			Expires:    order.Expires,
			Wildcard:   wildcard,
		}
		if err := s.db.InsertAuthz(a); err != nil {
			s.writeProblem(w, problem(http.StatusInternalServerError,
				"serverInternal", "authz insert"))
			return
		}
		types := []string{"http-01", "dns-01"}
		if wildcard {
			types = []string{"dns-01"} // the only proof for a whole subtree
		}
		for _, ct := range types {
			c := &Challenge{
				ID:      newID(),
				AuthzID: a.ID,
				Type:    ct,
				Token:   newID(),
				Status:  "pending",
			}
			if err := s.db.InsertChallenge(c); err != nil {
				s.writeProblem(w, problem(http.StatusInternalServerError,
					"serverInternal", "challenge insert"))
				return
			}
		}
		authzs = append(authzs, a)
	}
	log.Printf("order %s: account %s, identifiers %s", order.ID,
		req.account.ID, strings.Join(names, " "))
	w.Header().Set("Location", s.url("/acme/order/"+order.ID))
	s.writeJSON(w, http.StatusCreated, s.orderJSON(order, authzs))
}

// loadOwnedOrder fetches an order and enforces account ownership.
func (s *server) loadOwnedOrder(req *request,
	id string) (*Order, []*Authz, *problemErr) {
	order, err := s.db.OrderByID(id)
	if err != nil {
		return nil, nil, problem(http.StatusInternalServerError,
			"serverInternal", "order lookup")
	}
	if order == nil || order.AccountID != req.account.ID {
		return nil, nil, problem(http.StatusNotFound, "malformed",
			"no such order")
	}
	authzs, err := s.db.AuthzsByOrder(order.ID)
	if err != nil {
		return nil, nil, problem(http.StatusInternalServerError,
			"serverInternal", "authz lookup")
	}
	return order, authzs, nil
}

func (s *server) handleOrder(w http.ResponseWriter, r *http.Request) {
	req, p := s.verifyJWS(r, false)
	if p != nil {
		s.writeProblem(w, p)
		return
	}
	order, authzs, p := s.loadOwnedOrder(req, r.PathValue("id"))
	if p != nil {
		s.writeProblem(w, p)
		return
	}
	s.writeJSON(w, http.StatusOK, s.orderJSON(order, authzs))
}

func (s *server) authzJSON(a *Authz, challs []*Challenge) map[string]any {
	cs := make([]map[string]any, len(challs))
	for i, c := range challs {
		cs[i] = s.challengeJSON(c)
	}
	j := map[string]any{
		"identifier": identifier{Type: "dns", Value: a.Identifier},
		"status":     a.Status,
		"expires":    a.Expires.UTC().Format(time.RFC3339),
		"challenges": cs,
	}
	if a.Wildcard {
		j["wildcard"] = true
	}
	return j
}

func (s *server) challengeJSON(c *Challenge) map[string]any {
	j := map[string]any{
		"type":   c.Type,
		"url":    s.url("/acme/chall/" + c.ID),
		"token":  c.Token,
		"status": c.Status,
	}
	if c.Status == "valid" {
		j["validated"] = c.Validated.UTC().Format(time.RFC3339)
	}
	if c.Error != "" {
		j["error"] = json.RawMessage(c.Error)
	}
	return j
}

func (s *server) handleAuthz(w http.ResponseWriter, r *http.Request) {
	req, p := s.verifyJWS(r, false)
	if p != nil {
		s.writeProblem(w, p)
		return
	}
	authz, err := s.db.AuthzByID(r.PathValue("id"))
	if err != nil {
		s.writeProblem(w, problem(http.StatusInternalServerError,
			"serverInternal", "authz lookup"))
		return
	}
	if authz == nil || authz.AccountID != req.account.ID {
		s.writeProblem(w, problem(http.StatusNotFound, "malformed",
			"no such authorization"))
		return
	}
	challs, err := s.db.ChallengesByAuthz(authz.ID)
	if err != nil {
		s.writeProblem(w, problem(http.StatusInternalServerError,
			"serverInternal", "challenge lookup"))
		return
	}
	s.writeJSON(w, http.StatusOK, s.authzJSON(authz, challs))
}

// hostSafe mirrors the CA-side dns_safe rule minus '*' (wildcards are
// rejected earlier): ASCII hostname characters only, IDN as punycode.
func hostSafe(s string) bool {
	for _, c := range s {
		switch {
		case c >= 'a' && c <= 'z', c >= '0' && c <= '9', c == '.', c == '-':
		default:
			return false
		}
	}
	return true
}

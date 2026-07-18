// keyChange (RFC 8555 §7.3.5): account key rollover. The outer JWS is a
// normal account-authenticated POST (old key, kid, nonce); its payload is
// an inner JWS signed by the NEW key, proving its holder requested the
// change: jwk (the new key), the same url, no nonce, over
// {"account": <account URL>, "oldKey": <current JWK>}.
package main

import (
	"encoding/json"
	"log"
	"net/http"

	jose "github.com/go-jose/go-jose/v4"
)

type keyChangePayload struct {
	Account string          `json:"account"`
	OldKey  json.RawMessage `json:"oldKey"`
}

func (s *server) handleKeyChange(w http.ResponseWriter, r *http.Request) {
	req, p := s.verifyJWS(r, false)
	if p != nil {
		s.writeProblem(w, p)
		return
	}

	inner, err := jose.ParseSigned(string(req.payload), accountAlgs)
	if err != nil || len(inner.Signatures) != 1 {
		s.writeProblem(w, problem(http.StatusBadRequest, "malformed",
			"keyChange payload is not a single-signature JWS"))
		return
	}
	hdr := inner.Signatures[0].Header
	newKey := hdr.JSONWebKey
	switch { // §7.3.5: jwk required, same url, and no nonce on the inner JWS
	case newKey == nil || hdr.KeyID != "":
		s.writeProblem(w, problem(http.StatusBadRequest, "malformed",
			"inner JWS must carry the new key as jwk"))
		return
	case hdr.Nonce != "":
		s.writeProblem(w, problem(http.StatusBadRequest, "malformed",
			"inner JWS must not have a nonce"))
		return
	}
	if u, _ := hdr.ExtraHeaders[jose.HeaderKey("url")].(string); u != s.url(r.URL.Path) {
		s.writeProblem(w, problem(http.StatusBadRequest, "malformed",
			"inner JWS url does not match the request"))
		return
	}
	payload, err := inner.Verify(newKey)
	if err != nil {
		s.writeProblem(w, problem(http.StatusUnauthorized, "unauthorized",
			"inner JWS verification failed"))
		return
	}

	var kc keyChangePayload
	if err := json.Unmarshal(payload, &kc); err != nil {
		s.writeProblem(w, problem(http.StatusBadRequest, "malformed",
			"keyChange object is not JSON"))
		return
	}
	if kc.Account != s.url("/acme/acct/"+req.account.ID) {
		s.writeProblem(w, problem(http.StatusBadRequest, "malformed",
			"keyChange account does not match the outer JWS account"))
		return
	}
	var oldKey jose.JSONWebKey
	if err := json.Unmarshal(kc.OldKey, &oldKey); err != nil {
		s.writeProblem(w, problem(http.StatusBadRequest, "malformed",
			"keyChange oldKey is not a JWK"))
		return
	}
	oldTP, err := thumbprint(&oldKey)
	if err != nil || oldTP != req.account.Thumbprint {
		s.writeProblem(w, problem(http.StatusBadRequest, "malformed",
			"keyChange oldKey does not match the account key"))
		return
	}

	newTP, err := thumbprint(newKey)
	if err != nil {
		s.writeProblem(w, problem(http.StatusInternalServerError,
			"serverInternal", "thumbprint"))
		return
	}
	if newTP == oldTP {
		s.writeProblem(w, problem(http.StatusBadRequest, "malformed",
			"new key equals the current account key"))
		return
	}
	// §7.3.5: a key already bound to another account is a conflict, and the
	// response names the holder.
	other, err := s.db.AccountByThumbprint(newTP)
	if err != nil {
		s.writeProblem(w, problem(http.StatusInternalServerError,
			"serverInternal", "account lookup"))
		return
	}
	if other != nil {
		w.Header().Set("Location", s.url("/acme/acct/"+other.ID))
		s.writeProblem(w, problem(http.StatusConflict, "malformed",
			"the new key is already bound to an account"))
		return
	}

	jwkJSON, err := json.Marshal(newKey)
	if err != nil {
		s.writeProblem(w, problem(http.StatusInternalServerError,
			"serverInternal", "account key"))
		return
	}
	if err := s.db.SetAccountKey(req.account.ID, string(jwkJSON), newTP); err != nil {
		s.writeProblem(w, problem(http.StatusInternalServerError,
			"serverInternal", "account update"))
		return
	}
	req.account.Thumbprint = newTP
	log.Printf("account %s rolled its key over", req.account.ID)
	s.writeJSON(w, http.StatusOK, s.accountJSON(req.account))
}

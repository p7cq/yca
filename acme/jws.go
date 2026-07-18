// JWS handling (RFC 8555 §6): every ACME POST body is a JWS whose protected
// header binds an anti-replay nonce and the request URL, and carries either
// the full account key (jwk, newAccount only) or the account URL (kid).
package main

import (
	"crypto"
	"encoding/base64"
	"encoding/json"
	"fmt"
	"io"
	"log"
	"net/http"
	"strings"

	jose "github.com/go-jose/go-jose/v4"
)

// Account keys we accept. HS256 appears separately, only for the inner
// External Account Binding JWS.
var accountAlgs = []jose.SignatureAlgorithm{jose.ES256, jose.ES384, jose.RS256}

// problemErr is an RFC 7807 problem document with the ACME error namespace.
type problemErr struct {
	Status int    `json:"status"`
	Type   string `json:"type"`
	Detail string `json:"detail"`
}

func problem(status int, code, detail string) *problemErr {
	return &problemErr{Status: status,
		Type: "urn:ietf:params:acme:error:" + code, Detail: detail}
}

// request is a verified ACME POST.
type request struct {
	account   *Account         // nil only on newAccount
	jwk       *jose.JSONWebKey // the verified signing key
	payload   []byte           // decoded JWS payload ("" = POST-as-GET)
	postAsGet bool
}

// writeJSON emits an ACME resource (or problem document) with a fresh
// Replay-Nonce, as every ACME response must carry one.
func (s *server) writeJSON(w http.ResponseWriter, status int, v any) {
	nonce, err := s.db.IssueNonce()
	if err != nil {
		log.Printf("nonce: %v", err)
		http.Error(w, "internal error", http.StatusInternalServerError)
		return
	}
	w.Header().Set("Replay-Nonce", nonce)
	ct := "application/json"
	if _, isProblem := v.(*problemErr); isProblem {
		ct = "application/problem+json"
	}
	w.Header().Set("Content-Type", ct)
	w.WriteHeader(status)
	if v != nil {
		_ = json.NewEncoder(w).Encode(v)
	}
}

func (s *server) writeProblem(w http.ResponseWriter, p *problemErr) {
	s.writeJSON(w, p.Status, p)
}

// url reconstructs the canonical external URL of a request path - the value
// the client must have put in the protected "url" header. The daemon may sit
// behind a proxy, so the base comes from --url, not from the socket.
func (s *server) url(path string) string {
	return strings.TrimSuffix(s.baseURL, "/") + path
}

// verifyJWS authenticates an ACME POST: signature, single-use nonce, URL
// binding, and jwk-vs-kid discipline. When allowJWK is false (everything but
// newAccount) the request must reference an existing account via kid.
func (s *server) verifyJWS(r *http.Request, allowJWK bool) (*request, *problemErr) {
	body, err := io.ReadAll(io.LimitReader(r.Body, 1<<20))
	if err != nil {
		return nil, problem(http.StatusBadRequest, "malformed", "unreadable body")
	}
	jws, err := jose.ParseSigned(string(body), accountAlgs)
	if err != nil {
		return nil, problem(http.StatusBadRequest, "malformed",
			"body is not a JWS: "+err.Error())
	}
	if len(jws.Signatures) != 1 {
		return nil, problem(http.StatusBadRequest, "malformed",
			"exactly one JWS signature required")
	}
	hdr := jws.Signatures[0].Header

	// Anti-replay: the nonce must be one we issued, and dies on use.
	if hdr.Nonce == "" {
		return nil, problem(http.StatusBadRequest, "badNonce", "missing nonce")
	}
	ok, err := s.db.ConsumeNonce(hdr.Nonce)
	if err != nil {
		return nil, problem(http.StatusInternalServerError, "serverInternal",
			"nonce store")
	}
	if !ok {
		return nil, problem(http.StatusBadRequest, "badNonce",
			"unknown or reused nonce")
	}

	// URL binding: the protected header must name this exact resource.
	rawURL, _ := hdr.ExtraHeaders[jose.HeaderKey("url")].(string)
	if rawURL != s.url(r.URL.Path) {
		return nil, problem(http.StatusUnauthorized, "unauthorized",
			fmt.Sprintf("url header %q does not match %q", rawURL,
				s.url(r.URL.Path)))
	}

	// jwk XOR kid.
	req := &request{}
	switch {
	case hdr.JSONWebKey != nil && hdr.KeyID != "":
		return nil, problem(http.StatusBadRequest, "malformed",
			"jwk and kid are mutually exclusive")
	case hdr.JSONWebKey != nil:
		if !allowJWK {
			return nil, problem(http.StatusBadRequest, "malformed",
				"this resource requires kid, not jwk")
		}
		req.jwk = hdr.JSONWebKey
	case hdr.KeyID != "":
		acctPrefix := s.url("/acme/acct/")
		if !strings.HasPrefix(hdr.KeyID, acctPrefix) {
			return nil, problem(http.StatusBadRequest, "malformed",
				"kid is not an account URL of this server")
		}
		acct, err := s.db.AccountByID(strings.TrimPrefix(hdr.KeyID, acctPrefix))
		if err != nil {
			return nil, problem(http.StatusInternalServerError, "serverInternal",
				"account lookup")
		}
		if acct == nil {
			return nil, problem(http.StatusUnauthorized, "accountDoesNotExist",
				"unknown account")
		}
		if acct.Status != "valid" {
			return nil, problem(http.StatusUnauthorized, "unauthorized",
				"account is "+acct.Status)
		}
		var jwk jose.JSONWebKey
		if err := json.Unmarshal([]byte(acct.JWK), &jwk); err != nil {
			return nil, problem(http.StatusInternalServerError, "serverInternal",
				"stored account key")
		}
		req.account = acct
		req.jwk = &jwk
	default:
		return nil, problem(http.StatusBadRequest, "malformed",
			"one of jwk or kid is required")
	}

	payload, err := jws.Verify(req.jwk)
	if err != nil {
		return nil, problem(http.StatusUnauthorized, "unauthorized",
			"JWS verification failed")
	}
	req.payload = payload
	req.postAsGet = len(payload) == 0
	return req, nil
}

// verifyEAB checks the External Account Binding of a newAccount request: an
// inner HS256 JWS over the account key, signed with a provisioned HMAC
// credential (RFC 8555 §7.3.4). Returns the credential on success.
func (s *server) verifyEAB(raw json.RawMessage, outerURL string,
	acctKey *jose.JSONWebKey) (*EABCred, *problemErr) {
	if len(raw) == 0 {
		return nil, problem(http.StatusBadRequest, "externalAccountRequired",
			"this CA requires External Account Binding credentials")
	}
	eab, err := jose.ParseSigned(string(raw),
		[]jose.SignatureAlgorithm{jose.HS256})
	if err != nil || len(eab.Signatures) != 1 {
		return nil, problem(http.StatusBadRequest, "malformed",
			"externalAccountBinding is not a HS256 JWS")
	}
	hdr := eab.Signatures[0].Header
	if u, _ := hdr.ExtraHeaders[jose.HeaderKey("url")].(string); u != outerURL {
		return nil, problem(http.StatusBadRequest, "malformed",
			"EAB url header does not match the request")
	}
	cred, err := s.db.GetEAB(hdr.KeyID)
	if err != nil {
		return nil, problem(http.StatusInternalServerError, "serverInternal",
			"EAB lookup")
	}
	if cred == nil {
		return nil, problem(http.StatusUnauthorized, "unauthorized",
			"unknown EAB key id")
	}
	payload, err := eab.Verify(cred.HMAC)
	if err != nil {
		return nil, problem(http.StatusUnauthorized, "unauthorized",
			"EAB verification failed")
	}
	// The inner payload must be the account key itself: same thumbprint.
	var inner jose.JSONWebKey
	if err := json.Unmarshal(payload, &inner); err != nil {
		return nil, problem(http.StatusBadRequest, "malformed",
			"EAB payload is not a JWK")
	}
	it, err1 := inner.Thumbprint(crypto.SHA256)
	ot, err2 := acctKey.Thumbprint(crypto.SHA256)
	if err1 != nil || err2 != nil || string(it) != string(ot) {
		return nil, problem(http.StatusUnauthorized, "unauthorized",
			"EAB payload key does not match the account key")
	}
	return cred, nil
}

func thumbprint(k *jose.JSONWebKey) (string, error) {
	tp, err := k.Thumbprint(crypto.SHA256)
	if err != nil {
		return "", err
	}
	return base64.RawURLEncoding.EncodeToString(tp), nil
}

// keyAuthorization is the http-01 proof: token.thumbprint(accountKey).
func keyAuthorization(token string, acct *Account) string {
	return token + "." + acct.Thumbprint
}

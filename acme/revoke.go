// revokeCert (RFC 8555 §7.6): the client presents the certificate to
// revoke and proves control of either the account that ordered it (kid) or
// the certificate's own private key (jwk). Revocation execs
// `yca revoke server --serial <hex>` - the serial names the EXACT
// certificate, which by-CN cannot do during a renewal overlap.
package main

import (
	"bytes"
	"crypto/x509"
	"encoding/base64"
	"encoding/json"
	"log"
	"net/http"
	"strings"
)

// Accepted CRLReason codes (RFC 5280) and their yca --reason names. The
// rest (e.g. cACompromise) make no sense for an EE holder to claim.
var revokeReasons = map[int]string{
	0: "unspecified",
	1: "keyCompromise",
	4: "superseded",
	5: "cessationOfOperation",
}

func (s *server) handleRevokeCert(w http.ResponseWriter, r *http.Request) {
	req, p := s.verifyJWS(r, true /* jwk allowed: proof by certificate key */)
	if p != nil {
		s.writeProblem(w, p)
		return
	}
	var payload struct {
		Certificate string `json:"certificate"`
		Reason      *int   `json:"reason"`
	}
	if err := json.Unmarshal(req.payload, &payload); err != nil {
		s.writeProblem(w, problem(http.StatusBadRequest, "malformed",
			"revokeCert payload is not JSON"))
		return
	}
	der, err := base64.RawURLEncoding.DecodeString(payload.Certificate)
	if err != nil {
		s.writeProblem(w, problem(http.StatusBadRequest, "malformed",
			"certificate is not base64url"))
		return
	}
	cert, err := x509.ParseCertificate(der)
	if err != nil {
		s.writeProblem(w, problem(http.StatusBadRequest, "malformed",
			"unparsable certificate"))
		return
	}
	reason := 0
	if payload.Reason != nil {
		reason = *payload.Reason
	}
	reasonName, ok := revokeReasons[reason]
	if !ok {
		s.writeProblem(w, problem(http.StatusBadRequest, "badRevocationReason",
			"allowed reasons: 0, 1, 4, 5"))
		return
	}
	serial := strings.ToUpper(cert.SerialNumber.Text(16))

	// Authorization: the ordering account, or the certificate key itself.
	if req.account != nil {
		owned, err := s.db.CertByAccountSerial(req.account.ID, serial)
		if err != nil {
			s.writeProblem(w, problem(http.StatusInternalServerError,
				"serverInternal", "certificate lookup"))
			return
		}
		if owned == nil {
			s.writeProblem(w, problem(http.StatusForbidden, "unauthorized",
				"this account did not order that certificate"))
			return
		}
	} else {
		// jwk path: the JWS key must be the certificate's public key.
		got, err1 := x509.MarshalPKIXPublicKey(req.jwk.Key)
		want, err2 := x509.MarshalPKIXPublicKey(cert.PublicKey)
		if err1 != nil || err2 != nil || !bytes.Equal(got, want) {
			s.writeProblem(w, problem(http.StatusForbidden, "unauthorized",
				"JWS key is not the certificate key"))
			return
		}
	}

	if err := s.yca.revoke(serial, reasonName); err != nil {
		if strings.Contains(err.Error(), "no active") {
			s.writeProblem(w, problem(http.StatusBadRequest, "alreadyRevoked",
				"certificate is not active (revoked or expired)"))
			return
		}
		log.Printf("revokeCert serial %s: %v", serial, err)
		s.writeProblem(w, problem(http.StatusInternalServerError,
			"serverInternal", "revocation failed"))
		return
	}
	log.Printf("revoked serial %s (reason %s)", serial, reasonName)
	s.writeJSON(w, http.StatusOK, nil)
}

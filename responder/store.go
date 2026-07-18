package main

import (
	"crypto"
	"crypto/x509"
	"database/sql"
	"encoding/pem"
	"errors"
	"fmt"
	"log"
	"os"
	"path/filepath"
	"sync"
	"time"

	_ "github.com/mattn/go-sqlite3"
)

// Store is read-only access to a yca store directory: cert_index lookups plus
// the file credentials - the signing CA (issuer) certificate and the delegated
// responder certificate + key.
type Store struct {
	db  *sql.DB
	dir string

	Issuer *x509.Certificate // the one signing CA (single-issuer, phase 1)

	// Responder credentials, reloaded when the cert file changes (rotation
	// via `yca create ocsp`.
	credMu            sync.Mutex
	cred              Credentials
	credTime          time.Time // cert file mtime at last load
	certPath, keyPath string
}

// Credentials are the delegated responder certificate and its signing key.
type Credentials struct {
	Cert *x509.Certificate
	Key  crypto.Signer
}

// CertStatus is the status projection of one cert_index row.
type CertStatus struct {
	Status    string // 'active' | 'revoked'
	RevokedAt int64  // epoch seconds; 0 when not revoked
	Reason    int    // CRLReason code
}

// OpenStore opens <dir>/ca-store.db read-only, discovers the slugs from
// ca_config and loads the issuer certificate and responder credentials.
func OpenStore(dir string) (*Store, error) {
	dbPath := filepath.Join(dir, "ca-store.db")
	if _, err := os.Stat(dbPath); err != nil {
		return nil, fmt.Errorf("store not initialized? %w", err)
	}
	// Read-only connection with the writer's busy_timeout (store contract).
	db, err := sql.Open("sqlite3", "file:"+dbPath+"?mode=ro&_busy_timeout=5000")
	if err != nil {
		return nil, err
	}
	s := &Store{db: db, dir: dir}

	signingSlug, err := s.configValue("signing_ca_slug")
	if err != nil {
		db.Close()
		return nil, fmt.Errorf("ca_config signing_ca_slug: %w", err)
	}
	ocspSlug, err := s.configValue("ocsp_slug")
	if err != nil {
		db.Close()
		return nil, fmt.Errorf("ca_config ocsp_slug: %w", err)
	}

	s.Issuer, err = loadCert(filepath.Join(dir, "ca", signingSlug+".pem"))
	if err != nil {
		db.Close()
		return nil, fmt.Errorf("signing CA certificate: %w", err)
	}

	s.certPath = filepath.Join(dir, "ocsp", ocspSlug+".crt")
	s.keyPath = filepath.Join(dir, "ocsp", ocspSlug+".key")
	fi, err := os.Stat(s.certPath)
	if err == nil {
		s.cred, err = loadCredentials(s.certPath, s.keyPath)
	}
	if err != nil {
		db.Close()
		return nil, fmt.Errorf(
			"responder credentials (run 'yca create ocsp'?): %w", err)
	}
	s.credTime = fi.ModTime()
	return s, nil
}

func (s *Store) Close() error { return s.db.Close() }

// Lookup returns the status for a serial (uppercase minimal hex, see the
// store contract); found is false when no such serial was ever issued.
func (s *Store) Lookup(serial string) (CertStatus, bool, error) {
	var cs CertStatus
	err := s.db.QueryRow(
		"SELECT status, revoked_at, reason FROM cert_index "+
			"WHERE serial = ? LIMIT 1", serial).
		Scan(&cs.Status, &cs.RevokedAt, &cs.Reason)
	if errors.Is(err, sql.ErrNoRows) {
		return cs, false, nil
	}
	if err != nil {
		return cs, false, err
	}
	return cs, true, nil
}

// Creds returns the responder credentials, transparently reloading them when
// the certificate file changes. On reload failure the previous credentials
// are kept (and the error logged) - a half-done rotation must not take the
// responder down.
func (s *Store) Creds() Credentials {
	s.credMu.Lock()
	defer s.credMu.Unlock()
	if fi, err := os.Stat(s.certPath); err == nil &&
		!fi.ModTime().Equal(s.credTime) {
		if c, err := loadCredentials(s.certPath, s.keyPath); err != nil {
			log.Printf("credential reload: %v (keeping previous)", err)
		} else {
			s.cred = c
			s.credTime = fi.ModTime()
			log.Printf("responder credentials reloaded (notAfter %s)",
				c.Cert.NotAfter.Format(time.RFC3339))
		}
	}
	return s.cred
}

// Healthy reports whether the DB answers queries and the responder
// credential files are still readable.
func (s *Store) Healthy() error {
	var n int
	if err := s.db.QueryRow("SELECT COUNT(*) FROM cert_index").Scan(&n); err != nil {
		return fmt.Errorf("cert_index: %w", err)
	}
	for _, p := range []string{s.certPath, s.keyPath} {
		if _, err := os.Stat(p); err != nil {
			return err
		}
	}
	return nil
}

func (s *Store) configValue(key string) (string, error) {
	var v string
	err := s.db.QueryRow("SELECT value FROM ca_config WHERE key = ?", key).
		Scan(&v)
	if err != nil {
		return "", err
	}
	if v == "" {
		return "", fmt.Errorf("ca_config %s is empty", key)
	}
	return v, nil
}

func loadCert(path string) (*x509.Certificate, error) {
	raw, err := os.ReadFile(path)
	if err != nil {
		return nil, err
	}
	block, _ := pem.Decode(raw)
	if block == nil || block.Type != "CERTIFICATE" {
		return nil, fmt.Errorf("%s: no CERTIFICATE PEM block", path)
	}
	return x509.ParseCertificate(block.Bytes)
}

// loadCredentials loads the responder certificate and its unencrypted
// PKCS#8 key, and verifies they belong together.
func loadCredentials(certPath, keyPath string) (Credentials, error) {
	cert, err := loadCert(certPath)
	if err != nil {
		return Credentials{}, err
	}
	raw, err := os.ReadFile(keyPath)
	if err != nil {
		return Credentials{}, err
	}
	block, _ := pem.Decode(raw)
	if block == nil || block.Type != "PRIVATE KEY" {
		return Credentials{}, fmt.Errorf("%s: no PRIVATE KEY PEM block", keyPath)
	}
	parsed, err := x509.ParsePKCS8PrivateKey(block.Bytes)
	if err != nil {
		return Credentials{}, fmt.Errorf("%s: %w", keyPath, err)
	}
	key, ok := parsed.(crypto.Signer)
	if !ok {
		return Credentials{}, fmt.Errorf("%s: not a signing key", keyPath)
	}
	pub, ok := cert.PublicKey.(interface{ Equal(crypto.PublicKey) bool })
	if !ok || !pub.Equal(key.Public()) {
		return Credentials{}, fmt.Errorf("%s does not match %s", keyPath, certPath)
	}
	return Credentials{Cert: cert, Key: key}, nil
}

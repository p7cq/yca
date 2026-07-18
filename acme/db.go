// ACME protocol state. This database belongs to the frontend alone - CA
// state lives in the yca store, which yca-acme never opens (issuance goes
// through the yca CLI; see finalize.go.
package main

import (
	"crypto/rand"
	"database/sql"
	"encoding/base64"
	"encoding/json"
	"fmt"
	"time"

	_ "github.com/mattn/go-sqlite3"
)

const schema = `
CREATE TABLE IF NOT EXISTS nonces (
  value TEXT PRIMARY KEY, created INTEGER NOT NULL);
CREATE TABLE IF NOT EXISTS eab_creds (
  kid TEXT PRIMARY KEY, hmac TEXT NOT NULL, allow TEXT NOT NULL,
  created INTEGER NOT NULL);
CREATE TABLE IF NOT EXISTS accounts (
  id TEXT PRIMARY KEY, thumbprint TEXT NOT NULL UNIQUE, jwk TEXT NOT NULL,
  eab_kid TEXT NOT NULL, status TEXT NOT NULL, created INTEGER NOT NULL);
CREATE TABLE IF NOT EXISTS orders (
  id TEXT PRIMARY KEY, account_id TEXT NOT NULL, status TEXT NOT NULL,
  identifiers TEXT NOT NULL, expires INTEGER NOT NULL,
  cert_id TEXT NOT NULL DEFAULT '', error TEXT NOT NULL DEFAULT '');
CREATE TABLE IF NOT EXISTS authzs (
  id TEXT PRIMARY KEY, order_id TEXT NOT NULL, account_id TEXT NOT NULL,
  identifier TEXT NOT NULL, status TEXT NOT NULL, expires INTEGER NOT NULL,
  wildcard INTEGER NOT NULL DEFAULT 0);
CREATE TABLE IF NOT EXISTS challenges (
  id TEXT PRIMARY KEY, authz_id TEXT NOT NULL, type TEXT NOT NULL,
  token TEXT NOT NULL, status TEXT NOT NULL,
  validated INTEGER NOT NULL DEFAULT 0, error TEXT NOT NULL DEFAULT '');
CREATE TABLE IF NOT EXISTS certs (
  id TEXT PRIMARY KEY, order_id TEXT NOT NULL, account_id TEXT NOT NULL,
  cn TEXT NOT NULL, chain_pem TEXT NOT NULL, issued INTEGER NOT NULL,
  serial TEXT NOT NULL DEFAULT '', ari_id TEXT NOT NULL DEFAULT '',
  nb INTEGER NOT NULL DEFAULT 0, na INTEGER NOT NULL DEFAULT 0);
`

// nonceTTL bounds replay-nonce (and pending object) lifetimes; expired
// nonces are garbage-collected opportunistically on issue.
const nonceTTL = 30 * time.Minute

type DB struct{ sql *sql.DB }

func OpenDB(path string) (*DB, error) {
	h, err := sql.Open("sqlite3", path+"?_busy_timeout=5000")
	if err != nil {
		return nil, err
	}
	h.SetMaxOpenConns(1) // single writer; the daemon is the only client
	if _, err := h.Exec(schema); err != nil {
		return nil, fmt.Errorf("schema: %w", err)
	}
	d := &DB{sql: h}
	// Self-heal for databases created by earlier versions (CREATE TABLE IF
	// NOT EXISTS does not grow existing tables), like the CA's cert_index.
	for _, m := range []struct{ table, column, ddl string }{
		{"authzs", "wildcard", "INTEGER NOT NULL DEFAULT 0"},
		{"certs", "serial", "TEXT NOT NULL DEFAULT ''"},
		{"certs", "ari_id", "TEXT NOT NULL DEFAULT ''"},
		{"certs", "nb", "INTEGER NOT NULL DEFAULT 0"},
		{"certs", "na", "INTEGER NOT NULL DEFAULT 0"},
	} {
		if err := d.ensureColumn(m.table, m.column, m.ddl); err != nil {
			return nil, err
		}
	}
	return d, nil
}

func (d *DB) ensureColumn(table, column, ddl string) error {
	rows, err := d.sql.Query("PRAGMA table_info(" + table + ")")
	if err != nil {
		return err
	}
	defer rows.Close()
	for rows.Next() {
		var cid int
		var name, ctype string
		var notnull, pk int
		var dflt any
		if err := rows.Scan(&cid, &name, &ctype, &notnull, &dflt, &pk); err != nil {
			return err
		}
		if name == column {
			return nil
		}
	}
	if err := rows.Err(); err != nil {
		return err
	}
	_, err = d.sql.Exec(fmt.Sprintf("ALTER TABLE %s ADD COLUMN %s %s",
		table, column, ddl))
	return err
}

func (d *DB) Close() error { return d.sql.Close() }

// newID returns a 128-bit random URL-safe identifier (also used for
// tokens and nonces).
func newID() string {
	raw := make([]byte, 16)
	if _, err := rand.Read(raw); err != nil {
		panic(err) // crypto/rand failure is not a recoverable state
	}
	return base64.RawURLEncoding.EncodeToString(raw)
}

// --- nonces ---

func (d *DB) IssueNonce() (string, error) {
	n := newID()
	if _, err := d.sql.Exec("DELETE FROM nonces WHERE created < ?",
		time.Now().Add(-nonceTTL).Unix()); err != nil {
		return "", err
	}
	_, err := d.sql.Exec("INSERT INTO nonces (value, created) VALUES (?, ?)",
		n, time.Now().Unix())
	return n, err
}

// ConsumeNonce returns true when the nonce existed (single use).
func (d *DB) ConsumeNonce(n string) (bool, error) {
	res, err := d.sql.Exec("DELETE FROM nonces WHERE value = ?", n)
	if err != nil {
		return false, err
	}
	c, err := res.RowsAffected()
	return c > 0, err
}

// --- EAB credentials ---

type EABCred struct {
	KID   string
	HMAC  []byte   // raw key (stored base64url)
	Allow []string // identifier suffix patterns, e.g. "*.example.ca"
}

func (d *DB) InsertEAB(kid string, hmac []byte, allow string) error {
	_, err := d.sql.Exec(
		"INSERT INTO eab_creds (kid, hmac, allow, created) VALUES (?, ?, ?, ?)",
		kid, base64.RawURLEncoding.EncodeToString(hmac), allow,
		time.Now().Unix())
	return err
}

func (d *DB) GetEAB(kid string) (*EABCred, error) {
	var enc, allow string
	err := d.sql.QueryRow(
		"SELECT hmac, allow FROM eab_creds WHERE kid = ?", kid).
		Scan(&enc, &allow)
	if err == sql.ErrNoRows {
		return nil, nil
	}
	if err != nil {
		return nil, err
	}
	raw, err := base64.RawURLEncoding.DecodeString(enc)
	if err != nil {
		return nil, err
	}
	return &EABCred{KID: kid, HMAC: raw, Allow: splitAllow(allow)}, nil
}

// DeleteEAB removes a credential; accounts registered with it keep
// existing but can no longer order (allowed() denies on a missing
// credential). Returns false when no such kid exists.
func (d *DB) DeleteEAB(kid string) (bool, error) {
	res, err := d.sql.Exec("DELETE FROM eab_creds WHERE kid = ?", kid)
	if err != nil {
		return false, err
	}
	n, err := res.RowsAffected()
	return n > 0, err
}

// AccountsByEAB lists the ids of accounts registered with a credential.
func (d *DB) AccountsByEAB(kid string) ([]string, error) {
	rows, err := d.sql.Query(
		"SELECT id FROM accounts WHERE eab_kid = ? ORDER BY created", kid)
	if err != nil {
		return nil, err
	}
	defer rows.Close()
	var out []string
	for rows.Next() {
		var id string
		if err := rows.Scan(&id); err != nil {
			return nil, err
		}
		out = append(out, id)
	}
	return out, rows.Err()
}

func (d *DB) ListEAB() ([][2]string, error) { // kid, allow
	rows, err := d.sql.Query("SELECT kid, allow FROM eab_creds ORDER BY created")
	if err != nil {
		return nil, err
	}
	defer rows.Close()
	var out [][2]string
	for rows.Next() {
		var kid, allow string
		if err := rows.Scan(&kid, &allow); err != nil {
			return nil, err
		}
		out = append(out, [2]string{kid, allow})
	}
	return out, rows.Err()
}

// --- accounts ---

type Account struct {
	ID         string
	Thumbprint string // base64url SHA-256 of the account JWK
	JWK        string // JSON
	EABKid     string
	Status     string
}

func (d *DB) InsertAccount(a *Account) error {
	_, err := d.sql.Exec(
		"INSERT INTO accounts (id, thumbprint, jwk, eab_kid, status, created) "+
			"VALUES (?, ?, ?, ?, ?, ?)",
		a.ID, a.Thumbprint, a.JWK, a.EABKid, a.Status, time.Now().Unix())
	return err
}

func (d *DB) accountRow(where, arg string) (*Account, error) {
	a := &Account{}
	err := d.sql.QueryRow(
		"SELECT id, thumbprint, jwk, eab_kid, status FROM accounts WHERE "+
			where+" = ?", arg).
		Scan(&a.ID, &a.Thumbprint, &a.JWK, &a.EABKid, &a.Status)
	if err == sql.ErrNoRows {
		return nil, nil
	}
	return a, err
}

func (d *DB) AccountByID(id string) (*Account, error) {
	return d.accountRow("id", id)
}
func (d *DB) AccountByThumbprint(tp string) (*Account, error) {
	return d.accountRow("thumbprint", tp)
}

func (d *DB) SetAccountStatus(id, status string) error {
	_, err := d.sql.Exec("UPDATE accounts SET status = ? WHERE id = ?",
		status, id)
	return err
}

// SetAccountKey performs the keyChange rollover: the account is now
// controlled by (and only by) the new key.
func (d *DB) SetAccountKey(id, jwk, thumbprint string) error {
	_, err := d.sql.Exec(
		"UPDATE accounts SET jwk = ?, thumbprint = ? WHERE id = ?",
		jwk, thumbprint, id)
	return err
}

// --- orders / authzs / challenges ---

type Order struct {
	ID          string
	AccountID   string
	Status      string // pending -> ready -> processing -> valid | invalid
	Identifiers []string
	Expires     time.Time
	CertID      string
	Error       string
}

func (d *DB) InsertOrder(o *Order) error {
	ids, _ := json.Marshal(o.Identifiers)
	_, err := d.sql.Exec(
		"INSERT INTO orders (id, account_id, status, identifiers, expires) "+
			"VALUES (?, ?, ?, ?, ?)",
		o.ID, o.AccountID, o.Status, string(ids), o.Expires.Unix())
	return err
}

func (d *DB) OrderByID(id string) (*Order, error) {
	o := &Order{}
	var ids string
	var exp int64
	err := d.sql.QueryRow(
		"SELECT id, account_id, status, identifiers, expires, cert_id, error "+
			"FROM orders WHERE id = ?", id).
		Scan(&o.ID, &o.AccountID, &o.Status, &ids, &exp, &o.CertID, &o.Error)
	if err == sql.ErrNoRows {
		return nil, nil
	}
	if err != nil {
		return nil, err
	}
	if err := json.Unmarshal([]byte(ids), &o.Identifiers); err != nil {
		return nil, err
	}
	o.Expires = time.Unix(exp, 0)
	return o, nil
}

func (d *DB) SetOrderStatus(id, status, certID, problem string) error {
	_, err := d.sql.Exec(
		"UPDATE orders SET status = ?, cert_id = ?, error = ? WHERE id = ?",
		status, certID, problem, id)
	return err
}

type Authz struct {
	ID         string
	OrderID    string
	AccountID  string
	Identifier string // base name; a wildcard authz stores it without "*."
	Status     string // pending -> valid | invalid
	Expires    time.Time
	Wildcard   bool
}

func (d *DB) InsertAuthz(a *Authz) error {
	_, err := d.sql.Exec(
		"INSERT INTO authzs (id, order_id, account_id, identifier, status, "+
			"expires, wildcard) VALUES (?, ?, ?, ?, ?, ?, ?)",
		a.ID, a.OrderID, a.AccountID, a.Identifier, a.Status, a.Expires.Unix(),
		a.Wildcard)
	return err
}

func (d *DB) AuthzByID(id string) (*Authz, error) {
	a := &Authz{}
	var exp int64
	err := d.sql.QueryRow(
		"SELECT id, order_id, account_id, identifier, status, expires, "+
			"wildcard FROM authzs WHERE id = ?", id).
		Scan(&a.ID, &a.OrderID, &a.AccountID, &a.Identifier, &a.Status, &exp,
			&a.Wildcard)
	if err == sql.ErrNoRows {
		return nil, nil
	}
	a.Expires = time.Unix(exp, 0)
	return a, err
}

func (d *DB) AuthzsByOrder(orderID string) ([]*Authz, error) {
	rows, err := d.sql.Query(
		"SELECT id, order_id, account_id, identifier, status, expires, "+
			"wildcard FROM authzs WHERE order_id = ? ORDER BY id", orderID)
	if err != nil {
		return nil, err
	}
	defer rows.Close()
	var out []*Authz
	for rows.Next() {
		a := &Authz{}
		var exp int64
		if err := rows.Scan(&a.ID, &a.OrderID, &a.AccountID, &a.Identifier,
			&a.Status, &exp, &a.Wildcard); err != nil {
			return nil, err
		}
		a.Expires = time.Unix(exp, 0)
		out = append(out, a)
	}
	return out, rows.Err()
}

func (d *DB) SetAuthzStatus(id, status string) error {
	_, err := d.sql.Exec("UPDATE authzs SET status = ? WHERE id = ?", status, id)
	return err
}

type Challenge struct {
	ID        string
	AuthzID   string
	Type      string // http-01
	Token     string
	Status    string // pending -> processing -> valid | invalid
	Validated time.Time
	Error     string
}

func (d *DB) InsertChallenge(c *Challenge) error {
	_, err := d.sql.Exec(
		"INSERT INTO challenges (id, authz_id, type, token, status) "+
			"VALUES (?, ?, ?, ?, ?)",
		c.ID, c.AuthzID, c.Type, c.Token, c.Status)
	return err
}

func (d *DB) ChallengeByID(id string) (*Challenge, error) {
	c := &Challenge{}
	var val int64
	err := d.sql.QueryRow(
		"SELECT id, authz_id, type, token, status, validated, error "+
			"FROM challenges WHERE id = ?", id).
		Scan(&c.ID, &c.AuthzID, &c.Type, &c.Token, &c.Status, &val, &c.Error)
	if err == sql.ErrNoRows {
		return nil, nil
	}
	c.Validated = time.Unix(val, 0)
	return c, err
}

func (d *DB) ChallengesByAuthz(authzID string) ([]*Challenge, error) {
	rows, err := d.sql.Query(
		"SELECT id, authz_id, type, token, status, validated, error "+
			"FROM challenges WHERE authz_id = ?", authzID)
	if err != nil {
		return nil, err
	}
	defer rows.Close()
	var out []*Challenge
	for rows.Next() {
		c := &Challenge{}
		var val int64
		if err := rows.Scan(&c.ID, &c.AuthzID, &c.Type, &c.Token, &c.Status,
			&val, &c.Error); err != nil {
			return nil, err
		}
		c.Validated = time.Unix(val, 0)
		out = append(out, c)
	}
	return out, rows.Err()
}

func (d *DB) SetChallengeStatus(id, status, problem string) error {
	_, err := d.sql.Exec(
		"UPDATE challenges SET status = ?, error = ?, validated = ? WHERE id = ?",
		status, problem, time.Now().Unix(), id)
	return err
}

// GC deletes protocol objects whose expiry passed more than `grace` ago:
// orders and authzs (both expire 24 h after creation, RFC-visible in the
// resource), plus the challenges left without a parent authz. Issued
// certificates are NOT touched - they feed the certificate URL and ARI.
// Returns the total number of rows removed.
func (d *DB) GC(grace time.Duration) (int64, error) {
	cutoff := time.Now().Add(-grace).Unix()
	var total int64
	for _, q := range []string{
		"DELETE FROM orders WHERE expires < " + fmt.Sprint(cutoff),
		"DELETE FROM authzs WHERE expires < " + fmt.Sprint(cutoff),
		"DELETE FROM challenges WHERE authz_id NOT IN (SELECT id FROM authzs)",
	} {
		res, err := d.sql.Exec(q)
		if err != nil {
			return total, err
		}
		n, err := res.RowsAffected()
		if err != nil {
			return total, err
		}
		total += n
	}
	return total, nil
}

// --- certificates ---

type Cert struct {
	ID        string
	OrderID   string
	AccountID string
	CN        string
	ChainPEM  string
	Serial    string // uppercase minimal hex (the store contract's format)
	ARIID     string // RFC 9773 CertID: b64url(AKI)."."b64url(DER serial)
	NotBefore time.Time
	NotAfter  time.Time
}

func (d *DB) InsertCert(c *Cert) error {
	_, err := d.sql.Exec(
		"INSERT INTO certs (id, order_id, account_id, cn, chain_pem, issued, "+
			"serial, ari_id, nb, na) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)",
		c.ID, c.OrderID, c.AccountID, c.CN, c.ChainPEM, time.Now().Unix(),
		c.Serial, c.ARIID, c.NotBefore.Unix(), c.NotAfter.Unix())
	return err
}

func (d *DB) certRow(where string, args ...any) (*Cert, error) {
	c := &Cert{}
	var nb, na int64
	err := d.sql.QueryRow(
		"SELECT id, order_id, account_id, cn, chain_pem, serial, ari_id, "+
			"nb, na FROM certs WHERE "+where, args...).
		Scan(&c.ID, &c.OrderID, &c.AccountID, &c.CN, &c.ChainPEM, &c.Serial,
			&c.ARIID, &nb, &na)
	if err == sql.ErrNoRows {
		return nil, nil
	}
	c.NotBefore, c.NotAfter = time.Unix(nb, 0), time.Unix(na, 0)
	return c, err
}

func (d *DB) CertByID(id string) (*Cert, error) {
	return d.certRow("id = ?", id)
}

func (d *DB) CertByAccountSerial(accountID, serial string) (*Cert, error) {
	return d.certRow("account_id = ? AND serial = ?", accountID, serial)
}

func (d *DB) CertByARI(ariID string) (*Cert, error) {
	return d.certRow("ari_id = ?", ariID)
}

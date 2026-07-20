#!/usr/bin/env bash
# End-to-end tests for the yca CA CLI.
# Usage: e2e.sh <path-to-yca-binary>
# shellcheck disable=SC2015  # `cmd && ok || bad` is the assert idiom; ok/bad never fail
set -u

BIN="${1:?usage: e2e.sh <yca-binary>}"
# The suite exercises the passphrase-generation path on init; an inherited
# passphrase from the caller's environment would skip it and cascade-fail.
unset CA_STORE_PASSPHRASE
WORK="$(mktemp -d)"
trap 'rm -rf "$WORK"' EXIT
CFG="$WORK/yca.toml"
PKI="$WORK/pki"
# Diagnostics go to the log file (yca.log next to the store dir); errors
# are mirrored to stderr. Log assertions grep $WORK/yca.log.
cd "$WORK" || exit 1
LOG="$WORK/yca.log"

cat >"$CFG" <<'EOF'
org_name = "Example"
country_code = "CA"
repository_host = "pki.example.ca"
root_ca_cn = "ETS Root E1"
root_ca_curve = "secp384r1"
root_ca_digest = "SHA-384"
root_ca_valid_days = 8192
root_ca_slug_prefix = "ets-root-e"
signing_ca_cn = "CA E1"
signing_ca_curve = "secp384r1"
signing_ca_digest = "SHA-384"
signing_ca_valid_days = 8112
signing_ca_slug_prefix = "ca-e"
ee_curve = "secp256r1"
ee_digest = "SHA-256"
ee_valid_days = 397
root_arc_oid = "1.3.6.1.4.1.00000"
EOF

PASS=0
FAIL=0
ok() {
  PASS=$((PASS + 1))
  echo "  ok  : $1"
}
bad() {
  FAIL=$((FAIL + 1))
  echo "  FAIL: $1"
}
w() { "$BIN" --config "$CFG" --store "$PKI" "$@"; }
# openssl date ("Jul  2 16:42:14 2026 GMT") -> epoch seconds; GNU date takes
# -d, BSD (macOS) needs -j -f <format>.
epoch() {
  date -u -d "$1" +%s 2>/dev/null ||
    date -j -u -f "%b %e %H:%M:%S %Y %Z" "$1" +%s
}

# --- before init ---
w >/dev/null 2>&1 && ok "bare yca shows help (exit 0)" || bad "bare yca failed"
w create server --cn x.ca >/dev/null 2>&1 &&
  bad "command works before init" || ok "command before init rejected"
ERR="$(w get config 2>&1 >/dev/null)"
printf '%s' "$ERR" | grep -q "not initialized" &&
  ok "before-init error shown on stderr" || bad "before-init error silent"
w --version | grep -Eq '^yca version [0-9]+\.[0-9]+\.[0-9]+$' &&
  ok "--version prints semver" || bad "--version output"
[ ! -e "$PKI" ] && ok "read cmd does not create the store dir" ||
  bad "store dir conjured by read cmd"

# --- init ---
OUT="$(w init 2>&1)" && ok "init exits 0" || bad "init exit code"
PHRASE="$(printf '%s\n' "$OUT" | grep -A1 GENERATED | tail -1)"
[ -n "$PHRASE" ] && ok "passphrase generated once" || bad "no passphrase shown"
export CA_STORE_PASSPHRASE="$PHRASE"
[ -f "$PKI/ca-store.db" ] && ok "store ca-store.db created" || bad "store missing"
for f in ets-root-e1.pem ets-root-e1.crt ets-root-e1.crl \
  ca-e1.pem ca-e1.crt ca-e1.crl; do
  [ -f "$PKI/ca/$f" ] && ok "ca/$f written" || bad "ca/$f missing"
done

# --- permissions: 700 dirs, 400 artifacts, 600 store ---
# shellcheck disable=SC2012  # ls is portable here (GNU/BSD); names are controlled slugs
perm() { ls -ld "$1" | cut -c1-10; }
[ "$(perm "$PKI")" = "drwx------" ] && ok "store dir is 700" || bad "store dir perms: $(perm "$PKI")"
[ "$(perm "$PKI/ca")" = "drwx------" ] && ok "ca dir is 700" || bad "ca dir perms: $(perm "$PKI/ca")"
[ "$(perm "$PKI/ca-store.db")" = "-rw-------" ] && ok "store is 600" || bad "store perms: $(perm "$PKI/ca-store.db")"
[ "$(perm "$PKI/ca/ca-e1.pem")" = "-r--------" ] && ok "ca pem is 400" || bad "ca pem perms: $(perm "$PKI/ca/ca-e1.pem")"
[ "$(perm "$PKI/ca/ca-e1.crl")" = "-r--------" ] && ok "crl is 400" || bad "crl perms: $(perm "$PKI/ca/ca-e1.crl")"

# --- re-init + passphrase ---
w init >/dev/null 2>&1 && bad "re-init accepted" || ok "re-init rejected (already initialized)"
(
  export CA_STORE_PASSPHRASE=wrong
  w create server --cn wp.ca >/dev/null 2>&1
) &&
  bad "wrong passphrase accepted" || ok "wrong passphrase rejected"

# --- create server + chain ---
w create server --cn server.ca >/dev/null 2>&1 && ok "create server" || bad "create server"
{ [ -f "$PKI/ee/server.ca.crt" ] && [ -f "$PKI/ee/server.ca.key" ]; } &&
  ok "ee cert+key delivered" || bad "ee artifacts missing"
[ "$(perm "$PKI/ee")" = "drwx------" ] && ok "ee dir is 700" || bad "ee dir perms: $(perm "$PKI/ee")"
[ "$(perm "$PKI/ee/server.ca.key")" = "-r--------" ] && ok "ee key is 400" || bad "ee key perms: $(perm "$PKI/ee/server.ca.key")"
[ "$(perm "$PKI/ee/server.ca.crt")" = "-r--------" ] && ok "ee crt is 400" || bad "ee crt perms: $(perm "$PKI/ee/server.ca.crt")"
openssl verify -CAfile "$PKI/ca/ets-root-e1.pem" \
  -untrusted "$PKI/ca/ca-e1.pem" "$PKI/ee/server.ca.crt" >/dev/null 2>&1 &&
  ok "EE chain verifies to root" || bad "chain verify failed"
w get server --cn server.ca 2>/dev/null |
  openssl x509 -noout -ext subjectAltName 2>/dev/null | grep -q "DNS:server.ca" &&
  ok "server auto DNS:CN SAN" || bad "auto SAN missing"

# --- client SAN rule ---
w create client --cn "No SAN" >/dev/null 2>&1 &&
  bad "client without --san accepted" || ok "client without --san rejected"
w create client --cn "Client" --san=email:p@example.ca >/dev/null 2>&1 &&
  ok "create client with SAN" || bad "create client failed"

# --- uri SAN (SPIFFE): both profiles, one per certificate ---
w create client --cn workload --san=uri:spiffe://example.ca/ns/prod/sa/web \
  >/dev/null 2>&1 && ok "create client with uri SAN only" || bad "client uri SAN"
w get client --cn workload 2>/dev/null |
  openssl x509 -noout -ext subjectAltName 2>/dev/null |
  grep -q "URI:spiffe://example.ca/ns/prod/sa/web" &&
  ok "uri SAN present in the certificate" || bad "uri SAN missing"
w create server --cn svc.example.ca --san=uri:spiffe://example.ca/svc \
  >/dev/null 2>&1 && ok "create server with uri SAN" || bad "server uri SAN"
w create client --cn badsid --san=uri:spiffe://EXAMPLE.ca/wl >/dev/null 2>&1 &&
  bad "uppercase SPIFFE trust domain accepted" ||
  ok "uppercase SPIFFE trust domain rejected"
w create client --cn badsid2 --san=uri:spiffe://example.ca/wl/ >/dev/null 2>&1 &&
  bad "SPIFFE trailing slash accepted" || ok "SPIFFE trailing slash rejected"
w create client --cn badsid3 --san=uri:example.ca/wl >/dev/null 2>&1 &&
  bad "relative uri SAN accepted" || ok "relative uri SAN rejected"
w create client --cn twouris --san=uri:spiffe://example.ca/a \
  --san=uri:spiffe://example.ca/b >/dev/null 2>&1 &&
  bad "two uri SANs accepted" || ok "two uri SANs rejected (one per SVID)"

# --- uniqueness (cn, profile) ---
w create server --cn foo.ca >/dev/null 2>&1 && ok "server foo.ca" || bad "server foo.ca"
w create client --cn foo.ca --san=dns:foo.ca >/dev/null 2>&1 &&
  ok "client foo.ca (same CN, other profile)" || bad "client foo.ca"
w create server --cn foo.ca >/dev/null 2>&1 &&
  bad "duplicate active server foo.ca accepted" ||
  ok "duplicate active server foo.ca rejected"

# --- get disambiguation ---
w get server --cn foo.ca 2>/dev/null |
  openssl x509 -noout -ext extendedKeyUsage 2>/dev/null | grep -q "Server" &&
  ok "get server -> serverAuth" || bad "get server EKU"
w get client --cn foo.ca 2>/dev/null |
  openssl x509 -noout -ext extendedKeyUsage 2>/dev/null | grep -q "Client" &&
  ok "get client -> clientAuth" || bad "get client EKU"
w get ca --cn root-ca --encoding der 2>/dev/null |
  openssl x509 -inform DER -noout -subject 2>/dev/null | grep -q "ETS Root E1" &&
  ok "get ca der (root-ca alias)" || bad "get ca der"
(
  unset CA_STORE_PASSPHRASE
  w get ca --cn signing-ca >/dev/null 2>&1
) &&
  ok "get needs no passphrase" || bad "get required passphrase"

# --- revoke ---
w revoke server --cn foo.ca --reason keyCompromise >/dev/null 2>&1 &&
  ok "revoke server foo.ca" || bad "revoke server foo.ca"
w get server --cn foo.ca >/dev/null 2>&1 &&
  bad "revoked server still active" || ok "revoked server no longer active"
w get client --cn foo.ca >/dev/null 2>&1 &&
  ok "client foo.ca still active after server revoke" || bad "client foo.ca lost"
w create server --cn foo.ca >/dev/null 2>&1 &&
  ok "re-issue server foo.ca after revoke" || bad "re-issue blocked"
openssl crl -inform DER -in "$PKI/ca/ca-e1.crl" -CAfile "$PKI/ca/ca-e1.pem" -noout >/dev/null 2>&1 &&
  ok "signing CRL is signed/valid" || bad "CRL verify failed"
[ "$(openssl crl -inform DER -in "$PKI/ca/ca-e1.crl" -noout -text 2>/dev/null |
  grep -c 'Serial Number')" -ge 1 ] &&
  ok "CRL lists a revoked serial" || bad "CRL has no entries"

# --- crl exports + CRL-only pointer extensions ---
w get crl --cn signing-ca 2>/dev/null | openssl crl -noout -issuer 2>/dev/null |
  grep -q "CA E1" && ok "get crl signing-ca" || bad "get crl signing-ca"
w get crl --cn root-ca 2>/dev/null | openssl crl -noout -issuer 2>/dev/null |
  grep -q "ETS Root E1" && ok "get crl root-ca" || bad "get crl root-ca"
# Revocation is CRL-only: no certificate advertises an OCSP URI; AIA keeps
# caIssuers and the CDP points at the issuer's CRL.
SIGN_TEXT="$(w get ca --cn signing-ca 2>/dev/null | openssl x509 -noout -text 2>/dev/null)"
printf '%s' "$SIGN_TEXT" | grep -q "OCSP - URI" &&
  bad "signing CA advertises OCSP" || ok "signing CA carries no OCSP pointer"
printf '%s' "$SIGN_TEXT" | grep -q "CA Issuers - URI:.*ets-root-e1.crt" &&
  ok "signing CA caIssuers points at the root" || bad "signing CA caIssuers"
printf '%s' "$SIGN_TEXT" | grep -q "ets-root-e1.crl" &&
  ok "signing CA CDP points at the root CRL" || bad "signing CA CDP"
EE_TEXT="$(w get server --cn foo.ca 2>/dev/null | openssl x509 -noout -text 2>/dev/null)"
printf '%s' "$EE_TEXT" | grep -q "OCSP - URI" &&
  bad "EE advertises OCSP" || ok "EE carries no OCSP pointer"
printf '%s' "$EE_TEXT" | grep -q "CA Issuers - URI:.*ca-e1.crt" &&
  ok "EE caIssuers points at the signing CA" || bad "EE caIssuers"
printf '%s' "$EE_TEXT" | grep -q "ca-e1.crl" &&
  ok "EE CDP points at the signing CRL" || bad "EE CDP"

# --- refresh crl: same entries, crlNumber+1, fresh nextUpdate ---
# crlnum emits "0x01"-style hex; bash arithmetic evaluates it natively.
crlnum() { openssl crl -inform DER -in "$1" -noout -crlnumber 2>/dev/null | cut -d= -f2; }
SN_BEFORE="$(crlnum "$PKI/ca/ca-e1.crl")"
RN_BEFORE="$(crlnum "$PKI/ca/ets-root-e1.crl")"
NU_BEFORE="$(openssl crl -inform DER -in "$PKI/ca/ca-e1.crl" -noout -nextupdate | cut -d= -f2)"
w refresh crl >/dev/null 2>&1 && ok "refresh crl" || bad "refresh crl"
[ "$(($(crlnum "$PKI/ca/ca-e1.crl")))" -eq "$((SN_BEFORE + 1))" ] &&
  ok "signing crlNumber incremented" || bad "signing crlNumber"
[ "$(($(crlnum "$PKI/ca/ets-root-e1.crl")))" -eq "$((RN_BEFORE + 1))" ] &&
  ok "root crlNumber incremented" || bad "root crlNumber"
[ "$(openssl crl -inform DER -in "$PKI/ca/ca-e1.crl" -noout -text 2>/dev/null |
  grep -c 'Serial Number')" -ge 1 ] &&
  ok "revoked entries survive the refresh" || bad "refresh lost CRL entries"
openssl crl -inform DER -in "$PKI/ca/ca-e1.crl" -CAfile "$PKI/ca/ca-e1.pem" -noout >/dev/null 2>&1 &&
  ok "refreshed signing CRL verifies" || bad "refreshed signing CRL invalid"
openssl crl -inform DER -in "$PKI/ca/ets-root-e1.crl" -CAfile "$PKI/ca/ets-root-e1.pem" -noout >/dev/null 2>&1 &&
  ok "refreshed root CRL verifies" || bad "refreshed root CRL invalid"
NU_AFTER="$(openssl crl -inform DER -in "$PKI/ca/ca-e1.crl" -noout -nextupdate | cut -d= -f2)"
[ "$(epoch "$NU_AFTER")" -ge "$(epoch "$NU_BEFORE")" ] &&
  ok "nextUpdate refreshed" || bad "nextUpdate went backwards"
"$BIN" --config "$CFG" --store "$WORK/pki-none" refresh crl >/dev/null 2>&1 &&
  bad "refresh before init accepted" || ok "refresh before init rejected"
(unset CA_STORE_PASSPHRASE && w refresh crl >/dev/null 2>&1) &&
  bad "refresh without secret accepted" || ok "refresh without secret rejected"

# --- refresh crl scopes: signing and root run on separate cadences ---
SN_SCOPED="$(crlnum "$PKI/ca/ca-e1.crl")"
RN_SCOPED="$(crlnum "$PKI/ca/ets-root-e1.crl")"
w refresh crl signing >/dev/null 2>&1 &&
  ok "refresh crl signing" || bad "refresh crl signing"
[ "$(($(crlnum "$PKI/ca/ca-e1.crl")))" -eq "$((SN_SCOPED + 1))" ] &&
  [ "$(($(crlnum "$PKI/ca/ets-root-e1.crl")))" -eq "$((RN_SCOPED))" ] &&
  ok "signing scope leaves the root CRL untouched" ||
  bad "signing scope touched the root CRL"
w refresh crl root >/dev/null 2>&1 &&
  ok "refresh crl root" || bad "refresh crl root"
[ "$(($(crlnum "$PKI/ca/ets-root-e1.crl")))" -eq "$((RN_SCOPED + 1))" ] &&
  [ "$(($(crlnum "$PKI/ca/ca-e1.crl")))" -eq "$((SN_SCOPED + 1))" ] &&
  ok "root scope leaves the signing CRL untouched" ||
  bad "root scope touched the signing CRL"
# 183-day root horizon vs 7-day signing horizon (no clamping: both CAs
# have years of life left in this config).
RNU="$(openssl crl -inform DER -in "$PKI/ca/ets-root-e1.crl" -noout -nextupdate | cut -d= -f2)"
SNU="$(openssl crl -inform DER -in "$PKI/ca/ca-e1.crl" -noout -nextupdate | cut -d= -f2)"
NOW="$(date +%s)"
[ "$(epoch "$RNU")" -gt "$((NOW + 180 * 86400))" ] &&
  ok "root CRL carries the 6-month horizon" || bad "root CRL horizon"
[ "$(epoch "$SNU")" -lt "$((NOW + 8 * 86400))" ] &&
  ok "signing CRL keeps the 7-day horizon" || bad "signing CRL horizon"
w refresh crl bogus >/dev/null 2>&1 &&
  bad "bogus refresh scope accepted" || ok "bogus refresh scope rejected"

# --- config validation ---
sed 's/SHA-384/SHA-999/' "$CFG" >"$WORK/bad.toml"
"$BIN" --config "$WORK/bad.toml" --store "$WORK/pki2" init >/dev/null 2>&1 &&
  bad "invalid config accepted" || ok "invalid config rejected"
# only Botan curve names: the OpenSSL alias prime256v1 is rejected
sed 's/ee_curve = "secp256r1"/ee_curve = "prime256v1"/' "$CFG" >"$WORK/alias.toml"
"$BIN" --config "$WORK/alias.toml" --store "$WORK/pki2" init >/dev/null 2>&1 &&
  bad "prime256v1 alias accepted" || ok "prime256v1 alias rejected"

# --- unicode: names are DN-only (free-form), slugs/URL fields strict ASCII ---
sed -e 's/org_name = "Example"/org_name = "Компания 株"/' \
  -e 's/root_ca_cn = "ETS Root E1"/root_ca_cn = "ETS 株 Root E1"/' \
  "$CFG" >"$WORK/uni.toml"
"$BIN" --config "$WORK/uni.toml" --store "$WORK/pki_uni" init >/dev/null 2>&1 &&
  ok "unicode org+root names accepted (DN UTF8String)" || bad "unicode names rejected"
openssl x509 -in "$WORK/pki_uni/ca/ets-root-e1.pem" -noout -subject -nameopt utf8 2>/dev/null |
  grep -q "Компания" && ok "unicode O= present in DN" || bad "unicode O= missing"
openssl x509 -in "$WORK/pki_uni/ca/ets-root-e1.pem" -noout -subject -nameopt utf8 2>/dev/null |
  grep -q "ETS 株 Root E1" && ok "unicode CN= present, file named by slug" ||
  bad "unicode CN missing"
# get crl resolves the FILE by the configured slug, never by slugifying
# the CN - here they differ (the unicode CN cannot even name a file).
"$BIN" --config "$WORK/uni.toml" --store "$WORK/pki_uni" get crl --cn root-ca 2>/dev/null |
  openssl crl -noout -issuer -nameopt utf8 2>/dev/null |
  grep -q "ETS 株 Root E1" &&
  ok "get crl file named by slug (CN and slug decoupled)" ||
  bad "get crl resolved by slugified CN"
sed 's/root_ca_slug_prefix = "ets-root-e"/root_ca_slug_prefix = "ets-株"/' "$CFG" \
  >"$WORK/uni_slug.toml"
"$BIN" --config "$WORK/uni_slug.toml" --store "$WORK/pki_uni2" init >/dev/null 2>&1 &&
  bad "unicode slug prefix accepted" || ok "unicode slug prefix rejected (feeds URLs)"
w create server --cn "srv★.ca" >/dev/null 2>&1 &&
  bad "unicode server CN accepted" || ok "unicode server CN rejected (DNS SAN)"
w create client --cn "Иван Петров" --san=email:ivan@example.ca >/dev/null 2>&1 &&
  ok "unicode client CN accepted (DN-only)" || bad "unicode client CN rejected"

# --- DB source of truth + reconcile on create ---
w get config 2>/dev/null | grep -q 'repository_host = "pki.example.ca"' &&
  ok "get config reads DB" || bad "get config"
# readonly drift on create -> warned and ignored (cert uses DB repository_host)
sed 's/pki.example.ca/pki.drift.ca/' "$CFG" >"$WORK/ro.toml"
"$BIN" --config "$WORK/ro.toml" --store "$PKI" create server --cn ro.ca \
  >/dev/null 2>&1
grep -q "ignoring readonly" "$LOG" &&
  ok "readonly drift warned (log)" || bad "readonly drift not warned"
w get server --cn ro.ca 2>/dev/null | openssl x509 -noout -text 2>/dev/null |
  grep -q "pki.example.ca/ca-e1.crt" &&
  ok "readonly drift ignored (DB value used)" || bad "readonly drift applied"
# ee_valid_days is locked like everything else: changed in the file ->
# warned, ignored, and never written to the DB
sed 's/ee_valid_days = 397/ee_valid_days = 90/' "$CFG" >"$WORK/tune.toml"
"$BIN" --config "$WORK/tune.toml" --store "$PKI" create server --cn tuned.ca \
  >/dev/null 2>&1
w get config 2>/dev/null | grep -q "ee_valid_days = 397" &&
  ok "ee_valid_days change ignored (locked)" || bad "ee_valid_days synced"

# --- list (filter-based on cert_index) ---
w create server --cn l1.ca >/dev/null 2>&1
w create server --cn l2.ca >/dev/null 2>&1
w revoke server --cn l2.ca --reason superseded >/dev/null 2>&1
w list --last 2>/dev/null | grep -E "l1.ca" | grep -q active &&
  ok "list --last shows a recent active issue" || bad "list --last active"
w list --revoked 2>/dev/null | grep -E "l2.ca" | grep -q revoked &&
  ok "list --revoked shows revoked" || bad "list --revoked"
w list --cn root-ca 2>/dev/null | grep -q "ETS Root E1" &&
  ok "list --cn root-ca resolves the CA" || bad "list --cn root-ca"
w list --last --tsv 2>/dev/null | head -1 | grep -q "$(printf 'CN\tKIND\tSERIAL')" &&
  ok "list --tsv tab-separated" || bad "list tsv"
w list 2>/dev/null && bad "list with no filter accepted" ||
  ok "list requires exactly one filter"
# window cap = max ee_valid_days (398): --expiring can cover a full EE
# lifetime, so the signing CA is visible before issuance starts refusing
w list --expiring 398 >/dev/null 2>&1 &&
  ok "list --expiring 398 accepted" || bad "list --expiring 398"
w list --expiring 399 >/dev/null 2>&1 &&
  bad "list --expiring 399 accepted" || ok "list --expiring 399 rejected"

# --- corrupt CRL fails gracefully (logged error + exit 1, no abort) ---
cp "$PKI/ca/ca-e1.crl" "$WORK/crl.bak"
chmod u+w "$PKI/ca/ca-e1.crl" # artifacts are 0400; make writable to corrupt
echo garbage >"$PKI/ca/ca-e1.crl"
w revoke server --cn l1.ca --reason superseded >/dev/null 2>&1
rc=$?
[ "$rc" -eq 1 ] && ok "corrupt CRL rejected gracefully" || bad "corrupt CRL exit code $rc"
cp "$WORK/crl.bak" "$PKI/ca/ca-e1.crl"
w revoke server --cn l1.ca --reason superseded >/dev/null 2>&1 &&
  ok "revoke works after CRL restore" || bad "revoke after restore"

# --- 1. the locked policy applies to a new cert (the DB value, not the file) ---
sed 's/ee_valid_days = 397/ee_valid_days = 90/' "$CFG" >"$WORK/ee90.toml"
"$BIN" --config "$WORK/ee90.toml" --store "$PKI" create server --cn ee90.ca \
  >/dev/null 2>&1
NB=$(w get server --cn ee90.ca 2>/dev/null |
  openssl x509 -noout -startdate 2>/dev/null | cut -d= -f2)
NA=$(w get server --cn ee90.ca 2>/dev/null |
  openssl x509 -noout -enddate 2>/dev/null | cut -d= -f2)
DAYS=$((($(epoch "$NA") - $(epoch "$NB")) / 86400))
[ "$DAYS" -ge 396 ] && [ "$DAYS" -le 398 ] &&
  ok "locked policy applied to new cert ($DAYS days)" ||
  bad "ee validity not from DB ($DAYS)"

# --- 2. get config round-trips as valid TOML ---
w get config 2>/dev/null >"$WORK/dump1.toml"
head -1 "$WORK/dump1.toml" | grep -q "^# showing configuration stored in database" &&
  ok "get config shows provenance" || bad "provenance header missing"
(env -u CA_STORE_PASSPHRASE "$BIN" --config "$WORK/dump1.toml" \
  --store "$WORK/pki_rt" init >/dev/null 2>&1)
"$BIN" --config "$WORK/dump1.toml" --store "$WORK/pki_rt" get config \
  2>/dev/null >"$WORK/dump2.toml"
diff -q "$WORK/dump1.toml" "$WORK/dump2.toml" >/dev/null &&
  ok "get config round-trips" || bad "get config round-trip mismatch"

# --- 3. no field reconciles into the DB, on any command ---
sed 's/^ee_valid_days = .*/ee_valid_days = 120/' "$CFG" >"$WORK/e120.toml"
"$BIN" --config "$WORK/e120.toml" --store "$PKI" create server --cn lock120.ca \
  >/dev/null 2>&1
w get config 2>/dev/null | grep -q "ee_valid_days = 397" &&
  ok "config fully locked (create did not sync)" || bad "create synced a field"

# --- --valid override: [5m, ee_valid_days], the policy is the ceiling ---
# ee_valid_days is locked at init to $CFG's 397, so the ceiling here is
# 397 days.
w create server --cn v1h.ca --valid 1h >/dev/null 2>&1 &&
  ok "create --valid 1h" || bad "create --valid 1h"
NB=$(w get server --cn v1h.ca 2>/dev/null |
  openssl x509 -noout -startdate 2>/dev/null | cut -d= -f2)
NA=$(w get server --cn v1h.ca 2>/dev/null |
  openssl x509 -noout -enddate 2>/dev/null | cut -d= -f2)
SPAN=$(($(epoch "$NA") - $(epoch "$NB")))
[ "$SPAN" -eq 3600 ] && ok "--valid 1h applied (3600s)" || bad "--valid span ${SPAN}s"
w create server --cn v2d.ca --valid 2d >/dev/null 2>&1 &&
  ok "create --valid 2d (day-scale)" || bad "create --valid 2d"
DNB=$(w get server --cn v2d.ca 2>/dev/null |
  openssl x509 -noout -startdate 2>/dev/null | cut -d= -f2)
DNA=$(w get server --cn v2d.ca 2>/dev/null |
  openssl x509 -noout -enddate 2>/dev/null | cut -d= -f2)
[ "$(($(epoch "$DNA") - $(epoch "$DNB")))" -eq 172800 ] &&
  ok "--valid 2d applied (172800s)" || bad "--valid 2d span"
w create server --cn vlong.ca --valid 398d >/dev/null 2>&1 &&
  bad "--valid above ee_valid_days accepted" ||
  ok "--valid above ee_valid_days rejected"
w create server --cn vfast.ca --valid 4m >/dev/null 2>&1 &&
  bad "--valid < 5m accepted" || ok "--valid < 5m rejected"
w create server --cn vbad.ca --valid 90x >/dev/null 2>&1 &&
  bad "malformed --valid accepted" || ok "malformed --valid rejected"
w create ocsp --cn x >/dev/null 2>&1 &&
  bad "create ocsp accepted" || ok "create ocsp rejected (target removed)"

# --- 4. create ca / revoke ca rejected ---
w create ca --cn x >/dev/null 2>&1 && bad "create ca accepted" || ok "create ca rejected"
w revoke ca --cn x >/dev/null 2>&1 && bad "revoke ca accepted" || ok "revoke ca rejected"

# --- 5. invalid --san / --reason ---
w create server --cn bad1.ca --san=notype >/dev/null 2>&1 &&
  bad "--san without type accepted" || ok "--san without type rejected"
w create server --cn bad2.ca --san=foo:bar >/dev/null 2>&1 &&
  bad "unknown --san type accepted" || ok "unknown --san type rejected"
w revoke server --cn whatever.ca --reason bogus >/dev/null 2>&1 &&
  bad "invalid --reason accepted" || ok "invalid --reason rejected"

# --- CSR signing: enroll / get nonce / sign ---
w enroll --id user@example.ca >/dev/null 2>&1 && ok "enroll id" || bad "enroll id"
w enroll --id user@example.ca >/dev/null 2>&1 &&
  bad "re-enroll accepted" || ok "re-enroll rejected"
NONCE="$(w get nonce --id user@example.ca 2>/dev/null)"
printf '%s' "$NONCE" | grep -Eq '^[0-9a-f]{64}$' &&
  ok "nonce is 64 lowercase hex chars" || bad "nonce format: '$NONCE'"
[ "$(w get nonce --id user@example.ca 2>/dev/null)" = "$NONCE" ] &&
  ok "get nonce idempotent within validity" || bad "nonce rotated too early"
(
  unset CA_STORE_PASSPHRASE
  w get nonce --id user@example.ca >/dev/null 2>&1
) &&
  ok "get nonce needs no passphrase" || bad "get nonce required passphrase"
w get nonce --id ghost@example.ca >/dev/null 2>&1 &&
  bad "nonce for unenrolled id" || ok "unenrolled id rejected"

# Hostile CSR: the CA must take only pubkey/CN/supported SANs and dictate the
# rest (openssl-generated: real cross-implementation interop).
openssl ecparam -genkey -name prime256v1 -noout -out "$WORK/ee.key" 2>/dev/null
openssl req -new -key "$WORK/ee.key" -out "$WORK/evil.csr" \
  -subj "/C=US/O=Evil Corp/OU=Ops/CN=csr.example.ca" \
  -addext "basicConstraints=critical,CA:TRUE" \
  -addext "keyUsage=critical,keyCertSign,cRLSign" \
  -addext "extendedKeyUsage=codeSigning" \
  -addext "subjectAltName=DNS:alt.example.ca,URI:https://evil.example" \
  2>/dev/null
CN="$(w sign server --id user@example.ca --nonce "$NONCE" --csr "$WORK/evil.csr" 2>/dev/null)"
[ "$CN" = "csr.example.ca" ] && ok "sign prints the CN on stdout" || bad "sign stdout: '$CN'"
w get server --cn csr.example.ca 2>/dev/null >"$WORK/csr-signed.pem" ||
  bad "get for CSR-signed cert"
openssl x509 -in "$WORK/csr-signed.pem" -noout -subject 2>/dev/null |
  grep -q "^subject=CN" && ok "subject DN filtered to CN only" || bad "extra DN attrs kept"
openssl x509 -in "$WORK/csr-signed.pem" -noout -ext basicConstraints 2>/dev/null |
  grep -q "CA:FALSE" && ok "CSR CA:TRUE overridden to CA:FALSE" || bad "basicConstraints honored from CSR"
openssl x509 -in "$WORK/csr-signed.pem" -noout -ext keyUsage 2>/dev/null |
  grep -q "Certificate Sign" && bad "keyCertSign honored from CSR" || ok "keyUsage dictated (digitalSignature)"
openssl x509 -in "$WORK/csr-signed.pem" -noout -ext extendedKeyUsage 2>/dev/null |
  grep -q "Code Signing" && bad "codeSigning EKU honored from CSR" || ok "EKU dictated by profile"
SAN="$(openssl x509 -in "$WORK/csr-signed.pem" -noout -ext subjectAltName 2>/dev/null)"
printf '%s' "$SAN" | grep -q "DNS:csr.example.ca" &&
  ok "DNS:CN forced into SAN" || bad "DNS:CN missing from SAN"
printf '%s' "$SAN" | grep -q "DNS:alt.example.ca" &&
  ok "supported CSR SAN kept" || bad "dns SAN dropped"
# URI SANs are honored now (SPIFFE): the CA still dictates everything
# else, but a well-formed URI from the CSR survives.
printf '%s' "$SAN" | grep -q "URI:https://evil.example" &&
  ok "URI SAN kept from CSR" || bad "URI SAN dropped"
openssl verify -CAfile "$PKI/ca/ets-root-e1.pem" \
  -untrusted "$PKI/ca/ca-e1.pem" "$WORK/csr-signed.pem" >/dev/null 2>&1 &&
  ok "CSR-signed chain verifies to root" || bad "CSR-signed chain verify failed"
[ ! -e "$PKI/ee/csr.example.ca.crt" ] && [ ! -e "$PKI/ee/csr.example.ca.key" ] &&
  ok "sign leaves no ee/ artifacts (no private key at the CA)" || bad "sign wrote under ee/"

# nonce lifecycle: consumed on success, rotated on demand, single-use
w sign server --id user@example.ca --nonce "$NONCE" --csr "$WORK/evil.csr" >/dev/null 2>&1 &&
  bad "consumed nonce accepted" || ok "consumed nonce rejected"
NONCE2="$(w get nonce --id user@example.ca 2>/dev/null)"
[ "$NONCE2" != "$NONCE" ] && ok "nonce rotates after consumption" || bad "consumed nonce returned again"
w sign server --id user@example.ca --nonce "$NONCE" --csr "$WORK/evil.csr" >/dev/null 2>&1 &&
  bad "stale nonce accepted" || ok "stale nonce rejected"

# input forms: single-line inline PEM; DER via stdin; piped delivery
openssl req -new -key "$WORK/ee.key" -out "$WORK/cl.csr" \
  -subj "/CN=Client Name" -addext "subjectAltName=email:client@example.ca" 2>/dev/null
w sign client --id user@example.ca --nonce "$NONCE2" --csr "$(tr '\n' ' ' <"$WORK/cl.csr")" \
  >/dev/null 2>&1 && ok "inline single-line PEM CSR" || bad "inline PEM CSR"
openssl req -new -key "$WORK/ee.key" -outform DER -subj "/CN=der.example.ca" 2>/dev/null \
  >"$WORK/der.csr"
NONCE3="$(w get nonce --id user@example.ca 2>/dev/null)"
w sign server --id user@example.ca --nonce "$NONCE3" --csr - <"$WORK/der.csr" |
  w get server --cn - 2>/dev/null | openssl x509 -noout -subject 2>/dev/null |
  grep -q "der.example.ca" && ok "DER via stdin + piped get --cn -" || bad "stdin/pipe flow"

# --valid on the CSR path: same [5m, ee_valid_days] policy as create
openssl req -new -key "$WORK/ee.key" -out "$WORK/sv.csr" \
  -subj "/CN=sv.example.ca" 2>/dev/null
NONCE_SV="$(w get nonce --id user@example.ca 2>/dev/null)"
w sign server --id user@example.ca --nonce "$NONCE_SV" \
  --csr "$WORK/sv.csr" --valid 10m >/dev/null 2>&1 &&
  ok "sign --valid 10m" || bad "sign --valid 10m"
SNB=$(w get server --cn sv.example.ca 2>/dev/null |
  openssl x509 -noout -startdate 2>/dev/null | cut -d= -f2)
SNA=$(w get server --cn sv.example.ca 2>/dev/null |
  openssl x509 -noout -enddate 2>/dev/null | cut -d= -f2)
[ "$(($(epoch "$SNA") - $(epoch "$SNB")))" -eq 600 ] &&
  ok "sign --valid 10m applied (600s)" || bad "sign --valid span"
NONCE_SV2="$(w get nonce --id user@example.ca 2>/dev/null)"
w sign server --id user@example.ca --nonce "$NONCE_SV2" \
  --csr "$WORK/sv.csr" --valid 398d >/dev/null 2>&1 &&
  bad "sign --valid above ee_valid_days accepted" ||
  ok "sign --valid above ee_valid_days rejected"

# uri SANs on the CSR path: honored, validated and capped at one
openssl req -new -key "$WORK/ee.key" -out "$WORK/uri.csr" -subj "/CN=wl.example.ca" \
  -addext "subjectAltName=URI:spiffe://example.ca/wl" 2>/dev/null
NONCE_U="$(w get nonce --id user@example.ca 2>/dev/null)"
w sign client --id user@example.ca --nonce "$NONCE_U" --csr "$WORK/uri.csr" \
  >/dev/null 2>&1 && ok "sign CSR with uri SAN" || bad "sign CSR uri SAN"
w get client --cn wl.example.ca 2>/dev/null |
  openssl x509 -noout -ext subjectAltName 2>/dev/null |
  grep -q "URI:spiffe://example.ca/wl" &&
  ok "CSR uri SAN honored in the certificate" || bad "CSR uri SAN dropped"
openssl req -new -key "$WORK/ee.key" -out "$WORK/2uri.csr" -subj "/CN=two.example.ca" \
  -addext "subjectAltName=URI:spiffe://example.ca/a,URI:spiffe://example.ca/b" 2>/dev/null
NONCE_U2="$(w get nonce --id user@example.ca 2>/dev/null)"
w sign client --id user@example.ca --nonce "$NONCE_U2" --csr "$WORK/2uri.csr" \
  >/dev/null 2>&1 &&
  bad "CSR with two uri SANs accepted" || ok "CSR with two uri SANs rejected"
openssl req -new -key "$WORK/ee.key" -out "$WORK/badsid.csr" -subj "/CN=bs.example.ca" \
  -addext "subjectAltName=URI:spiffe://example.ca/wl/" 2>/dev/null
w sign client --id user@example.ca --nonce "$NONCE_U2" --csr "$WORK/badsid.csr" \
  >/dev/null 2>&1 &&
  bad "CSR with malformed SPIFFE ID accepted" ||
  ok "CSR with malformed SPIFFE ID rejected"

# CN-less (SAN-only) CSR, the certbot shape: CN derived from the dns SAN
openssl req -new -key "$WORK/ee.key" -out "$WORK/nocn.csr" -subj "/" \
  -addext "subjectAltName=DNS:nocn.example.ca" 2>/dev/null
NONCE_NC="$(w get nonce --id user@example.ca 2>/dev/null)"
[ "$(w sign server --id user@example.ca --nonce "$NONCE_NC" --csr "$WORK/nocn.csr" 2>/dev/null)" \
  = "nocn.example.ca" ] &&
  ok "CN-less CSR: CN derived from dns SAN" || bad "CN-less CSR"

# policing: wrong key type, missing SAN (client), tampered CSR, wrong id
openssl req -new -newkey rsa:2048 -nodes -keyout "$WORK/rsa.key" \
  -out "$WORK/rsa.csr" -subj "/CN=rsa.example.ca" 2>/dev/null
NONCE4="$(w get nonce --id user@example.ca 2>/dev/null)"
w sign server --id user@example.ca --nonce "$NONCE4" --csr "$WORK/rsa.csr" >/dev/null 2>&1 &&
  bad "RSA CSR accepted" || ok "RSA CSR rejected (ee_curve policy)"
openssl req -new -key "$WORK/ee.key" -out "$WORK/nosan.csr" -subj "/CN=No San" 2>/dev/null
w sign client --id user@example.ca --nonce "$NONCE4" --csr "$WORK/nosan.csr" >/dev/null 2>&1 &&
  bad "client CSR without SAN accepted" || ok "client CSR without SAN rejected"
sed '3s/[A-Za-z]/x/' "$WORK/cl.csr" >"$WORK/tampered.csr"
w sign client --id user@example.ca --nonce "$NONCE4" --csr "$WORK/tampered.csr" >/dev/null 2>&1 &&
  bad "tampered CSR accepted" || ok "tampered CSR rejected (bad signature)"
openssl req -new -key "$WORK/ee.key" -out "$WORK/ok.csr" \
  -subj "/CN=late.example.ca" 2>/dev/null
w sign server --id ghost@example.ca --nonce "$NONCE4" --csr "$WORK/ok.csr" >/dev/null 2>&1 &&
  bad "unenrolled id signed" || ok "sign rejects unenrolled id"
grep -q "requested by 'user@example.ca'" "$LOG" &&
  ok "issuance logged with requesting id" || bad "requested-by audit line missing"

# uniqueness applies to the CSR path too (csr.example.ca is still active)
w sign server --id user@example.ca --nonce "$NONCE4" --csr "$WORK/evil.csr" >/dev/null 2>&1 &&
  bad "duplicate active CN via CSR accepted" || ok "duplicate active CN via CSR rejected"
# and revocation treats CSR-signed certs like any other EE cert
w revoke server --cn csr.example.ca --reason superseded >/dev/null 2>&1 &&
  ok "revoke CSR-signed cert" || bad "revoke CSR-signed cert"

# --- unusable store: content-based init check; leftovers are never touched ---
PKID="$WORK/pki_dead"
d() { "$BIN" --config "$CFG" --store "$PKID" "$@"; }
d init >/dev/null 2>&1 && ok "dead-store suite: init" || bad "dead-store suite: init"
: >"$PKID/ca-store.db" # truncate, as an interrupted copy/restore would
ERR="$(d get config 2>&1 >/dev/null)"
printf '%s' "$ERR" | grep -q "not initialized" &&
  ok "truncated store reads as not initialized" || bad "truncated store error: '$ERR'"
OUT="$(d init 2>&1)" &&
  bad "re-init over leftovers accepted" || ok "re-init over leftovers refused"
printf '%s' "$OUT" | grep -q "move it away" &&
  ok "refusal shown on console with a next step" || bad "refusal message: '$OUT'"
[ -f "$PKID/ca-store.db" ] && ok "leftovers not moved or deleted" || bad "init touched the leftovers"
grep -q "move it away" "$LOG" &&
  ok "refusal recorded in the log" || bad "refusal log line missing"
mv "$PKID" "$PKID.dead" # the user's decision, not yca's
d init >/dev/null 2>&1 && ok "init clean after user moved the leftovers" || bad "init after move"
d get config >/dev/null 2>&1 && ok "store usable after re-init" || bad "store unusable after re-init"
echo garbage >"$PKID/ca-store.db" # not a SQLite file at all
d get config >/dev/null 2>&1 &&
  bad "garbage store accepted" || ok "garbage store reads as not initialized"
mkdir -p "$WORK/pki_premade" # e.g. a mountpoint: empty pre-existing dir is fine
"$BIN" --config "$CFG" --store "$WORK/pki_premade" init >/dev/null 2>&1 &&
  ok "pre-existing empty dir accepted" || bad "pre-existing empty dir refused"

# --- --log: file logging is not optional; empty value falls back with a warn ---
ERR="$(w --log "" get config 2>&1 >/dev/null)"
printf '%s' "$ERR" | grep -q "empty --log ignored" &&
  ok "empty --log warns on console" || bad "empty --log: '$ERR'"
grep -q "empty --log ignored" "$LOG" &&
  ok "empty --log fallback recorded in the default log" || bad "empty --log warn not in log"
w --log "$WORK/custom.log" get server --cn ghost.example.ca >/dev/null 2>&1
[ -s "$WORK/custom.log" ] && ok "--log redirects the file log" || bad "--log ignored"
ERR="$(w --log "$WORK/nodir/x.log" get config 2>&1 >/dev/null)" &&
  bad "unwritable --log accepted" || ok "unwritable --log is fatal"
printf '%s' "$ERR" | grep -q "cannot open" &&
  ok "unwritable --log explained on stderr" || bad "unwritable --log: '$ERR'"

echo
echo "e2e: $PASS passed, $FAIL failed"
[ "$FAIL" -eq 0 ]

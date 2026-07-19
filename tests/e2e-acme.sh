#!/usr/bin/env bash
# End-to-end tests for the ACME frontend (acme/, binary yca-acme) against a
# temp PKI on the internal backend, driven by a real ACME client (acme.sh).
# The daemon serves TLS with a certificate issued by the very CA under test;
# http-01 tokens are served from a webroot via python3. Skips (exit 77)
# without go, acme.sh or python3.
# Usage: e2e-acme.sh <path-to-yca-binary>
# shellcheck disable=SC2015  # `cmd && ok || bad` is the assert idiom; ok/bad never fail
set -u

BIN="$(realpath "${1:?usage: e2e-acme.sh <yca-binary>}")"
ACME_SRC="$(cd "$(dirname "$0")/../acme" && pwd)"

for dep in go acme.sh python3; do
  if ! command -v "$dep" >/dev/null 2>&1; then
    echo "e2e-acme: SKIPPED ($dep not installed)"
    exit 77
  fi
done

WORK="$(mktemp -d)"
DAEMON_PID=""
WEBROOT_PID=""
cleanup() {
  [ -n "$DAEMON_PID" ] && kill "$DAEMON_PID" 2>/dev/null
  [ -n "$WEBROOT_PID" ] && kill "$WEBROOT_PID" 2>/dev/null
  rm -rf "$WORK"
}
trap cleanup EXIT
CFG="$WORK/yca.toml"
PKI="$WORK/pki"
cd "$WORK" || exit 1

cat >"$CFG" <<'EOF'
org_name = "Example"
country_code = "CA"
repository_host = "pki.example.ca"
root_ca_cn = "ETS Root E1"
root_ca_curve = "secp384r1"
root_ca_digest = "SHA-384"
root_ca_valid_days = 8192
root_ca_slug = "ets-root-e1"
signing_ca_cn = "CA E1"
signing_ca_curve = "secp384r1"
signing_ca_digest = "SHA-384"
signing_ca_valid_days = 8112
signing_ca_slug = "ca-e1"
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

# --- build the frontend ---
(cd "$ACME_SRC" && go build -o "$WORK/yca-acme" .) &&
  ok "go build yca-acme" || { bad "go build failed"; exit 1; }
"$WORK/yca-acme" --version | grep -q '^yca-acme version ' &&
  ok "yca-acme --version" || bad "yca-acme --version"

# --- PKI: init, enroll the frontend identity, TLS cert for the daemon ---
export CA_STORE_PASSPHRASE=e2e-acme-pass
w init >/dev/null 2>&1 && ok "init" || bad "init"
w enroll --id acme >/dev/null 2>&1 && ok "enroll acme" || bad "enroll acme"
# CN deliberately != the ACME identifier: an active cert for the same
# (CN, profile) would push the ACME order into the renewal-window rule.
w create server --cn acme-daemon --san dns:localhost >/dev/null 2>&1 &&
  ok "TLS cert for the daemon (bootstrap ceremony)" || bad "create acme-daemon"
w get ca --cn root-ca >"$WORK/root.pem" 2>/dev/null &&
  ok "root exported" || bad "get ca root-ca"
cat "$PKI/ee/acme-daemon.crt" "$PKI/ca/ca-e1.pem" >"$WORK/tls-chain.pem"

# --- start the daemon (TLS) and the http-01 webroot ---
PORT=$((20000 + RANDOM % 20000))
VAPORT=$((PORT + 1))
DIR_URL="https://localhost:$PORT/acme/directory"
# --valid 90d: the daemon requests a shorter validity than the CA's
# ee_valid_days ceiling (397) - asserted on the issued leaf below.
"$WORK/yca-acme" --state "$WORK/acme.db" --listen "127.0.0.1:$PORT" \
  --url "https://localhost:$PORT" --yca "$BIN" --config "$CFG" \
  --store "$PKI" --id acme --http01-port "$VAPORT" --valid 90d \
  --tls-cert "$WORK/tls-chain.pem" --tls-key "$PKI/ee/acme-daemon.key" \
  >"$WORK/daemon.log" 2>&1 &
DAEMON_PID=$!
mkdir -p "$WORK/webroot"
(cd "$WORK/webroot" && python3 -m http.server "$VAPORT" >/dev/null 2>&1) &
WEBROOT_PID=$!
up=""
for _ in $(seq 1 50); do
  [ "$(curl -s -o /dev/null --cacert "$WORK/root.pem" \
    -w '%{http_code}' "https://localhost:$PORT/healthz")" = 200 ] &&
    up=1 && break
  sleep 0.1
done
[ -n "$up" ] && ok "daemon up over TLS (own CA trust)" ||
  { bad "daemon did not start"; cat "$WORK/daemon.log"; exit 1; }
curl -s --cacert "$WORK/root.pem" "$DIR_URL" |
  grep -q '"externalAccountRequired": true' &&
  ok "directory advertises EAB" || bad "directory"
curl -s --cacert "$WORK/root.pem" "$DIR_URL" | grep -q '"revokeCert"' &&
  curl -s --cacert "$WORK/root.pem" "$DIR_URL" | grep -q '"renewalInfo"' &&
  ok "directory advertises revokeCert + renewalInfo (ARI)" ||
  bad "phase-3 directory entries missing"
curl -s --cacert "$WORK/root.pem" "$DIR_URL" | grep -q '"keyChange"' &&
  ok "directory advertises keyChange" || bad "keyChange missing"

# --- EAB provisioning ---
"$WORK/yca-acme" eab new --state "$WORK/acme.db" --allow "localhost" \
  >"$WORK/eab.txt" 2>&1 && ok "eab new" || bad "eab new"
EAB_KID="$(awk '$1 == "kid" {print $3}' "$WORK/eab.txt")"
EAB_HMAC="$(awk '$1 == "hmac:" {print $2}' "$WORK/eab.txt")"
[ -n "$EAB_KID" ] && [ -n "$EAB_HMAC" ] &&
  ok "eab credential parsed" || bad "eab output: $(cat "$WORK/eab.txt")"
# -F --: the kid is random base64url and can start with '-'.
"$WORK/yca-acme" eab list --state "$WORK/acme.db" | grep -qF -- "$EAB_KID" &&
  ok "eab list shows the credential" || bad "eab list"

# --- acme.sh: register + issue ---
AH="$WORK/acmesh"
a() {
  acme.sh --home "$AH" --config-home "$AH" --server "$DIR_URL" \
    --ca-bundle "$WORK/root.pem" "$@"
}
a --register-account --eab-kid "$EAB_KID" --eab-hmac-key "$EAB_HMAC" \
  >"$WORK/register.log" 2>&1 &&
  ok "acme.sh --register-account (EAB)" ||
  { bad "register: $(tail -3 "$WORK/register.log")"; }

a --issue -d localhost --webroot "$WORK/webroot" --keylength ec-256 \
  >"$WORK/issue.log" 2>&1 &&
  ok "acme.sh --issue localhost (http-01)" ||
  bad "issue: $(tail -5 "$WORK/issue.log")"

LEAF="$AH/localhost_ecc/localhost.cer"
FULL="$AH/localhost_ecc/fullchain.cer"
[ -s "$LEAF" ] && [ -s "$FULL" ] && ok "client received cert + chain" ||
  bad "client cert files missing"
openssl verify -CAfile "$WORK/root.pem" -untrusted "$PKI/ca/ca-e1.pem" \
  "$LEAF" >/dev/null 2>&1 &&
  ok "issued cert chains to the root" || bad "chain verify failed"
# The daemon's --valid 90d reached the CA (397-day policy is the ceiling).
# openssl date -> epoch: GNU date takes -d, BSD (macOS) needs -j -f.
epoch() {
  date -u -d "$1" +%s 2>/dev/null ||
    date -j -u -f "%b %e %H:%M:%S %Y %Z" "$1" +%s
}
LNB=$(openssl x509 -in "$LEAF" -noout -startdate 2>/dev/null | cut -d= -f2)
LNA=$(openssl x509 -in "$LEAF" -noout -enddate 2>/dev/null | cut -d= -f2)
LDAYS=$(( ($(epoch "$LNA") - $(epoch "$LNB")) / 86400 ))
[ "$LDAYS" -eq 90 ] &&
  ok "ACME leaf validity 90d (daemon --valid)" ||
  bad "ACME leaf validity ${LDAYS}d, want 90"
openssl x509 -in "$LEAF" -noout -ext subjectAltName 2>/dev/null |
  grep -q "DNS:localhost" && ok "SAN DNS:localhost" || bad "SAN missing"
# The CA dictates the profile, whatever the client asked for.
openssl x509 -in "$LEAF" -noout -ext extendedKeyUsage 2>/dev/null |
  grep -q "TLS Web Server Authentication" &&
  ok "EKU serverAuth (CA-dictated profile)" || bad "EKU wrong"
grep -q "requested by 'acme'" "$WORK/yca.log" &&
  ok "audit trail: requested by 'acme'" || bad "no audit line in yca.log"
w list --cn localhost 2>/dev/null | grep -q active &&
  ok "CA lists the ACME cert as active" || bad "list --cn localhost"

# --- immediate forced renewal: refused by the renewal window ---
a --renew -d localhost --ecc --force >"$WORK/renew.log" 2>&1 &&
  bad "immediate renewal accepted" ||
  ok "immediate renewal refused (renewal window)"
grep -q "renewal window" "$WORK/yca.log" &&
  ok "renewal-window refusal logged by the CA" || bad "no window log line"

# --- revocation via ACME (revokeCert, account-authorized) ---
a --revoke -d localhost --ecc >"$WORK/revoke.log" 2>&1 &&
  ok "acme.sh --revoke" || bad "revoke: $(tail -3 "$WORK/revoke.log")"
w list --cn localhost 2>/dev/null | grep -q revoked &&
  ok "revocation landed in the CA (exact serial)" || bad "cert not revoked"
w get server --cn localhost >/dev/null 2>&1 &&
  bad "revoked cert still served" || ok "revoked cert no longer active"

# --- bogus EAB rejected ---
acme.sh --home "$WORK/acmesh2" --config-home "$WORK/acmesh2" \
  --server "$DIR_URL" --ca-bundle "$WORK/root.pem" \
  --register-account --eab-kid bogus --eab-hmac-key "$EAB_HMAC" \
  >"$WORK/register2.log" 2>&1 &&
  bad "bogus EAB accepted" || ok "bogus EAB rejected"

# --- name outside the EAB allow-list rejected ---
a --issue -d forbidden.example.org --webroot "$WORK/webroot" \
  --keylength ec-256 >"$WORK/issue2.log" 2>&1 &&
  bad "identifier outside --allow accepted" ||
  ok "identifier outside --allow rejected"

# --- eab delete: bound accounts can no longer order ---
"$WORK/yca-acme" eab new --state "$WORK/acme.db" --allow "localhost" \
  >"$WORK/eab3.txt" 2>&1
EAB3_KID="$(awk '$1 == "kid" {print $3}' "$WORK/eab3.txt")"
EAB3_HMAC="$(awk '$1 == "hmac:" {print $2}' "$WORK/eab3.txt")"
a3() {
  acme.sh --home "$WORK/acmesh3" --config-home "$WORK/acmesh3" \
    --server "$DIR_URL" --ca-bundle "$WORK/root.pem" "$@"
}
a3 --register-account --eab-kid "$EAB3_KID" --eab-hmac-key "$EAB3_HMAC" \
  >/dev/null 2>&1 && ok "third account registered" || bad "third register"
"$WORK/yca-acme" eab delete --state "$WORK/acme.db" "$EAB3_KID" \
  >"$WORK/eabdel.txt" 2>&1 &&
  ok "eab delete" || bad "eab delete: $(cat "$WORK/eabdel.txt")"
grep -q "can no longer order" "$WORK/eabdel.txt" &&
  ok "eab delete names the bound account" || bad "no bound-account warning"
"$WORK/yca-acme" eab delete --state "$WORK/acme.db" "$EAB3_KID" \
  >/dev/null 2>&1 &&
  bad "double eab delete accepted" || ok "double eab delete rejected"
a3 --issue -d localhost --webroot "$WORK/webroot" --keylength ec-256 \
  >"$WORK/issue3.log" 2>&1 &&
  bad "order accepted after eab delete" ||
  ok "order refused after eab delete"
grep -q rejectedIdentifier "$WORK/issue3.log" &&
  ok "refusal is rejectedIdentifier" || bad "wrong refusal: $(tail -2 "$WORK/issue3.log")"

# --- account deactivation (RFC 8555 §7.3.6), the account's last word ---
a --deactivate-account >"$WORK/deact.log" 2>&1 &&
  ok "acme.sh --deactivate-account" ||
  bad "deactivate: $(tail -3 "$WORK/deact.log")"
a --issue -d localhost --webroot "$WORK/webroot" --keylength ec-256 --force \
  >"$WORK/issue4.log" 2>&1 &&
  bad "deactivated account still issues" ||
  ok "deactivated account refused"

echo "e2e-acme: $PASS passed, $FAIL failed"
[ "$FAIL" -eq 0 ] || exit 1

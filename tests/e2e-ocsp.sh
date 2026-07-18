#!/usr/bin/env bash
# End-to-end tests for the Go OCSP responder (responder/, binary yca-ocsp)
# against a temp PKI on the internal backend. Asserts with openssl ocsp and
# curl. Skips (exit 77) if the Go toolchain is not installed.
# Usage: e2e-ocsp.sh <path-to-yca-binary>
# shellcheck disable=SC2015  # `cmd && ok || bad` is the assert idiom; ok/bad never fail
set -u

BIN="${1:?usage: e2e-ocsp.sh <yca-binary>}"
RESPONDER_SRC="$(cd "$(dirname "$0")/../responder" && pwd)"

if ! command -v go >/dev/null 2>&1; then
  echo "e2e-ocsp: SKIPPED (go toolchain not installed)"
  exit 77
fi

WORK="$(mktemp -d)"
RESPONDER_PID=""
cleanup() {
  [ -n "$RESPONDER_PID" ] && kill "$RESPONDER_PID" 2>/dev/null
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
ocsp_cn = "CA E1 OCSP Responder"
ocsp_slug = "ca-e1-ocsp-responder"
ocsp_valid_days = 7
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

# --- build the responder ---
(cd "$RESPONDER_SRC" && go build -o "$WORK/yca-ocsp" .) &&
  ok "go build yca-ocsp" || { bad "go build failed"; exit 1; }
"$WORK/yca-ocsp" -version | grep -q '^yca-ocsp version ' &&
  ok "responder -version" || bad "responder -version"

# --- PKI + credentials ---
export CA_STORE_PASSPHRASE=e2e-ocsp-pass
w init >/dev/null 2>&1 && ok "init" || bad "init"
w create ocsp >/dev/null 2>&1 && ok "create ocsp" || bad "create ocsp"
w create server --cn s1.example.ca >/dev/null 2>&1 && ok "create server" || bad "create server"

# --- start the responder, wait for health ---
PORT=$((20000 + RANDOM % 20000))
URL="http://127.0.0.1:$PORT/ocsp"
"$WORK/yca-ocsp" --store "$PKI" --listen "127.0.0.1:$PORT" \
  >"$WORK/responder.log" 2>&1 &
RESPONDER_PID=$!
up=""
for _ in $(seq 1 50); do
  [ "$(curl -s -o /dev/null -w '%{http_code}' "http://127.0.0.1:$PORT/healthz")" = 200 ] &&
    up=1 && break
  sleep 0.1
done
[ -n "$up" ] && ok "responder up, healthz 200" || bad "responder did not start"

q() { # q <extra openssl ocsp args...>: query + print status/verify lines
  openssl ocsp -issuer "$PKI/ca/ca-e1.pem" -url "$URL" \
    -CAfile "$PKI/ca/ets-root-e1.pem" "$@" 2>&1
}

# --- good, signature verified (delegated responder cert embedded) ---
OUT="$(q -cert "$PKI/ee/s1.example.ca.crt")"
printf '%s' "$OUT" | grep -q "Response verify OK" &&
  ok "response signature verifies (delegated)" || bad "verify failed: $OUT"
printf '%s' "$OUT" | grep -q "s1.example.ca.crt: good" &&
  ok "active cert -> good" || bad "expected good: $OUT"

# --- revoked with reason ---
w revoke server --cn s1.example.ca --reason keyCompromise >/dev/null 2>&1 &&
  ok "revoke server" || bad "revoke server"
OUT="$(q -cert "$PKI/ee/s1.example.ca.crt")"
printf '%s' "$OUT" | grep -q "s1.example.ca.crt: revoked" &&
  ok "revoked cert -> revoked" || bad "expected revoked: $OUT"
printf '%s' "$OUT" | grep -q "Reason: keyCompromise" &&
  ok "revocation reason propagated" || bad "reason missing: $OUT"
printf '%s' "$OUT" | grep -q "Response verify OK" &&
  ok "revoked response verifies" || bad "revoked verify failed"

# --- unknown serial ---
OUT="$(q -serial 0xDEADBEEF)"
printf '%s' "$OUT" | grep -q "0xDEADBEEF: unknown" &&
  ok "foreign serial -> unknown" || bad "expected unknown: $OUT"

# --- sha256 CertID accepted ---
q -sha256 -cert "$PKI/ee/s1.example.ca.crt" | grep -q "Response verify OK" &&
  ok "sha256 CertID accepted" || bad "sha256 CertID rejected"

# --- GET variant (RFC 6960 A.1) returns the same status ---
if command -v python3 >/dev/null 2>&1; then
  REQ="$(openssl ocsp -issuer "$PKI/ca/ca-e1.pem" \
    -cert "$PKI/ee/s1.example.ca.crt" -reqout /dev/stdout 2>/dev/null | base64 -w0)"
  ENC="$(python3 -c 'import urllib.parse,sys;print(urllib.parse.quote(sys.argv[1],safe=""))' "$REQ")"
  curl -s --globoff "$URL/$ENC" -o "$WORK/get.der"
  openssl ocsp -respin "$WORK/get.der" -issuer "$PKI/ca/ca-e1.pem" \
    -cert "$PKI/ee/s1.example.ca.crt" -CAfile "$PKI/ca/ets-root-e1.pem" 2>&1 |
    grep -q "s1.example.ca.crt: revoked" &&
    ok "GET variant matches POST" || bad "GET variant"
fi

# --- foreign issuer -> unauthorized ---
openssl req -x509 -newkey ec -pkeyopt ec_paramgen_curve:P-256 -nodes \
  -keyout "$WORK/f.key" -out "$WORK/f.pem" -subj "/CN=Foreign CA" -days 1 \
  >/dev/null 2>&1
openssl ocsp -issuer "$WORK/f.pem" -serial 0x01 -url "$URL" 2>&1 |
  grep -qi "unauthorized" &&
  ok "foreign issuer -> unauthorized" || bad "foreign issuer not rejected"

# --- malformed request -> malformedRequest, uncached ---
[ "$(curl -s -X POST --data-binary garbage \
  -H 'Content-Type: application/ocsp-request' "$URL" | xxd -p)" = "30030a0101" ] &&
  ok "garbage -> malformedRequest" || bad "malformed handling"

# --- headers on signed responses ---
HDRS="$(curl -s -D- -o /dev/null -X POST --data-binary \
  @<(openssl ocsp -issuer "$PKI/ca/ca-e1.pem" -cert "$PKI/ee/s1.example.ca.crt" \
    -reqout /dev/stdout 2>/dev/null) "$URL")"
printf '%s' "$HDRS" | grep -qi "content-type: application/ocsp-response" &&
  ok "content-type header" || bad "content-type"
printf '%s' "$HDRS" | grep -qi "cache-control: public, max-age=" &&
  ok "cache-control max-age" || bad "cache-control"

echo
echo "e2e-ocsp: $PASS passed, $FAIL failed"
[ "$FAIL" -eq 0 ]

#!/usr/bin/env bash
# End-to-end tests for the pkcs11 key backend, on an ephemeral SoftHSM2 token.
# Exercises both init paths: generate (fresh token) and adopt (second store
# over the same token). Skips (exit 77) if SoftHSM2 is not installed.
# Usage: e2e-hsm.sh <path-to-yca-binary>
# shellcheck disable=SC2015  # `cmd && ok || bad` is the assert idiom; ok/bad never fail
set -u

BIN="$(realpath "${1:?usage: e2e-hsm.sh <yca-binary>}")"

MODULE=""
for m in /usr/lib/softhsm/libsofthsm2.so /usr/lib/pkcs11/libsofthsm2.so \
  /usr/lib/x86_64-linux-gnu/softhsm/libsofthsm2.so \
  /opt/homebrew/lib/softhsm/libsofthsm2.so \
  /usr/lib64/softhsm/libsofthsm2.so \
  /usr/local/lib/softhsm/libsofthsm2.so; do
  [ -f "$m" ] && MODULE="$m" && break
done
if ! command -v softhsm2-util >/dev/null 2>&1 || [ -z "$MODULE" ]; then
  echo "e2e-hsm: SKIPPED (softhsm2 not installed)"
  exit 77
fi

WORK="$(mktemp -d)"
trap 'rm -rf "$WORK"' EXIT
CFG="$WORK/yca.toml"
PKI="$WORK/pki"
PKI2="$WORK/pki2"
cd "$WORK" || exit 1
LOG="$WORK/yca.log"

# Ephemeral token: config + token dir live under $WORK.
export SOFTHSM2_CONF="$WORK/softhsm2.conf"
mkdir -p "$WORK/tokens"
cat >"$SOFTHSM2_CONF" <<EOF
directories.tokendir = $WORK/tokens
objectstore.backend = file
EOF
PIN=1234
softhsm2-util --init-token --free --label yca-hsm \
  --pin "$PIN" --so-pin 87654321 >/dev/null

# Botan signs X.509 through the hash-on-token mechanisms (CKM_ECDSA_SHAxxx),
# which SoftHSM offers only since 2.7.0 - see docs/softhsm.md. On the 2.6.1
# still shipped by Debian, init would fail at the first signature (PKCS11
# error 112) despite key generation working. Probe when pkcs11-tool is
# around; without it, proceed and let the suite speak.
if command -v pkcs11-tool >/dev/null 2>&1 &&
  ! pkcs11-tool --module "$MODULE" -M 2>/dev/null | grep -q "ECDSA-SHA"; then
  echo "e2e-hsm: SKIPPED (SoftHSM < 2.7.0: no CKM_ECDSA_SHAxxx mechanisms)"
  exit 77
fi

cat >"$CFG" <<EOF
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
root_arc_oid = "1.3.6.1.4.1.32473"
key_backend = "pkcs11"
pkcs11_module = "$MODULE"
pkcs11_token_label = "yca-hsm"
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

export CA_HSM_PIN="$PIN"

# --- init requires the PIN, not a passphrase ---
(env -u CA_HSM_PIN "$BIN" --config "$CFG" --store "$PKI" init >/dev/null 2>&1) &&
  bad "init without CA_HSM_PIN accepted" || ok "init without CA_HSM_PIN rejected"
OUT="$(w init 2>&1)" && ok "init (generate path) exits 0" || bad "init exit code"
printf '%s' "$OUT" | grep -q GENERATED &&
  bad "passphrase generated in pkcs11 mode" || ok "no passphrase generated"
grep -q "generating keypair 'ets-root-e1' on the token" "$LOG" &&
  ok "root key generated on token" || bad "root key not generated on token"

# --- keys live on the token, not in the store ---
[ "$(sqlite3 "$PKI/ca-store.db" 'SELECT COUNT(*) FROM keys')" = "0" ] &&
  ok "store keys table is empty" || bad "keys present in SQLite store"
if command -v pkcs11-tool >/dev/null 2>&1; then
  LISTING="$(pkcs11-tool --module "$MODULE" --login --pin "$PIN" \
    --list-objects --type privkey 2>/dev/null)"
  printf '%s' "$LISTING" | grep -q "ets-root-e1" &&
    ok "token holds root privkey" || bad "root privkey missing on token"
  printf '%s' "$LISTING" | grep -q "ca-e1" &&
    ok "token holds signing privkey" || bad "signing privkey missing on token"
fi

# --- issuance + chain (signature made by the token) ---
w create server --cn server.ca >/dev/null 2>&1 && ok "create server" || bad "create server"
openssl verify -CAfile "$PKI/ca/ets-root-e1.pem" \
  -untrusted "$PKI/ca/ca-e1.pem" "$PKI/ee/server.ca.crt" >/dev/null 2>&1 &&
  ok "EE chain verifies to root" || bad "chain verify failed"

# --- wrong PIN rejected (no retry loop; softhsm has no lockout) ---
(export CA_HSM_PIN=9999; w create server --cn wp.ca >/dev/null 2>&1) &&
  bad "wrong PIN accepted" || ok "wrong PIN rejected"

# --- revoke: CRL signed by the token key ---
w revoke server --cn server.ca --reason keyCompromise >/dev/null 2>&1 &&
  ok "revoke server" || bad "revoke server"
openssl crl -inform DER -in "$PKI/ca/ca-e1.crl" -CAfile "$PKI/ca/ca-e1.pem" -noout >/dev/null 2>&1 &&
  ok "signing CRL is signed/valid" || bad "CRL verify failed"

# --- adopt path: a second store over the same token reuses the keys ---
"$BIN" --config "$CFG" --store "$PKI2" init >/dev/null 2>&1 &&
  ok "init (adopt path) exits 0" || bad "adopt init failed"
grep -q "using existing token keypair 'ets-root-e1'" "$LOG" &&
  ok "existing root keypair adopted" || bad "adopt not logged"
openssl x509 -in "$PKI/ca/ets-root-e1.pem" -pubkey -noout >"$WORK/pub1" 2>/dev/null
openssl x509 -in "$PKI2/ca/ets-root-e1.pem" -pubkey -noout >"$WORK/pub2" 2>/dev/null
diff -q "$WORK/pub1" "$WORK/pub2" >/dev/null &&
  ok "adopted root has the same public key" || bad "adopted key differs"

# --- config guard: pkcs11_* without the backend ---
grep -v '^key_backend' "$CFG" >"$WORK/half.toml"
"$BIN" --config "$WORK/half.toml" --store "$WORK/pki3" init >/dev/null 2>&1 &&
  bad "pkcs11_* without backend accepted" || ok "pkcs11_* without backend rejected"

echo
echo "e2e-hsm: $PASS passed, $FAIL failed"
[ "$FAIL" -eq 0 ]

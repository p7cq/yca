#!/usr/bin/env bash
# End-to-end tests for the pkcs11 key backend, on ephemeral SoftHSM2 tokens.
# Covers the single-token layout (both init paths: generate on a fresh
# token, adopt on a second store over the same token), the split-token
# layout (root key on its own token with its own PIN) and the hybrid
# layout (pkcs11 root, internal signing). Skips (exit 77) if SoftHSM2 is
# not installed.
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

# =========================================================================
# Split-token layout: the root key on its own token with its own PIN.
# =========================================================================
RPIN=5678
softhsm2-util --init-token --free --label yca-split-root \
  --pin "$RPIN" --so-pin 87654321 >/dev/null

CFG2="$WORK/split.toml"
sed -e 's/^root_ca_cn = .*/root_ca_cn = "SR Root E1"/' \
  -e 's/^signing_ca_cn = .*/signing_ca_cn = "SR CA E1"/' \
  -e 's/^root_ca_slug_prefix = .*/root_ca_slug_prefix = "sr-root-e"/' \
  -e 's/^signing_ca_slug_prefix = .*/signing_ca_slug_prefix = "sr-ca-e"/' \
  "$CFG" >"$CFG2"
echo 'pkcs11_root_token_label = "yca-split-root"' >>"$CFG2"
SPKI="$WORK/pki-split"
w2() { "$BIN" --config "$CFG2" --store "$SPKI" "$@"; }

# The root PIN falls back to CA_HSM_PIN, which is wrong for the root token.
(unset CA_HSM_ROOT_PIN
  "$BIN" --config "$CFG2" --store "$WORK/pki-splitx" init >/dev/null 2>&1) &&
  bad "split init with fallback PIN accepted" ||
  ok "split init needs the root token PIN (fallback is the signing PIN)"

export CA_HSM_ROOT_PIN="$RPIN"
w2 init >/dev/null 2>&1 && ok "split init exits 0" || bad "split init"
grep -q "generating keypair 'sr-root-e1' on the token" "$LOG" &&
  ok "split root key generated" || bad "split root key not generated"
if command -v pkcs11-tool >/dev/null 2>&1; then
  ROOT_LST="$(pkcs11-tool --module "$MODULE" --token-label yca-split-root \
    --login --pin "$RPIN" --list-objects --type privkey 2>/dev/null)"
  SIGN_LST="$(pkcs11-tool --module "$MODULE" --token-label yca-hsm \
    --login --pin "$PIN" --list-objects --type privkey 2>/dev/null)"
  printf '%s' "$ROOT_LST" | grep -q "sr-root-e1" &&
    ok "root token holds the root key" || bad "root key not on the root token"
  printf '%s' "$ROOT_LST" | grep -q "sr-ca-e1" &&
    bad "signing key leaked onto the root token" ||
    ok "signing key not on the root token"
  printf '%s' "$SIGN_LST" | grep -q "sr-ca-e1" &&
    ok "signing token holds the signing key" ||
    bad "signing key not on the signing token"
fi

# Adopt path: a second split store over the same two tokens reuses the keys.
"$BIN" --config "$CFG2" --store "$WORK/pki-split2" init >/dev/null 2>&1 &&
  ok "split init (adopt path) exits 0" || bad "split adopt init"
grep -q "using existing token keypair 'sr-root-e1'" "$LOG" &&
  ok "split root keypair adopted" || bad "split adopt not logged"

# Daily issuance must not touch the root token: a wrong root PIN is
# irrelevant to create and to the signing CRL scope.
(export CA_HSM_ROOT_PIN=9999; w2 create server --cn split.ca >/dev/null 2>&1) &&
  ok "create with only the signing token" || bad "create touched the root token"
openssl verify -CAfile "$SPKI/ca/sr-root-e1.pem" \
  -untrusted "$SPKI/ca/sr-ca-e1.pem" "$SPKI/ee/split.ca.crt" >/dev/null 2>&1 &&
  ok "split EE chain verifies" || bad "split chain verify failed"
(export CA_HSM_ROOT_PIN=9999; w2 refresh crl signing >/dev/null 2>&1) &&
  ok "signing CRL refresh with only the signing token" ||
  bad "signing refresh touched the root token"

# Root ceremonies must not touch the signing token.
(export CA_HSM_PIN=9999; w2 refresh crl root >/dev/null 2>&1) &&
  ok "root CRL refresh with only the root token" ||
  bad "root refresh touched the signing token"

# The rotation ceremony needs both tokens.
(export CA_HSM_ROOT_PIN=9999; w2 renew signing-ca --new-cn "SR CA E2" \
  >/dev/null 2>&1) &&
  bad "renew accepted a wrong root PIN" || ok "renew needs the root token"
w2 renew signing-ca --new-cn "SR CA E2" >/dev/null 2>&1 &&
  ok "renew with both tokens" || bad "renew with both tokens"
(export CA_HSM_PIN=9999; w2 revoke ca --cn "SR CA E1" --reason cACompromise \
  >/dev/null 2>&1) &&
  ok "revoke ca with only the root token" ||
  bad "revoke ca touched the signing token"
unset CA_HSM_ROOT_PIN

# =========================================================================
# Hybrid layout: pkcs11 root (token in the safe), internal signing key.
# =========================================================================
HPIN=9012
HPASS="hy-pass-0000"
softhsm2-util --init-token --free --label yca-hy-root \
  --pin "$HPIN" --so-pin 87654321 >/dev/null

CFG3="$WORK/hybrid.toml"
sed -e 's/^root_ca_cn = .*/root_ca_cn = "HY Root E1"/' \
  -e 's/^signing_ca_cn = .*/signing_ca_cn = "HY CA E1"/' \
  -e 's/^root_ca_slug_prefix = .*/root_ca_slug_prefix = "hy-root-e"/' \
  -e 's/^signing_ca_slug_prefix = .*/signing_ca_slug_prefix = "hy-ca-e"/' \
  -e '/^key_backend/d' -e '/^pkcs11_token_label/d' \
  "$CFG" >"$CFG3"
{
  echo 'root_key_backend = "pkcs11"'
  echo 'pkcs11_root_token_label = "yca-hy-root"'
} >>"$CFG3"
HPKI="$WORK/pki-hy"
w3() { "$BIN" --config "$CFG3" --store "$HPKI" "$@"; }

# The internal signing key wants a passphrase: one is generated when unset.
OUT="$(env -u CA_HSM_PIN -u CA_STORE_PASSPHRASE CA_HSM_ROOT_PIN="$HPIN" \
  "$BIN" --config "$CFG3" --store "$WORK/pki-hy-gen" init 2>&1)" &&
  ok "hybrid init (generated passphrase) exits 0" || bad "hybrid gen init"
printf '%s' "$OUT" | grep -q GENERATED &&
  ok "hybrid generates a passphrase" || bad "no passphrase generated"

export CA_STORE_PASSPHRASE="$HPASS"
(unset CA_HSM_PIN
  export CA_HSM_ROOT_PIN="$HPIN"
  w3 init >/dev/null 2>&1) &&
  ok "hybrid init exits 0 (no signing PIN needed)" || bad "hybrid init"
grep -q "generating keypair 'hy-root-e1' on the token" "$LOG" &&
  ok "hybrid root key generated on token" || bad "hybrid root key not on token"
[ "$(sqlite3 "$HPKI/ca-store.db" 'SELECT COUNT(*) FROM keys')" = "1" ] &&
  ok "store holds exactly the signing key" || bad "unexpected store keys count"
if command -v pkcs11-tool >/dev/null 2>&1; then
  HY_LST="$(pkcs11-tool --module "$MODULE" --token-label yca-hy-root \
    --login --pin "$HPIN" --list-objects --type privkey 2>/dev/null)"
  printf '%s' "$HY_LST" | grep -q "hy-root-e1" &&
    ok "hybrid token holds the root key" || bad "root key missing on token"
  printf '%s' "$HY_LST" | grep -q "hy-ca-e1" &&
    bad "signing key leaked onto the token" || ok "signing key stays software"
fi

# Issuance and the signing CRL scope run on the passphrase alone.
(unset CA_HSM_PIN CA_HSM_ROOT_PIN
  w3 create server --cn hy.ca >/dev/null 2>&1) &&
  ok "create with no token PIN" || bad "create wanted a PIN"
openssl verify -CAfile "$HPKI/ca/hy-root-e1.pem" \
  -untrusted "$HPKI/ca/hy-ca-e1.pem" "$HPKI/ee/hy.ca.crt" >/dev/null 2>&1 &&
  ok "hybrid EE chain verifies" || bad "hybrid chain verify failed"
(export CA_STORE_PASSPHRASE=wrong; w3 create server --cn hw.ca \
  >/dev/null 2>&1) &&
  bad "wrong passphrase accepted" || ok "wrong passphrase rejected"
(unset CA_HSM_PIN CA_HSM_ROOT_PIN
  w3 refresh crl signing >/dev/null 2>&1) &&
  ok "signing CRL refresh on the passphrase alone" || bad "signing refresh"

# Root ceremonies want the root PIN (and only it for the root CRL).
(unset CA_HSM_PIN CA_HSM_ROOT_PIN
  w3 refresh crl root >/dev/null 2>&1) &&
  bad "root refresh without a PIN accepted" || ok "root refresh needs the PIN"
(unset CA_HSM_PIN CA_STORE_PASSPHRASE
  export CA_HSM_ROOT_PIN="$HPIN"
  w3 refresh crl root >/dev/null 2>&1) &&
  ok "root CRL refresh with only the root PIN" || bad "root refresh"
(unset CA_HSM_PIN CA_HSM_ROOT_PIN
  w3 renew signing-ca --new-cn "HY CA E2" >/dev/null 2>&1) &&
  bad "renew without the root PIN accepted" || ok "renew needs the root PIN"
(CA_HSM_ROOT_PIN="$HPIN" w3 renew signing-ca --new-cn "HY CA E2" \
  >/dev/null 2>&1) &&
  ok "renew with PIN + passphrase" || bad "hybrid renew"
[ "$(sqlite3 "$HPKI/ca-store.db" 'SELECT COUNT(*) FROM keys')" = "2" ] &&
  ok "new signing key persisted in the store" || bad "renewed key not stored"

# With the root token gone (in the safe), daily operation is unaffected
# and root ceremonies fail cleanly.
softhsm2-util --delete-token --token yca-hy-root >/dev/null 2>&1
(unset CA_HSM_PIN CA_HSM_ROOT_PIN
  w3 create server --cn hy2.ca >/dev/null 2>&1) &&
  ok "create with the root token removed" || bad "create needs the root token"
(CA_HSM_ROOT_PIN="$HPIN" w3 refresh crl root >/dev/null 2>&1) &&
  bad "root refresh without the token accepted" ||
  ok "root refresh fails without the token"
unset CA_STORE_PASSPHRASE

echo
echo "e2e-hsm: $PASS passed, $FAIL failed"
[ "$FAIL" -eq 0 ]

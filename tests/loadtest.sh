#!/usr/bin/env bash
# Load-test harness: seed the store at ramp sizes and time operations.
# Usage: loadtest.sh <yca-binary> [size ...]   (default ramp: 1000 10000 100000)
# NOTE: build a Release binary first (`./build.sh release`) - the debug/ASan
# binary is several times slower and reserves ~20T VIRT (ASan shadow).
set -u

BIN="${1:?usage: loadtest.sh <yca-binary> [size ...]}"
SEED_BIN="$(dirname "$BIN")/$(basename "$BIN")-seed"
[ -x "$SEED_BIN" ] || { echo "$(basename "$BIN")-seed not found next to $BIN" >&2; exit 1; }
shift
SIZES=("$@")
[ "${#SIZES[@]}" -eq 0 ] && SIZES=(1000 10000 100000)

WORK="$(mktemp -d)"
trap 'rm -rf "$WORK"' EXIT
CFG="$WORK/config.test.toml"
cat >"$CFG" <<'EOF'
org_name = "Load Test"
country_code = "CA"
repository_host = "pki.test.ca"
root_ca_cn = "TTS Root Load Test"
root_ca_curve = "secp384r1"
root_ca_digest = "SHA-384"
root_ca_valid_days = 8192
root_ca_slug_prefix = "tts-root-load-e"
signing_ca_cn = "CA Load Test"
signing_ca_curve = "secp384r1"
signing_ca_digest = "SHA-384"
signing_ca_valid_days = 8112
signing_ca_slug_prefix = "ca-load-e"
ee_curve = "secp256r1"
ee_digest = "SHA-256"
ee_valid_days = 397
root_arc_oid = "1.3.6.1.4.1.00000"
EOF

export CA_STORE_PASSPHRASE=loadtest
ms() { # echo elapsed milliseconds of the given command
  local s e
  s=$(date +%s%N)
  "$@" >/dev/null 2>&1
  e=$(date +%s%N)
  echo $(((e - s) / 1000000))
}

printf "%-10s %-9s %-9s %-9s %-11s %-7s %-8s\n" \
  count seed_s list_ms get_ms revoke_ms db_MB crl_MB
for N in "${SIZES[@]}"; do
  PKI="$WORK/pki_$N"
  "$BIN" --config "$CFG" --store "$PKI" init >/dev/null 2>&1
  W() { "$BIN" --config "$CFG" --store "$PKI" "$@"; }

  s=$(date +%s)
  "$SEED_BIN" --config "$CFG" --store "$PKI" --count "$N" >/dev/null 2>&1
  seed_s=$(($(date +%s) - s))

  list_ms=$(ms W list --expiring 30)
  get_ms=$(ms W get server --cn s0.test.ca)
  revoke_ms=$(ms W revoke server --cn s0.test.ca --reason superseded)
  db_mb=$(du -m "$PKI/ca-store.db" | cut -f1)
  crl_mb=$(du -m "$PKI/ca/ca-load-e1.crl" 2>/dev/null | cut -f1)

  printf "%-10s %-9s %-9s %-9s %-11s %-7s %-8s\n" \
    "$N" "$seed_s" "$list_ms" "$get_ms" "$revoke_ms" "${db_mb:-?}" "${crl_mb:-?}"
done

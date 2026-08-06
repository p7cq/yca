#!/usr/bin/env bash
# Smoke test for the zsh completion scripts (share/zsh-completion/_yca,
# _yca-acme). Every option spec is fed to a real completion widget in a
# throwaway zsh: a malformed spec makes _arguments print
# "invalid option definition" (or another comparguments diagnostic) and
# offer nothing, which this suite catches. It does not check that the
# completions are *correct*, only that they parse and produce candidates.
# Skips (exit 77) if zsh or its zpty module is unavailable.
# Usage: completion.sh [path-to-share/zsh-completion]
# shellcheck disable=SC2015  # `cmd && ok || bad` is the assert idiom; ok/bad never fail
set -u

COMPDIR="${1:-$(dirname "$0")/../share/zsh-completion}"
COMPDIR="$(cd "$COMPDIR" 2>/dev/null && pwd)" || {
  echo "completion: FAIL (no completion directory: ${1:-<default>})"
  exit 1
}

command -v zsh >/dev/null 2>&1 || {
  echo "completion: SKIPPED (zsh not installed)"
  exit 77
}
zsh -fc 'zmodload zsh/zpty' >/dev/null 2>&1 || {
  echo "completion: SKIPPED (zsh/zpty module unavailable)"
  exit 77
}

WORK="$(mktemp -d)"
trap 'rm -rf "$WORK"' EXIT

# The driver spawns one interactive zsh on a pty, loads the completions from
# $COMPDIR, then replays every command line it is given: each is sent
# followed by a TAB, and whatever the terminal echoes back is emitted
# between markers for the caller to grep. Ctrl-U clears the line before the
# next case so a completion that offered nothing cannot bleed into it.
# Output is read adaptively (poll until the terminal goes quiet) rather than
# on a fixed sleep, which keeps the whole suite to a few seconds.
cat >"$WORK/drive.zsh" <<'DRIVER'
zmodload zsh/zpty
# The CSI stripping below uses (#b) and the # closure, both of which need
# extendedglob; zsh -f does not set it.
setopt extendedglob
compdir=$1
cache=$2
shift 2

# Collect pty output until it stays silent for $1 consecutive polls
# (0.05s each), or $2 polls have elapsed overall.
slurp() {
  local quiet=0 total=0 chunk out=''
  while (( quiet < $1 && total < $2 )); do
    if zpty -r -t z chunk 2>/dev/null; then
      out+=$chunk
      quiet=0
    else
      (( quiet++ ))
    fi
    (( total++ ))
    sleep 0.05
  done
  print -rn -- $out
}

zpty z "PS1='RDY> ' zsh -f"
zpty -w z "fpath=($compdir \$fpath); autoload -U compinit; compinit -u -d $cache"
zpty -w z 'zstyle ":completion:*" menu no; setopt nobeep'
slurp 6 200 >/dev/null

for line in "$@"; do
  # -n is mandatory: without it zpty appends a newline, so every probe is
  # completed *and then executed*. That runs real yca commands; `yca-acme`
  # in particular starts a listening ACME server that holds the pty and
  # starves every later probe.
  zpty -w -n z "$line"$'\t'
  out=$(slurp 8 200)
  # CSI/OSC stripping: the pty echoes cursor moves and line-erase
  # sequences around the completion listing.
  out=${out//$'\r'/}
  print -r -- "<<<PROBE"
  print -r -- ${out//(#b)$'\e'\[[0-9;?]#[a-zA-Z]/}
  print -r -- "PROBE>>>"
  zpty -w -n z $'\C-u'
  slurp 3 40 >/dev/null
done
zpty -d z
DRIVER

# Cases are "<command line>|<string expected among the candidates>". The
# separator is | rather than whitespace so that the trailing space of the
# command line is explicit and cannot be lost: a line ending in a space asks
# for the *next* word (the positional), one ending in a word re-completes
# that word instead. Both the option list and the positional are exercised
# per subcommand. `yca add` is the case that regressed: an unescaped [ inside
# the --purpose description terminated the spec early.
CASES=(
  'yca --|--config'
  'yca |revoke'
  'yca create --|--san'
  'yca create |client'
  'yca enroll --|--id'
  'yca sign --|--csr'
  'yca sign |email'
  'yca revoke --|--reason'
  'yca revoke |ca'
  'yca add --|--purpose'
  'yca add |signing-ca'
  'yca renew --|--new-cn'
  'yca refresh |crl'
  'yca get --|--encoding'
  'yca get |nonce'
  'yca list --|--expiring'
  'yca-acme --|--http01-port'
  'yca-acme |eab'
  'yca-acme ari --|--window'
  'yca-acme ari |accelerate'
  'yca-acme eab --|--allow'
  'yca-acme eab |delete'
)

LINES=()
WANTS=()
for c in "${CASES[@]}"; do
  LINES+=("${c%%|*}")
  WANTS+=("${c##*|}")
done

echo "completion: driving \$COMPDIR=$COMPDIR"
zsh -f "$WORK/drive.zsh" "$COMPDIR" "$WORK/zcompdump" "${LINES[@]}" \
  >"$WORK/out" 2>&1 || {
  echo "completion: FAIL (driver died)"
  cat "$WORK/out"
  exit 1
}

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

# Split the driver transcript back into one file per probe, in order.
awk -v dir="$WORK" '
  /^<<<PROBE$/ { n++; f = dir "/probe." n; next }
  /^PROBE>>>$/ { next }
  n { print > f }
' "$WORK/out"

i=0
for line in "${LINES[@]}"; do
  i=$((i + 1))
  out="$WORK/probe.$i"
  if [ ! -f "$out" ]; then
    bad "$line -> no output captured"
    continue
  fi
  # A malformed spec is reported by comparguments and offers nothing;
  # assert on the diagnostic first so the message names the real cause.
  if grep -Eq '_arguments:|comparguments|invalid option definition|bad option' "$out"; then
    bad "$line -> $(grep -Em1 '_arguments:|comparguments|bad option' "$out")"
    continue
  fi
  # -w so a short candidate cannot be satisfied by the echoed command line
  # itself: without it `ca` matches the `yca` in the prompt echo.
  grep -qwF -- "${WANTS[$((i - 1))]}" "$out" &&
    ok "$line offers ${WANTS[$((i - 1))]}" ||
    bad "$line did not offer ${WANTS[$((i - 1))]}"
done

echo
echo "completion: $PASS passed, $FAIL failed"
[ "$FAIL" -eq 0 ]

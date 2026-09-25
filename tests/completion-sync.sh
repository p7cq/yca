#!/usr/bin/env bash
# Keeps the shell completions in step with the binaries.
# Skips (exit 77) if go is unavailable to build yca-acme.
# Usage: completion-sync.sh <yca binary> <acme source dir> <share dir>
# shellcheck disable=SC2015  # `cmd && ok || bad` is the assert idiom; ok/bad never fail
set -u

YCA="${1:?usage: completion-sync.sh <yca binary> <acme source dir> <share dir>}"
ACME_SRC="${2:?missing acme source dir}"
SHARE="${3:?missing share dir}"
YCA="$(realpath "$YCA")"
ACME_SRC="$(realpath "$ACME_SRC")"
SHARE="$(realpath "$SHARE")"
ZSH_YCA="$SHARE/zsh-completion/_yca"
ZSH_ACME="$SHARE/zsh-completion/_yca-acme"
BASH_YCA="$SHARE/bash-completion/yca"
BASH_ACME="$SHARE/bash-completion/yca-acme"

command -v go >/dev/null 2>&1 || {
  echo "completion-sync: SKIPPED (go not installed)"
  exit 77
}

WORK="$(mktemp -d)"
trap 'rm -rf "$WORK"' EXIT
fail=0
ok() { echo "ok   $1"; }
bad() {
  echo "FAIL $1"
  fail=1
}

# Sorted, de-duplicated, one line: "a b c".
norm() { tr ' ' '\n' | sed '/^$/d' | sort -u | tr '\n' ' ' | sed 's/ $//'; }

# same <name> <want: from the binary> <got: from a completion> [empty]
# An empty want fails unless the fourth argument allows it, so an
# extraction that silently matches nothing cannot pass.
same() {
  if [ -z "$2" ] && [ "${4-}" != empty ]; then
    bad "$1 (nothing extracted from the binary)"
  elif [ "$2" = "$3" ]; then
    ok "$1"
  else
    bad "$1"
    echo "  binary:     $2"
    echo "  completion: $3"
  fi
}

# bash_complete <script> <function> <words...>: the candidates for the last
# word, as Bash would offer them.
bash_complete() {
  bash --norc --noprofile -c '
    script=$1 func=$2
    shift 2
    . "$script"
    COMP_WORDS=("$@")
    COMP_CWORD=$(($# - 1))
    COMPREPLY=()
    "$func" 2>/dev/null
    printf "%s\n" "${COMPREPLY[@]}"
  ' _ "$@" | norm
}

# --- yca ------------------------------------------------------------------

# -h/--help are left out everywhere: every command has them.
no_help() { grep -vxE -- '-h|--help'; }

cli_cmds="$("$YCA" --help | awk '/^Subcommands:/ { f = 1; next } f && /^  [a-z]/ { print $1 }' | norm)"
cli_globals="$("$YCA" --help | awk '/^Options:/ { f = 1; next } /^$/ { f = 0 } f' |
  grep -oE '^  (-[a-z],)?--[a-z][a-z0-9-]+' | grep -oE -- '--[a-z0-9-]+$' | no_help | norm)"
cli_opts() {
  "$YCA" "$1" --help | sed -n '/^Options:/,$p' |
    grep -oE '^  (-[a-z],)?--[a-z][a-z0-9-]+' | grep -oE -- '--[a-z0-9-]+$' | no_help | norm
}
cli_pos() {
  "$YCA" "$1" --help | grep -oE '^  [a-z]+ TEXT:\{[^}]*\}' | sed -n "${2}p" |
    sed 's/.*{//; s/}//; s/,/ /g' | norm
}

zsh_cmds="$(awk '/local -a commands=\(/ { f = 1; next } f && /^ *\)/ { f = 0 } f' "$ZSH_YCA" |
  grep -oE "^ *'[a-z-]+:" | tr -d " ':" | norm)"
zsh_globals="$(awk '/_arguments -C/ { f = 1 } /case \$state/ { f = 0 } f' "$ZSH_YCA" |
  grep -oE -- "--[a-z][a-z0-9-]+('?\[|=)" | grep -oE -- '--[a-z0-9-]+' | no_help | norm)"
zsh_section() { awk -v c="$1" '$0 ~ "^ +" c "\\)$" { f = 1; next } f && /^ +;;$/ { f = 0 } f' "$ZSH_YCA"; }
zsh_opts() { zsh_section "$1" | grep -oE -- "--[a-z][a-z0-9-]+(\[|=)" | grep -oE -- '--[a-z0-9-]+' | norm; }
zsh_pos() { zsh_section "$1" | grep -oE "'$2:[a-z]+:\([^)]*\)'" | sed 's/.*(//; s/).*//' | norm; }

bash_opts() { bash_complete "$BASH_YCA" _yca yca "$1" - | tr ' ' '\n' | no_help | norm; }
bash_pos() {
  # bash_pos <cmd> <n>: positional n, after a valid choice for each earlier one
  local words=(yca "$1") n
  for ((n = 1; n < $2; n++)); do
    words+=("$(cli_pos "$1" "$n" | cut -d' ' -f1)")
  done
  bash_complete "$BASH_YCA" _yca "${words[@]}" ''
}

same "yca: commands (Zsh)" "$cli_cmds" "$zsh_cmds"
same "yca: commands (Bash)" "$cli_cmds" "$(bash_complete "$BASH_YCA" _yca yca '')"
same "yca: global options (Zsh)" "$cli_globals" "$zsh_globals"
same "yca: global options (Bash)" "$cli_globals" \
  "$(bash_complete "$BASH_YCA" _yca yca - | tr ' ' '\n' | no_help | norm)"

for c in $cli_cmds; do
  # init has no options of its own
  want="$(cli_opts "$c")"
  same "yca $c: options (Zsh)" "$want" "$(zsh_opts "$c")" empty
  same "yca $c: options (Bash)" "$want" "$(bash_opts "$c")" empty
  for n in 1 2; do
    want="$(cli_pos "$c" "$n")"
    [ -n "$want" ] || continue
    same "yca $c: positional $n (Zsh)" "$want" "$(zsh_pos "$c" "$n")"
    same "yca $c: positional $n (Bash)" "$want" "$(bash_pos "$c" "$n")"
  done
done

# --- yca-acme -------------------------------------------------------------

if ! (cd "$ACME_SRC" && go build -tags libsqlite3 -o "$WORK/yca-acme" .); then
  bad "go build yca-acme"
  exit 1
fi
# Run in the scratch dir: nothing may open a ./acme.db elsewhere.
cd "$WORK" || exit 1
ACME="$WORK/yca-acme"

# Go flag prints -name; the completions offer --name.
go_flags() { "$ACME" "$@" -h 2>&1 | grep -oE '^  -[a-z][a-z0-9-]+' | sed 's/^  -/--/' | norm; }
go_verbs() { "$ACME" "$1" 2>&1 | grep -oE "$1 <[a-z|]+>" | sed 's/.*<//; s/>//; s/|/ /g' | norm; }
src_cmds="$(grep -ohE 'os\.Args\[1\] == "[a-z]+"' "$ACME_SRC"/*.go | grep -oE '"[a-z]+"' | tr -d '"' | norm)"

zsh_acme_section() {
  awk -v s="$1" '
    /== ari \]\]/ { cur = "ari" } /== eab \]\]/ { cur = "eab" }
    /'"'"'1:command:/ { cur = "daemon" }
    cur == s { print } /return/ { cur = "" }' "$ZSH_ACME"
}
zsh_acme_flags() { zsh_acme_section "$1" | grep -oE -- "--[a-z][a-z0-9-]+\[" | tr -d '[' | norm; }

same "yca-acme: commands (Zsh)" "$src_cmds" \
  "$(grep -oE "'1:command:\([^)]*\)'" "$ZSH_ACME" | sed 's/.*(//; s/).*//' | norm)"
same "yca-acme: commands (Bash)" "$src_cmds" "$(bash_complete "$BASH_ACME" _yca_acme yca-acme '')"
same "yca-acme: daemon options (Zsh)" "$(go_flags)" "$(zsh_acme_flags daemon)"
same "yca-acme: daemon options (Bash)" "$(go_flags)" "$(bash_complete "$BASH_ACME" _yca_acme yca-acme -)"

# `new` and `list` stop at -h before opening the state db (`eab delete`
# would take -h for the kid).
for s in "eab new" "ari list"; do
  sub=${s%% *} verb=${s#* }
  want="$(go_flags "$sub" "$verb")"
  same "yca-acme $sub: options (Zsh)" "$want" "$(zsh_acme_flags "$sub")"
  same "yca-acme $sub: options (Bash)" "$want" \
    "$(bash_complete "$BASH_ACME" _yca_acme yca-acme "$sub" "$verb" -)"
  want="$(go_verbs "$sub")"
  same "yca-acme $sub: verbs (Zsh)" "$want" \
    "$(zsh_acme_section "$sub" | grep -oE "'2:verb:\([^)]*\)'" | sed 's/.*(//; s/).*//' | norm)"
  same "yca-acme $sub: verbs (Bash)" "$want" \
    "$(bash_complete "$BASH_ACME" _yca_acme yca-acme "$sub" '')"
done

# --- Bash option values ---------------------------------------------------

same "Bash: yca revoke --reason" \
  "$(tr ' ' '\n' < "$ZSH_YCA" | sed -n '/reasons=(/,/)/p' | grep -oE '^[a-zA-Z]+$' | norm)" \
  "$(bash_complete "$BASH_YCA" _yca yca revoke server --reason '')"
same "Bash: yca get --encoding" "der pem" \
  "$(bash_complete "$BASH_YCA" _yca yca get server --encoding '')"
same "Bash: yca get --cn" "- root-ca signing-ca" \
  "$(bash_complete "$BASH_YCA" _yca yca get ca --cn '')"
same "Bash: global option value skipped" "create" \
  "$(bash_complete "$BASH_YCA" _yca yca --config y.toml cr)"
same "Bash: option value is not a positional" "" \
  "$(bash_complete "$BASH_YCA" _yca yca sign server --id x '')" empty

exit "$fail"

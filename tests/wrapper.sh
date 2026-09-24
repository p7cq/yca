#!/bin/sh
#
# Tests the operator wrapper's dispatch (share/bin/yca.in) with stubbed
# uname/id/sudo/systemd-run and a stub real binary: every stub prints its
# name and arguments, one per line, so each case asserts the exact command
# the wrapper would exec.
#
# Usage: wrapper.sh <configured wrapper>
set -eu

wrapper="${1:?usage: wrapper.sh <configured wrapper>}"
tmp="$(mktemp -d)"
trap 'rm -rf "$tmp"' EXIT
fail=0

# configure_file must have substituted the libexec path.
if ! grep -q '^real=".*/libexec/yca/yca"$' "$wrapper"; then
    echo "FAIL wrapper: real binary path not configured"
    exit 1
fi

mkdir "$tmp/bin"
for s in sudo systemd-run real; do
    printf '#!/bin/sh\nprintf "%%s\\n" "%s" "$@"\n' "$s" > "$tmp/bin/$s"
done
printf '#!/bin/sh\necho "$STUB_OS"\n' > "$tmp/bin/uname"
cat > "$tmp/bin/id" <<'EOF'
#!/bin/sh
case "$1" in
-un) echo "$STUB_USER" ;;
-u) echo "$STUB_UID" ;;
esac
EOF
chmod 755 "$tmp/bin/"*
sed "s|^real=.*|real=\"$tmp/bin/real\"|" "$wrapper" > "$tmp/yca"

run() {
    # run <os> <user> <uid> <args...>
    os=$1 user=$2 uid=$3
    shift 3
    STUB_OS=$os STUB_USER=$user STUB_UID=$uid PATH="$tmp/bin:$PATH" \
        sh "$tmp/yca" "$@" < /dev/null
}

expect() {
    # expect <name> <got> <want lines...>
    name=$1 got=$2
    shift 2
    want="$(printf '%s\n' "$@")"
    if [ "$got" = "$want" ]; then
        echo "ok   $name"
    else
        echo "FAIL $name"
        echo "  got:"
        printf '%s\n' "$got" | sed 's/^/    /'
        echo "  want:"
        printf '%s\n' "$want" | sed 's/^/    /'
        fail=1
    fi
}

linux_run() {
    # the systemd-run command line, after the caller-dependent prefix
    printf '%s\n' systemd-run --quiet --wait --collect --pipe \
        --uid=yca --gid=yca --working-directory=/var/lib/yca \
        -p UMask=0077 -p EnvironmentFile=-/etc/yca/yca.env \
        -p NoNewPrivileges=yes -p ProtectSystem=strict \
        -p ReadWritePaths=/var/lib/yca "$tmp/bin/real"
}

expect "as yca: exec real, arguments untouched" \
    "$(run Linux yca 990 list --limit 5)" \
    real list --limit 5

expect "--version: exec real, no privileges" \
    "$(run Linux paul 2000 --version)" \
    real --version

# shellcheck disable=SC2046 # one word per line by construction
expect "operator: sudo systemd-run with packaged defaults" \
    "$(run Linux paul 2000 list --limit 5)" \
    sudo $(linux_run) --config /etc/yca/yca.toml \
    --store /var/lib/yca/store list --limit 5

# shellcheck disable=SC2046
expect "root: systemd-run without sudo" \
    "$(run Linux root 0 list)" \
    $(linux_run) --config /etc/yca/yca.toml --store /var/lib/yca/store list

# shellcheck disable=SC2046
expect "explicit --config/--store=: no defaults added" \
    "$(run Linux paul 2000 --config c.toml --store=/tmp/s list)" \
    sudo $(linux_run) --config c.toml --store=/tmp/s list

# FreeBSD: sudo /bin/sh -c <script> sh <env> <state> <real> <args...>;
# the script itself spans several lines, so assert around it.
bsd="$(run FreeBSD paul 2000 list)"
expect "FreeBSD: sudo /bin/sh -c" \
    "$(printf '%s\n' "$bsd" | head -n 3)" \
    sudo /bin/sh -c
expect "FreeBSD: script arguments and defaults" \
    "$(printf '%s\n' "$bsd" | tail -n 9)" \
    sh /usr/local/etc/yca/yca.env /var/db/yca "$tmp/bin/real" \
    --config /usr/local/etc/yca/yca.toml --store /var/db/yca/store list
case "$bsd" in
*"su -m yca -c"*) echo "ok   FreeBSD: switches with su -m yca" ;;
*)
    echo "FAIL FreeBSD: no su -m yca in the script"
    fail=1
    ;;
esac

exit "$fail"

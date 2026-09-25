#!/bin/sh
#
# Tests the yca-acme operator wrapper's dispatch (share/bin/yca-acme.in)
# with stubbed uname/id/sudo/systemd-run and a stub real binary: every stub
# prints its name and arguments, one per line, so each case asserts the
# exact command the wrapper would exec.
#
# Usage: wrapper-acme.sh <configured wrapper>
set -eu

wrapper="${1:?usage: wrapper-acme.sh <configured wrapper>}"
unset CA_STORE_PASSPHRASE CA_HSM_PIN CA_HSM_ROOT_PIN
tmp="$(mktemp -d)"
trap 'rm -rf "$tmp"' EXIT
fail=0

# configure_file must have substituted the libexec path.
if ! grep -q '^real=".*/libexec/yca/yca-acme"$' "$wrapper"; then
    echo "FAIL wrapper-acme: real binary path not configured"
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
sed "s|^real=.*|real=\"$tmp/bin/real\"|" "$wrapper" > "$tmp/yca-acme"

run() {
    # run <os> <user> <uid> <args...>
    os=$1 user=$2 uid=$3
    shift 3
    STUB_OS=$os STUB_USER=$user STUB_UID=$uid PATH="$tmp/bin:$PATH" \
        sh "$tmp/yca-acme" "$@" < /dev/null
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
        -p UMask=0077 -p NoNewPrivileges=yes -p ProtectSystem=strict \
        -p ReadWritePaths=/var/lib/yca "$tmp/bin/real"
}

expect "as yca: exec real, arguments untouched" \
    "$(run Linux yca 990 eab list)" \
    real eab list

expect "--version: exec real, no privileges" \
    "$(run Linux paul 2000 --version)" \
    real --version

expect "daemon flags: exec real as the caller" \
    "$(run Linux paul 2000 --listen 127.0.0.1:8555 --state db)" \
    real --listen 127.0.0.1:8555 --state db

expect "subcommand without verb: exec real for its usage" \
    "$(run Linux paul 2000 eab)" \
    real eab

# shellcheck disable=SC2046 # one word per line by construction
expect "operator eab list: sudo systemd-run, default state after the verb" \
    "$(run Linux paul 2000 eab list)" \
    sudo $(linux_run) eab list --state /var/lib/yca/acme.db

# shellcheck disable=SC2046
expect "eab delete: kid stays last" \
    "$(run Linux paul 2000 eab delete kid-1)" \
    sudo $(linux_run) eab delete --state /var/lib/yca/acme.db kid-1

# shellcheck disable=SC2046
expect "ari accelerate: flags kept after the default" \
    "$(run Linux paul 2000 ari accelerate --issuer 'CA E1')" \
    sudo $(linux_run) ari accelerate --state /var/lib/yca/acme.db \
    --issuer 'CA E1'

# shellcheck disable=SC2046
expect "explicit --state: no default added" \
    "$(run Linux paul 2000 eab list --state /tmp/a.db)" \
    sudo $(linux_run) eab list --state /tmp/a.db

# shellcheck disable=SC2046
expect "explicit -state=: no default added" \
    "$(run Linux paul 2000 ari list -state=/tmp/a.db)" \
    sudo $(linux_run) ari list -state=/tmp/a.db

# shellcheck disable=SC2046
expect "root: systemd-run without sudo" \
    "$(run Linux root 0 eab list)" \
    $(linux_run) eab list --state /var/lib/yca/acme.db

# CA secrets are not needed by eab/ari: never preserved or forwarded.
# shellcheck disable=SC2046
expect "exported CA secrets: not passed on" \
    "$(export CA_STORE_PASSPHRASE=p CA_HSM_PIN=q && run Linux paul 2000 eab list)" \
    sudo $(linux_run) eab list --state /var/lib/yca/acme.db

# FreeBSD: sudo /bin/sh -c <script> sh <state> <real> <args...>;
# the script itself spans several lines, so assert around it.
bsd="$(run FreeBSD paul 2000 eab list)"
expect "FreeBSD: sudo /bin/sh -c" \
    "$(printf '%s\n' "$bsd" | head -n 3)" \
    sudo /bin/sh -c
expect "FreeBSD: script arguments and default state" \
    "$(printf '%s\n' "$bsd" | tail -n 7)" \
    sh /var/db/yca "$tmp/bin/real" eab list --state /var/db/yca/acme.db
case "$bsd" in
*"su -m yca -c"*) echo "ok   FreeBSD: switches with su -m yca" ;;
*)
    echo "FAIL FreeBSD: no su -m yca in the script"
    fail=1
    ;;
esac
case "$bsd" in
*yca.env*)
    echo "FAIL FreeBSD: reads the CA env file"
    fail=1
    ;;
*) echo "ok   FreeBSD: no CA env file" ;;
esac

exit "$fail"

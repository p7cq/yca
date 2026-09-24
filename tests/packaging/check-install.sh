#!/bin/sh
#
# Asserts the service account and ownership a freshly installed yca package
# must provide. Run as root right after installing the package.
#
# Usage: check-install.sh linux|freebsd
set -eu

fail=0

check() {
    # check <path> <owner> <group> <mode>
    if [ "$os" = freebsd ]; then
        got="$(stat -f '%Su %Sg %Lp' "$1")"
    else
        got="$(stat -c '%U %G %a' "$1")"
    fi
    if [ "$got" = "$2 $3 $4" ]; then
        echo "ok   $1: $got"
    else
        echo "FAIL $1: got '$got', want '$2 $3 $4'"
        fail=1
    fi
}

os="${1:?usage: check-install.sh linux|freebsd}"

case "$os" in
linux)
    etc=/etc/yca
    state=/var/lib/yca
    ;;
freebsd)
    etc=/usr/local/etc/yca
    state=/var/db/yca
    ;;
*)
    echo "unknown os: $os" >&2
    exit 2
    ;;
esac

entry="$(getent passwd yca)" || {
    echo "FAIL no yca user"
    exit 1
}
shell="${entry##*:}"
case "$shell" in
*/nologin) echo "ok   yca shell: $shell" ;;
*)
    echo "FAIL yca shell: $shell"
    fail=1
    ;;
esac

# Locked or empty-forbidden password field: '!', '!*', '*' (Linux shadow)
# or '*' (FreeBSD master.passwd).
if [ "$os" = freebsd ]; then
    pw="$(pw usershow yca | cut -d: -f2)"
else
    pw="$(getent shadow yca | cut -d: -f2)"
fi
case "$pw" in
'!'* | '*'*) echo "ok   yca password: locked" ;;
*)
    echo "FAIL yca password field: '$pw'"
    fail=1
    ;;
esac

check "$etc" root yca 750
check "$etc/yca.toml" root yca 640
check "$state" yca yca 700
check /srv/yca yca yca 755

exit "$fail"

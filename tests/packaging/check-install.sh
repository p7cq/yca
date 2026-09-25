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
    prefix=/usr
    wheel=root
    ;;
freebsd)
    etc=/usr/local/etc/yca
    state=/var/db/yca
    prefix=/usr/local
    wheel=wheel
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
check /srv/yca root "$wheel" 755
check /srv/yca/pub yca yca 755
check /srv/yca/webroot root "$wheel" 755

# yca-publish (the FreeBSD crontab sample) needs rsync: a package dependency.
if [ -x "$prefix/bin/rsync" ]; then
    echo "ok   $prefix/bin/rsync: present"
else
    echo "FAIL $prefix/bin/rsync: missing (dependency not pulled in)"
    fail=1
fi

# The real CLI and yca-acme live in libexec; on PATH are the operator
# wrappers.
for b in yca yca-acme; do
    if [ -x "$prefix/libexec/yca/$b" ]; then
        echo "ok   $prefix/libexec/yca/$b: executable"
    else
        echo "FAIL $prefix/libexec/yca/$b: missing or not executable"
        fail=1
    fi
    # Any sh shebang: Fedora's brp-mangle-shebangs rewrites /bin/sh to
    # /usr/bin/sh.
    shebang="$(head -n 1 "$prefix/bin/$b")"
    if [ "${shebang#\#!}" != "$shebang" ] && [ "${shebang%/sh}" != "$shebang" ] &&
        grep -q "^real=\"$prefix/libexec/yca/$b\"$" "$prefix/bin/$b"; then
        echo "ok   $prefix/bin/$b: wrapper for $prefix/libexec/yca/$b"
    else
        echo "FAIL $prefix/bin/$b: not the wrapper for $prefix/libexec/yca/$b"
        fail=1
    fi
done

exit "$fail"

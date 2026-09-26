# Maintainer: p7cq <707c71@gmail.com>
#
# Builds from the local checkout (source=()), not a downloaded tarball: CI
# runs makepkg directly against a git checkout.
pkgname=yca
pkgver=0.7.10
pkgrel=1
pkgdesc='Two-tier ECDSA certificate authority CLI with an ACME frontend'
arch=('x86_64')
url='https://github.com/p7cq/yca'
license=('Apache-2.0')
depends=('sqlite' 'libc++' 'libc++abi' 'rsync')
# bind-tools (nsupdate) is only needed if the ACME client run against
# repository_host validates via DNS-01 through dynamic DNS (e.g. acme.sh's
# dns_nsupdate hook).
optdepends=('bind-tools: acme.sh dns_nsupdate hook for DNS-01 challenge'
            'bash-completion: tab completion for yca and yca-acme in Bash')
makedepends=('clang' 'cmake' 'ninja' 'go')
# Default config, preserved across upgrades.
backup=('etc/yca/yca.toml')
options=('!lto' '!debug')
source=()

pkgver() {
  cd "$startdir"
  cat VERSION
}

build() {
  cd "$startdir"

  # --- C++ CLI: Release, clang + libc++, vendored Botan
  cmake -B build -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=/usr \
    -DCMAKE_C_COMPILER=clang \
    -DCMAKE_CXX_COMPILER=clang++ \
    -Wno-dev
  cmake --build build --target yca

  # --- Go ACME frontend (yca-acme); go-sqlite3 is CGO, gcc from base-devel
  export GOTOOLCHAIN=local CGO_ENABLED=1
  export GOFLAGS='-buildmode=pie -mod=readonly -modcacherw -trimpath'
  ( cd acme && go build -tags libsqlite3 \
      -ldflags "-X main.version=$pkgver -linkmode=external" -o ../bin/yca-acme . )
}

package() {
  cd "$startdir"

  # yca binary + completion + yca.1 man page
  DESTDIR="$pkgdir" cmake --install build

  install -Dm755 bin/yca-acme "$pkgdir/usr/libexec/yca/yca-acme"
  install -Dm644 share/man/yca-acme.1 "$pkgdir/usr/share/man/man1/yca-acme.1"
  install -Dm644 share/zsh-completion/_yca-acme \
    "$pkgdir/usr/share/zsh/site-functions/_yca-acme"
  install -Dm644 share/bash-completion/yca-acme \
    "$pkgdir/usr/share/bash-completion/completions/yca-acme"

  install -Dm600 yca.toml "$pkgdir/etc/yca/yca.toml"

  install -Dm644 share/sysusers.d/yca.conf "$pkgdir/usr/lib/sysusers.d/yca.conf"
  install -Dm644 share/tmpfiles.d/yca.conf "$pkgdir/usr/lib/tmpfiles.d/yca.conf"

  # Packaged systemd units are installed but not enabled.
  install -dm755 "$pkgdir/usr/lib/systemd/system"
  install -m644 share/systemd/*.service share/systemd/*.timer \
    "$pkgdir/usr/lib/systemd/system/"

  install -Dm644 share/nginx/yca.conf "$pkgdir/usr/share/$pkgname/nginx/yca.conf"
  install -Dm644 LICENSE "$pkgdir/usr/share/licenses/$pkgname/LICENSE"
}

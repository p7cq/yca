# Maintainer: p7cq <707c71@gmail.com>
#
# Builds from the local checkout (source=()), not a downloaded tarball: CI
# runs makepkg directly against a git checkout. pkgver() below keeps
# pkgver in sync with VERSION dynamically - no sed step needed, and a
# local `makepkg` gets the right version too, not just CI's.
pkgname=yca
pkgver=0.7.10
pkgrel=1
pkgdesc='Two-tier ECDSA certificate authority CLI with an ACME frontend'
# aarch64 not built here: Arch Linux itself has no aarch64 repos (that's
# Arch Linux ARM, a separate project/toolchain) - CI only builds x86_64.
arch=('x86_64')
url='https://github.com/p7cq/yca'
license=('Apache-2.0')
# yca (C++) links libsqlite3 + libc++/libc++abi dynamically; Botan is vendored
# and linked statically, so it is not a runtime dependency. yca-acme (Go) bundles
# its sqlite driver via CGO.
depends=('sqlite' 'libc++' 'libc++abi' 'rsync')
# bind-tools (nsupdate) is only needed if the ACME client run against
# repository_host validates via DNS-01 through dynamic DNS (e.g. acme.sh's
# dns_nsupdate hook).
optdepends=('bind-tools: acme.sh dns_nsupdate hook for repository_host DNS-01')
makedepends=('clang' 'cmake' 'ninja' 'go')
backup=('etc/yca/yca.toml')
options=('!lto' '!debug')
source=()

pkgver() {
  cd "$startdir"
  cat VERSION
}

build() {
  cd "$startdir"

  # --- C++ CLI: Release, clang + libc++, vendored Botan (per-arch amalgamation)
  cmake -B build -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=/usr \
    -DCMAKE_C_COMPILER=clang \
    -DCMAKE_CXX_COMPILER=clang++ \
    -Wno-dev
  # Only the shipped CLI; yca-seed and yca_tests are dev/test tools.
  cmake --build build --target yca

  # --- Go ACME frontend (yca-acme); go-sqlite3 is CGO, so gcc from base-devel
  export GOTOOLCHAIN=local CGO_ENABLED=1
  export GOFLAGS='-buildmode=pie -mod=readonly -modcacherw -trimpath'
  ( cd acme && go build -ldflags "-X main.version=$pkgver -linkmode=external" \
      -o ../bin/yca-acme . )
}

package() {
  cd "$startdir"

  # yca binary + _yca zsh completion + yca.1 man page (CMake install rules)
  DESTDIR="$pkgdir" cmake --install build

  # ACME frontend + its man page and completion (not covered by CMake)
  install -Dm755 bin/yca-acme "$pkgdir/usr/bin/yca-acme"
  install -Dm644 share/man/yca-acme.1 "$pkgdir/usr/share/man/man1/yca-acme.1"
  install -Dm644 share/zsh-completion/_yca-acme \
    "$pkgdir/usr/share/zsh/site-functions/_yca-acme"

  # Default config, marked backup so pacman preserves operator edits.
  install -Dm600 yca.toml "$pkgdir/etc/yca/yca.toml"

  # Service account and directories. pacman's systemd hooks run
  # systemd-sysusers and systemd-tmpfiles on install, and the tmpfiles
  # entries set yca.toml to root:yca 0640 (the package cannot name the yca
  # group before the account exists).
  install -Dm644 share/sysusers.d/yca.conf "$pkgdir/usr/lib/sysusers.d/yca.conf"
  install -Dm644 share/tmpfiles.d/yca.conf "$pkgdir/usr/lib/tmpfiles.d/yca.conf"

  # Packaged systemd units live under /usr/lib; they are installed but NOT
  # enabled: a CA rollout is a manual ceremony (see docs/acme-operation.md).
  install -dm755 "$pkgdir/usr/lib/systemd/system"
  install -m644 share/systemd/*.service share/systemd/*.timer \
    "$pkgdir/usr/lib/systemd/system/"

  # Example nginx snippet + license.
  install -Dm644 share/nginx/yca.conf "$pkgdir/usr/share/$pkgname/nginx/yca.conf"
  install -Dm644 LICENSE "$pkgdir/usr/share/licenses/$pkgname/LICENSE"
}

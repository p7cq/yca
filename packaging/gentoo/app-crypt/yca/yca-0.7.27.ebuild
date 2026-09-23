# Copyright 2026 p7cq
# Distributed under the terms of the Apache License, Version 2.0

EAPI=8

inherit cmake systemd

DESCRIPTION="Two-tier ECDSA certificate authority CLI with an ACME frontend"
HOMEPAGE="https://github.com/p7cq/yca"
SRC_URI="https://github.com/p7cq/yca/archive/refs/tags/v${PV}.tar.gz -> ${P}.tar.gz"
S="${WORKDIR}/${PN}-${PV}"

LICENSE="Apache-2.0"
SLOT="0"
KEYWORDS="~amd64 ~arm64"

BDEPEND="
	llvm-core/clang
	dev-build/ninja
	dev-lang/go
"
RDEPEND="
	llvm-runtimes/libcxx
	llvm-runtimes/libcxxabi
	dev-db/sqlite
"
DEPEND="${RDEPEND}"

src_configure() {
	export CC=clang CXX=clang++
	local mycmakeargs=(
		-DCMAKE_CXX_COMPILER=clang++
	)
	cmake_src_configure
}

src_compile() {
	# Only the shipped CLI; yca-seed/yca_tests are dev/test tools.
	cmake_build yca

	# yca-acme (Go); go-sqlite3 needs CGO, clang is already the active CC.
	pushd acme > /dev/null || die
	go build -ldflags "-X main.version=${PV}" -o ../bin/yca-acme . ||
		die "go build failed"
	popd > /dev/null || die
}

src_install() {
	# CMake's own install() rules: yca binary, zsh completion, man page.
	cmake_src_install

	dobin bin/yca-acme
	doman share/man/yca-acme.1

	insinto /usr/share/zsh/site-functions
	doins share/zsh-completion/_yca-acme

	insinto /etc/yca
	doins yca.toml
	fperms 600 /etc/yca/yca.toml

	# Installed but not enabled: a CA rollout is a manual ceremony (see
	# the unit headers and docs/install.md).
	systemd_dounit share/systemd/yca-publish.service
	systemd_dounit share/systemd/yca-publish.timer
	systemd_dounit share/systemd/yca-crl-refresh.service
	systemd_dounit share/systemd/yca-crl-refresh.timer
	systemd_dounit share/systemd/yca-root-crl-refresh.service
	systemd_dounit share/systemd/yca-root-crl-refresh.timer
	systemd_dounit share/systemd/yca-acme.service
	systemd_dounit share/systemd/yca-acme-renew.service
	systemd_dounit share/systemd/yca-acme-renew.timer

	insinto /usr/share/doc/${PF}/examples
	doins share/nginx/yca.conf
}

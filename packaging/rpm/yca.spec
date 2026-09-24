# Version is passed on the command line (rpmbuild --define "version X"), not
# hardcoded here, so the spec never needs a manual edit on a VERSION bump.
# yca ships stripped (Release build, no .debug_info; see CMakeLists.txt) and
# yca-acme's Go debug info isn't meaningful packaged alone, so skip the
# auto-generated debuginfo/debugsource subpackages entirely.
%global debug_package %{nil}
Name:           yca
Version:        %{version}
Release:        1%{?dist}
Summary:        Two-tier ECDSA certificate authority CLI with an ACME frontend

License:        Apache-2.0
URL:            https://github.com/p7cq/yca
Source0:        %{name}-%{version}.tar.gz

BuildRequires:  clang cmake ninja-build sqlite-devel libcxx-devel libcxxabi-devel golang pkgconf-pkg-config systemd-rpm-macros
Requires:       sqlite-libs libcxx libcxxabi rsync
# Only needed if the ACME client used against repository_host validates via
# DNS-01 through dynamic DNS (e.g. acme.sh's dns_nsupdate hook).
Suggests:       bind-utils

%description
yca is a command-line certificate authority tool (ECDSA, two-tier
root/intermediate hierarchy) with yca-acme, an ACME frontend for
automated certificate issuance.

%prep
%setup -q

%build
export CC=clang CXX=clang++
# Fedora's rpmbuild auto-exports CFLAGS/CXXFLAGS carrying gcc-only
# -specs=.../redhat-hardened-cc1 (annobin) flags, which CMake would pick up
# from the environment; clang rejects them as unused arguments, which
# CMakeLists.txt's -Werror then turns into a build failure. The project sets
# its own flags explicitly, so nothing from rpm's default optflags is wanted.
unset CFLAGS CXXFLAGS FFLAGS FCFLAGS LDFLAGS
cmake -B build -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_INSTALL_PREFIX=%{_prefix} \
  -DCMAKE_CXX_COMPILER=clang++ \
  -Wno-dev
cmake --build build --target yca -j %{?_smp_mflags}

pushd acme
go build -ldflags "-X main.version=%{version} -linkmode=external" -o ../bin/yca-acme .
popd

%install
rm -rf %{buildroot}
DESTDIR=%{buildroot} cmake --install build
install -Dm755 bin/yca-acme %{buildroot}%{_bindir}/yca-acme
install -Dm644 share/man/yca-acme.1 %{buildroot}%{_mandir}/man1/yca-acme.1
install -Dm644 share/zsh-completion/_yca-acme %{buildroot}%{_datadir}/zsh/site-functions/_yca-acme
install -Dm640 yca.toml %{buildroot}%{_sysconfdir}/yca/yca.toml
install -Dm644 share/sysusers.d/yca.conf %{buildroot}%{_sysusersdir}/yca.conf
install -Dm644 share/tmpfiles.d/yca.conf %{buildroot}%{_tmpfilesdir}/yca.conf
install -dm700 %{buildroot}%{_sharedstatedir}/yca
install -dm755 %{buildroot}/srv/yca/pub %{buildroot}/srv/yca/webroot
install -dm755 %{buildroot}%{_unitdir}
install -m644 share/systemd/*.service share/systemd/*.timer %{buildroot}%{_unitdir}/
install -Dm644 share/nginx/yca.conf %{buildroot}%{_docdir}/%{name}/examples/nginx/yca.conf

# rpm creates the yca account from the packaged sysusers.d file before
# installing files, so the yca owners in the file list resolve at install time.
%post
%tmpfiles_create %{_tmpfilesdir}/yca.conf

%files
%{_bindir}/yca
%{_bindir}/yca-acme
%{_libexecdir}/yca/yca
%{_mandir}/man1/yca.1*
%{_mandir}/man1/yca-acme.1*
%{_datadir}/zsh/site-functions/_yca
%{_datadir}/zsh/site-functions/_yca-acme
%dir %attr(0750,root,yca) %{_sysconfdir}/yca
%config(noreplace) %attr(0640,root,yca) %{_sysconfdir}/yca/yca.toml
%dir %attr(0700,yca,yca) %{_sharedstatedir}/yca
%dir %attr(0755,root,root) /srv/yca
%dir %attr(0755,yca,yca) /srv/yca/pub
%dir %attr(0755,root,root) /srv/yca/webroot
%{_sysusersdir}/yca.conf
%{_tmpfilesdir}/yca.conf
%{_unitdir}/*
%doc %{_docdir}/%{name}/examples/nginx/yca.conf
%license LICENSE

%changelog
* Tue Sep 22 2026 p7cq <707c71@gmail.com> - %{version}-1
- Automated packaging build.

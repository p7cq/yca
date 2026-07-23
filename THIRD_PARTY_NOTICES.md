# Third-party notices

yca is licensed under the Apache License 2.0 (see [LICENSE](LICENSE)). It
bundles or depends on the following third-party components, each under its
own license. The vendored single-header libraries under `third_party/`
retain their full copyright and license notices in the file headers.

## System libraries

| Component | License | Notes |
|-----------|---------|-------|
| [SQLite](https://sqlite.org) | Public domain | linked from the system by the `yca` CLI (Botan's SQLite certificate store) |

## Vendored (`third_party/`)

| Component | Version | License | Copyright |
|-----------|---------|---------|-----------|
| [Botan](https://botan.randombit.net) (amalgamation) | 3.12.0 | BSD-2-Clause (Simplified BSD) | (c) 1999-2023 The Botan Authors |
| [PKCS #11 header](https://github.com/latchset/pkcs11-headers) (`botan/pkcs11.h`) | v3.2 draft | Public domain | Simo Sorce |
| [CLI11](https://github.com/CLIUtils/CLI11) | 2.4.2 | BSD-3-Clause | (c) 2017-2024 University of Cincinnati / Henry Schreiner |
| [doctest](https://github.com/doctest/doctest) | 2.4.11 | MIT | (c) 2016-2023 Viktor Kirilov |
| [toml++](https://github.com/marzer/tomlplusplus) | 3.4.0 | MIT | (c) Mark Gillard |

## Go modules (`acme/go.mod`)

| Component | Version | License | Copyright |
|-----------|---------|---------|-----------|
| [go-sqlite3](https://github.com/mattn/go-sqlite3) | 1.14.47 | MIT | (c) Yasuhiro Matsumoto |
| [go-jose](https://github.com/go-jose/go-jose) | 4.1.3 | Apache-2.0 License | (c) go-jose |

go-sqlite3 embeds the [SQLite](https://sqlite.org) library, which is public
domain.

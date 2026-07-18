#pragma once

// Internal helpers shared between ca.cpp and the load-test seeder (seed.cpp).
// Not part of the public ca:: API.

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <optional>
#include <string>

#include <botan/botan_all.h>

#include "config.h"

namespace ca::detail {

// Store path: <store_dir>/<app::store_file> - a fixed name.
std::filesystem::path store_path(const std::filesystem::path &store_dir);

// Opens a store connection with the store contract's bounded lock waits
// (busy_timeout ≥ 5000 ms): even pure readers can hit SQLITE_BUSY
// for a moment - e.g. the exclusive lock of the WAL checkpoint a closing
// writer runs on exit - so every connection waits instead of failing instantly.
std::shared_ptr<Botan::Sqlite3_Database>
open_store(const std::filesystem::path &db);

// Loads the signing CA certificate from its published PEM (O(1)); avoids an
// O(N) scan of the store to find it on every issuance/revocation.
std::optional<Botan::X509_Certificate>
load_signing_cert(const cfg::Config &config,
                  const std::filesystem::path &store_dir);

// cert_index: queryable/indexed metadata table (Botan's store keeps the DER
// blobs but is not indexed on the fields we filter/sort by).
void ensure_cert_index(Botan::SQL_Database &db);

// Adds/updates a cert's metadata row. status "active" or "revoked".
void index_cert(Botan::SQL_Database &db, const Botan::X509_Certificate &c,
                const std::string &kind, const std::string &status = "active",
                std::size_t revoked_at = 0);

// Writes the PEM encoding atomically (temp sibling + rename, final mode 0400):
// a reader of the published CRL never observes a partial file, and rewrites
// replace the read-only artifact instead of truncating it.
bool write_pem(const std::filesystem::path &path,
               const Botan::X509_Object &obj);

// Same atomic contract, DER encoding (the published .crt / .crl artifacts).
bool write_der(const std::filesystem::path &path,
               const Botan::X509_Object &obj);

std::size_t now_epoch();

// True when a certificate issued now with `validity` would expire after the
// issuer does - and says so on the error channel. Past the issuer's notAfter
// the whole chain fails validation (RFC 5280), so the leaf's tail would be
// dead weight: issuance must stop. The locked config only guarantees
// ee|ocsp_valid_days < signing_ca_valid_days at init time; this is the
// runtime guard against the signing CA aging past the difference.
bool outlives_issuer(const Botan::X509_Certificate &issuer,
                     std::chrono::seconds validity);

// CRL nextUpdate offset from now, in seconds: app::crl_next_update_days,
// clamped so the promise never extends past the issuer's own notAfter (a CA
// cannot promise a publication after its own expiry). One-hour floor keeps
// the field sane even on an already-expired issuer - revocation stays legal.
uint32_t crl_next_update(const Botan::X509_Certificate &issuer);

} // namespace ca::detail

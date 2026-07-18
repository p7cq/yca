#pragma once

#include <chrono>
#include <filesystem>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "config.h"

namespace ca {

enum class Profile { Server, Client };

struct San {
  enum class Type { Dns, Email, Ip } type;
  std::string value;
};

// True when the store's cert_index holds active root and signing anchors
// whose CNs match the locked config (ca_config) recorded in the store itself.
// An absent, unreadable, or inconsistent store is not initialized.
bool is_initialized(const std::filesystem::path &store_dir);

// Initializes the PKI in `store_dir`: creates root + signing CA and records
// the PKI identity + locked config snapshot. Backend "internal": keys are
// generated in software and persisted encrypted into the SQLite store.
// Backend "pkcs11": keys live on the token (existing keypairs labeled with
// the CA slugs are adopted, missing ones are generated on-token).
// Fails if already initialized, or if `store_dir` exists and is not an empty
// directory.
//
// `secret` is the CA secret from the environment: the store passphrase
// (app::passphrase_env; if empty a strong one is generated and shown once),
// or the token user PIN (app::pin_env, required) with the pkcs11 backend.
//
// Returns false on any fatal error.
bool init(const cfg::Config &config, const std::filesystem::path &store_dir,
          std::string_view secret);

// Loads the effective config from the DB (ca_config snapshot) - the source of
// truth after init. Returns nullopt if not initialized.
std::optional<cfg::Config> load_config(const std::filesystem::path &store_dir);

// Reconciles yca.toml (`file`) against the effective DB config (`eff`, from
// load_config), called before issuance: readonly-field changes are warned and
// ignored; valid ee/ocsp validity changes are written to the DB and applied to
// `eff`.
void reconcile(const cfg::Config &file, cfg::Config &eff,
               const std::filesystem::path &store_dir);

// Issues an end-entity certificate (server/client) signed by the signing CA.
// Server always includes DNS:CN plus any extra SANs; client requires at least
// one SAN. Fails if a certificate for the same CN is still active - unless
// that cert is inside the renewal window (less than app::renew_window_pct of
// its lifetime left): then an overlapping successor is issued and the old
// cert is left to expire. The cert is stored and, together with its
// (unencrypted) key, written under <store>/ee/.
//
// `valid_override` (CLI --valid) replaces the ee_valid_days validity
// for this one issuance; sub-day only, [5m, 1d), and not persisted
// anywhere. Day-scale validity always comes from ee_valid_days.
bool issue_ee(
    const cfg::Config &config, const std::filesystem::path &store_dir,
    std::string_view secret, Profile profile, const std::string &cn,
    const std::vector<San> &extra_sans,
    std::optional<std::chrono::seconds> valid_override = std::nullopt);

// Enrolls an identity (e.g. an email) into the store's enrollment table so it
// can later request nonces and CSR signatures. Fails if already enrolled.
bool enroll(const std::filesystem::path &store_dir, const std::string &id);

// Issues (or returns) the enrollment nonce for `id` and writes it to stdout.
// A pending nonce with at least max(1 minute, 20%) of its validity left is
// returned as is; otherwise a fresh one (32 random bytes, lowercase hex) is
// issued with app::max_nonce_validity minutes of validity. No CA secret
// needed: the nonce gates a later `sign`, which requires the secret anyway.
bool get_nonce(const std::filesystem::path &store_dir, const std::string &id);

// Issues an end-entity certificate from an external PKCS#10 CSR, gated by a
// valid (id, nonce) pair. `csr_src` is inline PEM (starts with "-----BEGIN"),
// "-" for stdin, or a file path; PEM/DER are auto-detected and the CSR's
// self-signature (proof-of-possession) is verified on decode.
//
// Only the public key, the subject CN and the supported SAN entries
// (dns/email/ipv4) are taken from the CSR - any other requested extension or
// DN attribute is ignored and the CA dictates the profile (same extensions
// as issue_ee). The key must be ECDSA on the configured ee_curve.
// Profile rules match issue_ee: server needs a hostname CN (DNS:CN is always
// included), client needs at least one supported SAN.
//
// On success the nonce is consumed (same transaction as the insert), the cert
// is stored/indexed like issue_ee - but nothing is written under <store>/ee/
// (the CA never sees the private key) - and the CN is printed to stdout for
// retrieval via `get <profile> --cn`.
bool sign_csr(const cfg::Config &config, const std::filesystem::path &store_dir,
              std::string_view secret, Profile profile, const std::string &id,
              const std::string &nonce, const std::string &csr_src);

// Issues the OCSP responder certificate (CN = config.ocsp_cn, EKU
// id-kp-OCSPSigning + id-pkix-ocsp-nocheck), signed by the signing CA, and
// delivers cert+key under <store>/ocsp/. Fails if an active OCSP cert already
// exists, unless it is inside the renewal window (see issue_ee).
bool issue_ocsp(const cfg::Config &config,
                const std::filesystem::path &store_dir,
                std::string_view secret);

// Revokes the active `target` certificate (server|client|ocsp) for `cn` by
// adding it to the signing CA's CRL (<store>/ca/<signing-slug>.crl). For ocsp,
// `cn` is ignored (config.ocsp_cn is used). `reason` is a CRLReason name.
// A non-empty `serial` (hex, ':' separators tolerated) selects the exact
// certificate instead - during a renewal overlap, by-CN means "the newest
// active" while by-serial is unambiguous (what ACME revokeCert needs).
// Fails if no matching active cert exists.
bool revoke(const cfg::Config &config, const std::filesystem::path &store_dir,
            std::string_view secret, const std::string &target,
            const std::string &cn, const std::string &reason,
            const std::string &serial = "");

// Which published CRLs refresh_crl re-signs. Root and Signing exist so the
// two CRLs can run on separate cadences (app::root_crl_next_update_days vs
// app::crl_next_update_days) - a signing-scope run never loads the root key.
enum class CrlScope { Root, Signing, All };

// Re-signs the published CRLs selected by `scope` from their current entry
// sets: same revocations, crlNumber+1, fresh thisUpdate/nextUpdate. Run
// each scope on a schedule shorter than its nextUpdate horizon (see
// share/systemd/yca-crl-refresh.* for signing, daily, and
// share/systemd/yca-root-crl-refresh.* for root, quarterly) so relying
// parties never see a stale CRL.
bool refresh_crl(const cfg::Config &config,
                 const std::filesystem::path &store_dir,
                 std::string_view secret, CrlScope scope = CrlScope::All);

// Writes a certificate to stdout. `profile` is "ca", "server", or "client".
// For "ca", `selector` is an alias ("root-ca"/"signing-ca") or the CA CN; for
// server/client it is the EE CN and the active cert of that profile is
// returned. `encoding` is "pem" or "der". Read-only: no passphrase required.
bool get_cert(const cfg::Config &config, const std::filesystem::path &store_dir,
              const std::string &profile, const std::string &selector,
              const std::string &encoding);

// Lists certificates via cert_index. `filter` is one of "expiring", "expired",
// "revoked", "last" (windowed by `days`, ordered) or "cn" (by `cn`, which may
// be a CA alias root-ca/signing-ca). Columns: CN, kind, truncated serial,
// expiry, status. `tsv` for tab-separated output. Read-only; no passphrase.
bool list_certs(const cfg::Config &config,
                const std::filesystem::path &store_dir,
                const std::string &filter, int days, const std::string &cn,
                bool tsv);

} // namespace ca

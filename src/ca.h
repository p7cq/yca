#pragma once

#include <chrono>
#include <cstddef>
#include <filesystem>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "config.h"

namespace ca {

// Profiles are named, not enumerated: the shape each name dictates lives in
// profile.h, and the CA that issues it is whichever one lists it.

// The CA secrets, one per backend/token; which ones an operation needs
// depends on the layout (per-CA key backends, see config.h). `passphrase`
// (app::passphrase_env) unlocks keys on the internal backend, `pin`
// (app::pin_env) the signing token, `root_pin` (app::root_pin_env, with
// the caller applying the fallback to `pin`) the root token. The
// converting constructors serve the single-secret layouts and the tests,
// where one value plays every role.
struct Secrets {
  std::string_view passphrase;
  std::string_view pin;
  std::string_view root_pin;

  Secrets() = default;
  Secrets(std::string_view all) : passphrase(all), pin(all), root_pin(all) {}
  Secrets(const char *all) : Secrets(std::string_view(all)) {}
  Secrets(std::string_view passphrase, std::string_view pin,
          std::string_view root_pin)
      : passphrase(passphrase), pin(pin), root_pin(root_pin) {}
};

struct San {
  enum class Type { Dns, Email, Ip, Uri } type;
  std::string value;
};

// Checks only if this store is initialized per its yca.toml, a historical
// fact about the ceremony - what later became of those anchors (revoked,
// expired, superseded by a newer generation) does not change it. An absent,
// unreadable, or inconsistent store is not initialized.
bool is_initialized(const std::filesystem::path &store_dir);

// Initializes the PKI in `store_dir`: creates root + signing CA and records
// the PKI identity + locked config snapshot. Each CA key follows its
// backend. "internal": generated in software and persisted encrypted into
// the SQLite store. "pkcs11": lives on its token (an existing keypair
// labeled with the CA slug is adopted, a missing one is generated
// on-token). Fails if already initialized, or if `store_dir` exists and is
// not an empty directory.
//
// `secrets` come from the environment: token PINs are required for pkcs11
// keys; the passphrase is required when any key is internal (if empty a
// strong one is generated and shown once).
//
// Returns false on any fatal error.
bool init(const cfg::Config &config, const std::filesystem::path &store_dir,
          const Secrets &secrets);

// Creates an issuing CA that `config` declares but the store does not hold
// yet: generation 1 of `purpose`, signed by the active root, so this is a
// root key ceremony like a rotation. The CA's section is locked into the
// store as it is created, which is what lets a purpose be added to an
// initialized store without re-initializing it. Fails if the store already
// holds that purpose, or if the file does not declare it.
bool add_signing_ca(const cfg::Config &config,
                    const std::filesystem::path &store_dir,
                    const Secrets &secrets, const std::string &purpose);

// Creates the next generation of the issuing CA named by `purpose`: a fresh
// key and that CA's configured profile under a new display name `new_cn`,
// signed by the active root (the ceremony that brings the root key online).
// The successor becomes the active issuer for that purpose alone; the
// incumbent turns `retiring` and keeps publishing its CRL until the last
// certificate it signed expires. Other purposes are untouched. Prints the
// new CN on stdout.
bool renew_signing_ca(const cfg::Config &config,
                      const std::filesystem::path &store_dir,
                      const Secrets &secrets, const std::string &purpose,
                      const std::string &new_cn);

// Loads the effective config from the DB (ca_config snapshot) - the source of
// truth after init. Returns nullopt if not initialized.
std::optional<cfg::Config> load_config(const std::filesystem::path &store_dir);

// Compares yca.toml (`file`) against the effective DB config (`eff`, from
// load_config), called before issuance: the config is fully locked, so any
// changed field is warned and ignored (re-init to change anything).
void reconcile(const cfg::Config &file, const cfg::Config &eff);

// Issues an end-entity certificate of `profile`, signed by the CA that
// lists that profile. The profile dictates the certificate's shape (EKU,
// key usage, subject rules, policy OID, validity ceiling) - see profile.h.
// Refused for a profile no configured CA claims, and for one the CA may
// only sign from a CSR. Fails if a certificate for the same CN is still active
// - unless that cert is inside the renewal window (less than
// app::renew_window_pct of its lifetime left): then an overlapping successor is
// issued and the old cert is left to expire. The cert is stored and, together
// with its (unencrypted) key, written under <store>/ee/.
//
// `valid_override` (CLI --valid) replaces the ee_valid_days validity
// for this one issuance; range [5m, ee_valid_days] - the policy is the
// ceiling, shorter is always allowed - and not persisted anywhere.
bool issue_ee(
    const cfg::Config &config, const std::filesystem::path &store_dir,
    const Secrets &secrets, const std::string &profile, const std::string &cn,
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
// retrieval via `get <profile> --cn`. `valid_override` behaves exactly as in
// issue_ee ([5m, ee_valid_days], one-shot).
bool sign_csr(
    const cfg::Config &config, const std::filesystem::path &store_dir,
    const Secrets &secrets, const std::string &profile, const std::string &id,
    const std::string &nonce, const std::string &csr_src,
    std::optional<std::chrono::seconds> valid_override = std::nullopt);

// Revokes the active `target` certificate (server|client) for `cn` by
// adding it to the signing CA's CRL (<store>/ca/<signing-slug>.crl).
// `reason` is a CRLReason name.
// A non-empty `serial` (hex, ':' separators tolerated) selects the exact
// certificate instead - during a renewal overlap, by-CN means "the newest
// active" while by-serial is unambiguous (what ACME revokeCert needs).
// Fails if no matching active cert exists.
bool revoke(const cfg::Config &config, const std::filesystem::path &store_dir,
            const Secrets &secrets, const std::string &target,
            const std::string &cn, const std::string &reason,
            const std::string &serial = "");

// Revokes a signing CA generation by putting it on the ROOT CRL.
// `selector` is a generation's CN or the alias signing-ca. Refused for
// the root (a trust anchor is dropped by relying parties, not revoked) and
// for the active issuer, which must be replaced by `renew_signing_ca` first
// so issuance never has a gap. The updated root CRL carries a fresh nextUpdate,
// so only publication is left to do.
bool revoke_ca(const cfg::Config &config,
               const std::filesystem::path &store_dir, const Secrets &secrets,
               const std::string &selector, const std::string &reason);

// Which published CRLs refresh_crl re-signs. Root and Signing exist so the
// two CRLs can run on separate cadences (app::root_crl_next_update_days vs
// app::crl_next_update_days) - a signing-scope run never loads the root key.
enum class CrlScope { Root, Signing, All };

// Re-signs the published CRLs selected by `scope` from their current entry
// sets: same unexpired revocations, crlNumber+1, fresh thisUpdate/nextUpdate.
// Entries whose certificate has expired are pruned per RFC 5280 3.3 (see
// crl_entry_prunable), so a CRL is bounded by the revoked-and-unexpired set,
// not by the CA's age. Run each scope on a schedule shorter than its
// nextUpdate horizon (see share/systemd/yca-crl-refresh.* for signing,
// daily, and share/systemd/yca-root-crl-refresh.* for root, quarterly) so
// relying parties never see a stale CRL.
bool refresh_crl(const cfg::Config &config,
                 const std::filesystem::path &store_dir, const Secrets &secrets,
                 CrlScope scope = CrlScope::All);

// RFC 5280 3.3 pruning decision for one CRL entry: an entry may leave the
// CRL once it has appeared on one scheduled CRL issued beyond the
// certificate's validity. `prev_this_update` is the previous CRL's
// thisUpdate (that CRL carried the entry); `not_after` is the certificate's
// expiry, nullopt when the serial is unknown to cert_index (kept forever,
// conservative). Boundary: expiry exactly at thisUpdate is kept - the CRL
// must be issued strictly beyond the validity period.
bool crl_entry_prunable(std::size_t prev_this_update,
                        std::optional<std::size_t> not_after);

// Writes a certificate to stdout. `profile` is "ca", "server", or "client".
// For "ca", `selector` is an alias ("root-ca"/"signing-ca") or the CA CN; for
// server/client it is the EE CN and the active cert of that profile is
// returned. `encoding` is "pem" or "der"; `chain` is PEM-only.
bool get_cert(const cfg::Config &config, const std::filesystem::path &store_dir,
              const std::string &profile, const std::string &selector,
              const std::string &encoding, bool chain = false);

// Lists certificates via cert_index. `filter` is one of "expiring", "expired",
// "revoked", "last" (windowed by `days`, ordered) or "cn" (by `cn`, which may
// be a CA alias root-ca/signing-ca). Columns: CN, kind, truncated serial,
// expiry, status. `tsv` for tab-separated output. At most `limit` rows are
// printed (0 = unlimited); a truncated listing says so on stderr. Read-only;
// no passphrase.
bool list_certs(const std::filesystem::path &store_dir,
                const std::string &filter, int days, const std::string &cn,
                bool tsv, int limit);

} // namespace ca

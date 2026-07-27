#pragma once

#include <cstddef>
#include <string_view>

namespace app {

// The name is provisional and will change.
inline constexpr std::string_view name = "yca";

// Store passphrase of the internal key backend.
inline constexpr const char *passphrase_env = "CA_STORE_PASSPHRASE";

// User PIN of the PKCS#11 signing token.
inline constexpr const char *pin_env = "CA_HSM_PIN";

// User PIN of the PKCS#11 root token; falls back to pin_env when unset
// (identical-PIN case and the single-token layout).
inline constexpr const char *root_pin_env = "CA_HSM_ROOT_PIN";

// Default directory holding the store and the delivered artifacts.
inline constexpr std::string_view store_dir = "store";
inline constexpr std::string_view store_file = "ca-store.db";

// Configuration table names. `config_table` holds the sections that exist
// once per store ([pki], [pkcs11], [root]); `purpose_table` holds one row
// per issuing CA, so a CA declared after init can be locked on its own.
inline constexpr std::string_view config_table = "ca_config";
inline constexpr std::string_view purpose_table = "ca_purpose";

// The cap on ee_valid_days lives in the profile table (profile.h): it is a
// property of the certificate shape, not of the deployment, and the
// profiles no longer share one number. The TLS profiles keep the historic
// CA/Browser Forum limit deliberately - private trust is exempt from the
// SC-081 reductions (200 days from 2026, 100 from 2027, 47 from 2029).

// SPIFFE ID limits (SPIFFE-ID standard): a trust domain name has "a
// maximum length of 255 bytes", and implementations "SHOULD NOT generate
// URIs of length greater than 2048 bytes" - as the generator, yca
// enforces both when a uri SAN uses the spiffe scheme.
inline constexpr std::size_t spiffe_trust_domain_max = 255;
inline constexpr std::size_t spiffe_id_max = 2048;

// Default window for the `list` time filters, in days.
inline constexpr int default_list_window_days = 30;

// Default row cap for `list` output; --limit overrides, 0 lifts it.
inline constexpr int default_list_limit = 50;

// The --valid floor: below 5 minutes clock skew makes a certificate dead
// on arrival. The ceiling is the effective ee_valid_days policy (the
// industry model: the CA sets a maximum, a request may always be shorter).
inline constexpr int min_valid_override_minutes = 5;

// Validity of an issued enrollment nonce (CSR signing), in minutes.
inline constexpr int max_nonce_validity = 5;

// `get nonce` returns the pending nonce while at least
// max(floor, pct% of its validity) remains; otherwise it rotates.
inline constexpr int nonce_rotate_floor_secs = 60;
inline constexpr int nonce_rotate_pct = 20;

// Renewal window, as percent of lifetime remaining: once an active
// certificate has less than this left, an overlapping successor for the
// same (CN, profile) may be issued (the Let's Encrypt renew-at-1/3
// convention). Using automated rotation via ACME needs the overlap.
// Mirrored by the ACME frontend's ARI window (acme/ari.go, renewPct)
// - change both together.
inline constexpr int renew_window_pct = 33;

// Generated secrets, in raw bytes (delivered hex-encoded).
inline constexpr std::size_t passphrase_bytes = 32;
inline constexpr std::size_t nonce_bytes = 32;

// CRL nextUpdate horizons, in days: the re-publication promise made to
// relying parties (previously Botan's implicit 604800-second default for
// both). Clamped at signing time to the issuing CA's own notAfter - see
// ca::detail::crl_next_update - with a floor that keeps the field sane
// even on an already-expired issuer.
//
// The root CRL gets its own, much longer horizon: the root only certifies
// the signing CA (routine revocations land on the signing CRL), so it
// follows offline-root practice - 6 months, within the industry-standard
// 6-12 month band. Trade-off: revoking the signing CA propagates to
// relying parties only after `yca refresh crl root` is run by hand.
// Each horizon has its own timer; keep the timer interval comfortably
// below the horizon (see share/systemd/yca-crl-refresh.timer and
// yca-root-crl-refresh.timer).
inline constexpr int crl_next_update_days = 7;
inline constexpr int root_crl_next_update_days = 183;
inline constexpr int crl_next_update_floor_secs = 60 * 60;

// SQLite busy_timeout set on EVERY store connection, in ms - the bounded
// lock-wait contract. The Go responder's DSN (_busy_timeout=5000)
// mirrors it; change both together.
inline constexpr int store_busy_timeout_ms = 5000;

// PKCS#11 CK_TOKEN_INFO.label is a fixed 32-byte field.
inline constexpr std::size_t p11_token_label_max = 32;

// `list` serial column truncation, in hex chars.
inline constexpr std::size_t list_serial_hex = 16;

} // namespace app

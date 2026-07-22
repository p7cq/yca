#include "ca.h"

#include <algorithm>
#include <cctype>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <ctime>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <memory>
#include <optional>
#include <print>
#include <stdexcept>
#include <string>
#include <system_error>
#include <unordered_set>
#include <utility>
#include <vector>

#include <botan/botan_all.h>

#include "app.h"
#include "ca_detail.h"
#include "log.h"
#include "p11.h"
#include "util.h"

namespace ca {
namespace {

namespace fs = std::filesystem;
using Clock = std::chrono::system_clock;
using namespace util;

std::string lower(const std::string &s) {
  std::string r;
  r.reserve(s.size());
  for (char c : s)
    r.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(c))));
  return r;
}

void set_perms(const std::filesystem::path &path, fs::perms p) {
  std::error_code ec;
  fs::permissions(path, p, ec);
  if (ec)
    log::warn("could not set permissions on {}: {}", path.string(),
              ec.message());
}

// Artifacts written atomically (tmp + rename), ends up read-only (0400).
// Rewrites (CRL on revoke, re-issued EE cert) replace the file via rename.
bool write_file(const std::filesystem::path &path, std::string_view bytes) {
  fs::path tmp = path;
  tmp += ".tmp";
  {
    std::ofstream out(tmp, std::ios::binary | std::ios::trunc);
    if (!out)
      return false;
    set_perms(tmp, fs::perms::owner_read | fs::perms::owner_write);
    out.write(bytes.data(), static_cast<std::streamsize>(bytes.size()));
    if (!out.good()) {
      out.close();
      std::error_code rm;
      fs::remove(tmp, rm);
      return false;
    }
  }
  set_perms(tmp, fs::perms::owner_read);
  std::error_code ec;
  fs::rename(tmp, path, ec);
  if (ec) {
    std::error_code rm;
    fs::remove(tmp, rm);
    return false;
  }
  return true;
}

// Serializes store writers across processes: the connection's bounded lock
// waits (open_store) plus an immediate (reserved) transaction make
// check-then-insert sequences atomic. Error paths simply return with the
// transaction open - closing the connection rolls it back.
void begin_write(Botan::SQL_Database &db) {
  // WAL: a long-running reader never blocks a CA write
  // and vice versa.
  db.new_statement("PRAGMA journal_mode=WAL")->spin();
  db.new_statement("BEGIN IMMEDIATE")->spin();
}

void commit_write(Botan::SQL_Database &db) {
  db.new_statement("COMMIT")->spin();
}

} // namespace

// Definitions for ca_detail.h - shared with the load-test seeder (seed.cpp).
namespace detail {

std::filesystem::path store_path(const std::filesystem::path &store_dir) {
  return store_dir / app::store_file;
}

std::shared_ptr<Botan::Sqlite3_Database> open_store(const fs::path &db) {
  auto dbh = std::make_shared<Botan::Sqlite3_Database>(db.string());
  dbh->new_statement(
         std::format("PRAGMA busy_timeout = {}", app::store_busy_timeout_ms))
      ->spin();
  return dbh;
}

std::optional<Botan::X509_Certificate> load_ca_cert(const fs::path &store_dir,
                                                    const std::string &slug) {
  const fs::path p = store_dir / "ca" / (slug + ".pem");
  if (!fs::exists(p))
    return std::nullopt;
  return Botan::X509_Certificate(p.string());
}

// The generation-1 identity, straight from the locked config.
CaGen config_gen(const cfg::Config &config, const std::string &kind) {
  return kind == "root"
             ? CaGen{1, config.root_ca_cn, config.root_ca_slug}
             : CaGen{1, config.signing_ca_cn, config.signing_ca_slug};
}

bool has_ca_index(Botan::SQL_Database &db) {
  auto s = db.new_statement("SELECT 1 FROM sqlite_master WHERE type='table' "
                            "AND name='ca_cert_index'");
  return s->step();
}

void ensure_ca_index(Botan::SQL_Database &db, const cfg::Config &config) {
  db.create_table("CREATE TABLE IF NOT EXISTS ca_cert_index ("
                  "kind TEXT NOT NULL, gen INTEGER NOT NULL, cn TEXT NOT NULL, "
                  "slug TEXT NOT NULL, status TEXT NOT NULL, "
                  "PRIMARY KEY (kind, gen))");
  db.new_statement("CREATE INDEX IF NOT EXISTS cai_ks ON "
                   "ca_cert_index(kind, status)")
      ->spin();
  // Generation 1 is what the ceremony created; record it so later
  // generations have a predecessor to succeed.
  for (const char *kind : {"root", "signing"}) {
    auto q = db.new_statement("SELECT 1 FROM ca_cert_index WHERE kind=?1");
    q->bind(1, kind);
    if (q->step())
      continue;
    const CaGen g = config_gen(config, kind);
    auto ins = db.new_statement("INSERT INTO ca_cert_index "
                                "(kind,gen,cn,slug,status) "
                                "VALUES (?1,?2,?3,?4,'active')");
    ins->bind(1, kind);
    ins->bind(2, static_cast<std::size_t>(g.gen));
    ins->bind(3, g.cn);
    ins->bind(4, g.slug);
    ins->spin();
  }
}

std::optional<CaGen> gen_by_cn(Botan::SQL_Database &db, const std::string &kind,
                               const std::string &cn) {
  if (!has_ca_index(db))
    return std::nullopt;
  auto q = db.new_statement("SELECT gen,cn,slug FROM ca_cert_index WHERE "
                            "kind=?1 AND cn=?2 LIMIT 1");
  q->bind(1, kind);
  q->bind(2, cn);
  if (!q->step())
    return std::nullopt;
  return CaGen{static_cast<int>(q->get_size_t(0)), q->get_str(1),
               q->get_str(2)};
}

std::vector<CaGen> live_cas(Botan::SQL_Database &db, const cfg::Config &config,
                            const std::string &kind) {
  std::vector<CaGen> out;
  if (has_ca_index(db)) {
    auto q = db.new_statement("SELECT gen,cn,slug FROM ca_cert_index WHERE "
                              "kind=?1 AND status IN ('active','retiring') "
                              "ORDER BY gen");
    q->bind(1, kind);
    while (q->step())
      out.push_back(
          {static_cast<int>(q->get_size_t(0)), q->get_str(1), q->get_str(2)});
  }
  if (out.empty())
    out.push_back(config_gen(config, kind));
  return out;
}

CaGen active_ca(Botan::SQL_Database &db, const cfg::Config &config,
                const std::string &kind) {
  if (!has_ca_index(db))
    return config_gen(config, kind);
  auto q = db.new_statement("SELECT gen,cn,slug FROM ca_cert_index WHERE "
                            "kind=?1 AND status='active' "
                            "ORDER BY gen DESC LIMIT 1");
  q->bind(1, kind);
  if (!q->step())
    return config_gen(config, kind);
  return CaGen{static_cast<int>(q->get_size_t(0)), q->get_str(1),
               q->get_str(2)};
}

void ensure_cert_index(Botan::SQL_Database &db) {
  db.create_table(
      "CREATE TABLE IF NOT EXISTS cert_index ("
      "fingerprint TEXT PRIMARY KEY, cn TEXT NOT NULL, kind TEXT NOT NULL, "
      "serial TEXT NOT NULL, not_before INTEGER NOT NULL, "
      "not_after INTEGER NOT NULL, status TEXT NOT NULL, "
      "revoked_at INTEGER NOT NULL DEFAULT 0, "
      "reason INTEGER NOT NULL DEFAULT 0)");
  for (const char *ix :
       {"CREATE INDEX IF NOT EXISTS ci_na ON cert_index(not_after)",
        "CREATE INDEX IF NOT EXISTS ci_nb ON cert_index(not_before)",
        "CREATE INDEX IF NOT EXISTS ci_st ON cert_index(status, revoked_at)",
        "CREATE INDEX IF NOT EXISTS ci_cn ON cert_index(cn)",
        "CREATE INDEX IF NOT EXISTS ci_sn ON cert_index(serial)",
        // list: expiring/expired ride (status, not_after) instead of sorting
        // every active row; the CA aliases ride (kind, not_before) instead of
        // scanning the whole index for a handful of CA generations.
        "CREATE INDEX IF NOT EXISTS ci_sna ON cert_index(status, not_after)",
        "CREATE INDEX IF NOT EXISTS ci_knb ON cert_index(kind, not_before)"})
    db.new_statement(ix)->spin();
}

void index_cert(Botan::SQL_Database &db, const Botan::X509_Certificate &c,
                const std::string &kind, const std::string &status,
                std::size_t revoked_at) {
  auto s = db.new_statement(
      "INSERT OR REPLACE INTO cert_index "
      "(fingerprint,cn,kind,serial,not_before,not_after,status,revoked_at) "
      "VALUES (?1,?2,?3,?4,?5,?6,?7,?8)");
  s->bind(1, c.fingerprint("SHA-256"));
  s->bind(2, c.subject_dn().get_first_attribute("X520.CommonName"));
  s->bind(3, kind);
  s->bind(4, Botan::hex_encode(c.serial_number()));
  s->bind(5, static_cast<std::size_t>(c.not_before().time_since_epoch()));
  s->bind(6, static_cast<std::size_t>(c.not_after().time_since_epoch()));
  s->bind(7, status);
  s->bind(8, revoked_at);
  s->spin();
}

bool write_pem(const std::filesystem::path &path,
               const Botan::X509_Object &obj) {
  return write_file(path, obj.PEM_encode());
}

bool write_der(const std::filesystem::path &path,
               const Botan::X509_Object &obj) {
  const std::vector<uint8_t> der = obj.BER_encode();
  return write_file(
      path,
      std::string_view(reinterpret_cast<const char *>(der.data()), der.size()));
}

std::size_t now_epoch() {
  return static_cast<std::size_t>(
      std::chrono::duration_cast<std::chrono::seconds>(
          Clock::now().time_since_epoch())
          .count());
}

bool outlives_issuer(const Botan::X509_Certificate &issuer,
                     std::chrono::seconds validity) {
  if (now_epoch() + static_cast<std::size_t>(validity.count()) <=
      issuer.not_after().time_since_epoch())
    return false;
  // Two ways here: the signing CA aged past (signing - ee) days, or an
  // unlocked *_valid_days was raised too close to the signing CA's lifetime.
  log::error("requested validity ends after the signing CA expires ({}); "
             "lower the configured validity or renew the signing CA",
             issuer.not_after().readable_string());
  return true;
}

uint32_t crl_next_update(const Botan::X509_Certificate &issuer,
                         int horizon_days) {
  const std::size_t want = util::days_to_seconds(horizon_days);
  const std::size_t na = issuer.not_after().time_since_epoch();
  const std::size_t now = now_epoch();
  return static_cast<uint32_t>(std::max<std::size_t>(
      std::min(want, na > now ? na - now : 0),
      static_cast<std::size_t>(app::crl_next_update_floor_secs)));
}

} // namespace detail

using namespace detail;

bool crl_entry_prunable(std::size_t prev_this_update,
                        std::optional<std::size_t> not_after) {
  return not_after && *not_after < prev_this_update;
}

namespace detail {

// The entries of `prev` minus those already past their final scheduled
// appearance (the ca::crl_entry_prunable rule): every entry considered was
// read from `prev`, so `prev` itself is the post-expiry CRL the RFC
// requires. One indexed query (ci_sna) collects the prunable serials; an
// entry whose serial is not in cert_index never lands in the set, so it is
// kept forever, matching the rule's unknown-serial case.
std::vector<Botan::CRL_Entry> prune_crl_entries(Botan::SQL_Database &db,
                                                const Botan::X509_CRL &prev) {
  const auto tu =
      static_cast<std::size_t>(prev.this_update().time_since_epoch());
  std::unordered_set<std::string> prunable;
  auto q = db.new_statement(
      "SELECT serial FROM cert_index WHERE status='revoked' AND not_after<?1");
  q->bind(1, tu);
  while (q->step())
    prunable.insert(q->get_str(0));
  std::vector<Botan::CRL_Entry> kept;
  for (const Botan::CRL_Entry &e : prev.get_revoked())
    if (!prunable.contains(Botan::hex_encode(e.serial_number())))
      kept.push_back(e);
  return kept;
}

} // namespace detail

namespace {

// A uri SAN must be an absolute, IA5-safe URI; when it uses the spiffe
// scheme the SPIFFE-ID rules apply on top (they are strictly narrower,
// and a malformed SPIFFE ID is a silent authorization failure in a mesh
// rather than a visible error). Says why on the error channel.
bool valid_uri_san(const std::string &v) {
  if (!uri_safe(v)) {
    log::error("uri SAN must be an absolute, printable-ASCII URI "
               "(scheme:value, percent-encode the rest): {}",
               v);
    return false;
  }
  if (v.starts_with("spiffe:") && !spiffe_id_safe(v)) {
    log::error("invalid SPIFFE ID: {} (spiffe://<trust-domain>[/<path>], "
               "lowercase trust domain of [a-z0-9.-_] with no port, path "
               "segments of [a-zA-Z0-9.-_], no trailing slash, no query or "
               "fragment)",
               v);
    return false;
  }
  return true;
}

std::optional<Botan::CRL_Code> parse_reason(const std::string &s) {
  const std::string r = lower(s);
  if (r == "unspecified")
    return Botan::CRL_Code::Unspecified;
  if (r == "keycompromise")
    return Botan::CRL_Code::KeyCompromise;
  if (r == "cacompromise")
    return Botan::CRL_Code::CaCompromise;
  if (r == "affiliationchanged")
    return Botan::CRL_Code::AffiliationChanged;
  if (r == "superseded")
    return Botan::CRL_Code::Superseded;
  if (r == "cessationofoperation")
    return Botan::CRL_Code::CessationOfOperation;
  if (r == "certificatehold")
    return Botan::CRL_Code::CertificateHold;
  if (r == "removefromcrl")
    return Botan::CRL_Code::RemoveFromCrl;
  if (r == "privilegewithdrawn")
    return Botan::CRL_Code::PrivilegeWithdrawn;
  if (r == "aacompromise")
    return Botan::CRL_Code::AaCompromise;
  return std::nullopt;
}

// path_limit: a value sets BasicConstraints pathlen; nullopt means CA:TRUE with
// no path length constraint (as real roots do).
Botan::X509_Cert_Options ca_options(const cfg::Config &config,
                                    const std::string &cn, int valid_days,
                                    std::optional<size_t> path_limit) {
  Botan::X509_Cert_Options o("", util::days_to_seconds(valid_days));
  o.common_name = cn;
  o.country = config.country_code;
  o.organization = config.org_name;
  if (path_limit) {
    o.CA_key(*path_limit);
  } else {
    o.is_CA = true;
    o.extensions.add_new(
        std::make_unique<Botan::Cert_Extension::Basic_Constraints>(
            true, std::optional<size_t>{}),
        true);
  }
  return o;
}

// AIA (caIssuers -> root), CDP (root CRL), and CertificatePolicies for
// the signing CA. Pointers reference the root (issuer of this cert). URLs are
// flat under repository_host; cert/CRL file names come from the root CA slug.
// Revocation is CRL-only in this PKI (no OCSP), so AIA carries no OCSP URI;
// the signing CA's status channel is the root CRL (CDP).
// Policies only when root_arc_oid is configured (it is optional).
void add_signing_pointer_extensions(Botan::X509_Cert_Options &o,
                                    const cfg::Config &config,
                                    const std::string &root_slug) {
  const std::string base = "http://" + config.repository_host;
  const std::string &root = root_slug;

  o.extensions.add_new(
      std::make_unique<Botan::Cert_Extension::Authority_Information_Access>(
          std::vector<std::string>{},
          std::vector<std::string>{base + "/" + root + ".crt"}));

  Botan::AlternativeName cdp;
  cdp.add_uri(base + "/" + root + ".crl");
  using DP = Botan::Cert_Extension::CRL_Distribution_Points::Distribution_Point;
  o.extensions.add_new(
      std::make_unique<Botan::Cert_Extension::CRL_Distribution_Points>(
          std::vector<DP>{DP(cdp)}));

  if (!config.root_arc_oid.empty())
    o.extensions.add_new(
        std::make_unique<Botan::Cert_Extension::Certificate_Policies>(
            std::vector<Botan::OID>{Botan::OID(config.root_arc_oid + ".1.1"),
                                    Botan::OID(config.root_arc_oid + ".1.2")}));
}

// EE pointers reference the signing CA (issuer of the leaf). Single policy per
// profile: server -> <arc>.1.1, client -> <arc>.1.2. Works on Extensions
// directly so the CSR path (no X509_Cert_Options) can use it too.
// Revocation is CRL-only in this PKI (no OCSP), so AIA carries only
// caIssuers; the leaf's status channel is the signing CRL (CDP).
void add_ee_pointer_extensions(Botan::Extensions &ext,
                               const cfg::Config &config, Profile profile,
                               const std::string &issuer_slug) {
  const std::string base = "http://" + config.repository_host;
  const std::string &ca = issuer_slug;

  ext.add_new(
      std::make_unique<Botan::Cert_Extension::Authority_Information_Access>(
          std::vector<std::string>{},
          std::vector<std::string>{base + "/" + ca + ".crt"}));

  Botan::AlternativeName cdp;
  cdp.add_uri(base + "/" + ca + ".crl");
  using DP = Botan::Cert_Extension::CRL_Distribution_Points::Distribution_Point;
  ext.add_new(std::make_unique<Botan::Cert_Extension::CRL_Distribution_Points>(
      std::vector<DP>{DP(cdp)}));

  if (!config.root_arc_oid.empty()) {
    const std::string suffix = profile == Profile::Server ? ".1.1" : ".1.2";
    ext.add_new(std::make_unique<Botan::Cert_Extension::Certificate_Policies>(
        std::vector<Botan::OID>{Botan::OID(config.root_arc_oid + suffix)}));
  }
}

const char *profile_name(Profile p) {
  return p == Profile::Server ? "server" : "client";
}

void emit(const Botan::X509_Object &obj, const std::string &encoding) {
  if (encoding == "der") {
    const std::vector<uint8_t> der = obj.BER_encode();
    std::fwrite(der.data(), 1, der.size(), stdout);
  } else {
    const std::string pem = obj.PEM_encode();
    std::fwrite(pem.data(), 1, pem.size(), stdout);
  }
}

std::string fmt_epoch(std::size_t e) {
  const std::time_t t = static_cast<std::time_t>(e);
  std::tm tm{};
  gmtime_r(&t, &tm);
  char buf[32];
  std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M UTC", &tm);
  return buf;
}

// Fingerprint of the NEWEST active cert for (cn, kind), via cert_index.
// During a renewal overlap two certs are active; get/revoke operate on the
// newest one (revoke again to clear the older).
std::optional<std::string> active_fp(Botan::SQL_Database &db,
                                     const std::string &cn,
                                     const std::string &kind) {
  auto q = db.new_statement(
      "SELECT fingerprint FROM cert_index WHERE cn=?1 AND kind=?2 AND "
      "status='active' AND not_after>?3 ORDER BY not_before DESC LIMIT 1");
  q->bind(1, cn);
  q->bind(2, kind);
  q->bind(3, now_epoch());
  if (q->step())
    return q->get_str(0);
  return std::nullopt;
}

// Fingerprint of the active cert with `serial` (uppercase minimal hex, the
// store contract's format) and `kind` - the exact-certificate selector:
// during a renewal overlap by-CN means "the newest", by-serial is precise.
std::optional<std::string> active_fp_by_serial(Botan::SQL_Database &db,
                                               const std::string &serial,
                                               const std::string &kind) {
  auto q = db.new_statement(
      "SELECT fingerprint FROM cert_index WHERE serial=?1 AND kind=?2 AND "
      "status='active' AND not_after>?3 LIMIT 1");
  q->bind(1, serial);
  q->bind(2, kind);
  q->bind(3, now_epoch());
  if (q->step())
    return q->get_str(0);
  return std::nullopt;
}

// Normalizes a user-supplied serial to the store contract's format:
// uppercase minimal hex - ':' separators (openssl -text) and leading
// zeros (openssl -serial pads to even length) are tolerated.
std::string normalize_serial(const std::string &s) {
  std::string out;
  out.reserve(s.size());
  for (char c : s) {
    if (c == ':')
      continue;
    out.push_back(
        static_cast<char>(std::toupper(static_cast<unsigned char>(c))));
  }
  const auto nz = out.find_first_not_of('0');
  if (nz == std::string::npos)
    return out.empty() ? out : "0";
  return out.substr(nz);
}

// True (and logged) when an active cert for (cn, kind) exists that is not
// yet inside the renewal window - the uniqueness rule, made renewal-aware:
// a successor may only be issued once the active cert has less than
// app::renew_window_pct of its lifetime left. The overlap is what automated
// rotation needs; the superseded cert is left to expire, never auto-revoked
// (it may still be serving during the rollout).
bool blocking_duplicate(Botan::SQL_Database &db, const std::string &cn,
                        const std::string &kind) {
  auto q = db.new_statement(
      "SELECT COUNT(*) FROM cert_index WHERE cn=?1 AND kind=?2 AND "
      "status='active' AND not_after>?3 AND "
      "(not_after - ?3) * 100 > (not_after - not_before) * ?4");
  q->bind(1, cn);
  q->bind(2, kind);
  q->bind(3, now_epoch());
  q->bind(4, static_cast<std::size_t>(app::renew_window_pct));
  q->step();
  if (q->get_size_t(0) == 0)
    return false;
  log::error("an active {} certificate for CN '{}' already exists (not yet "
             "within the {}% renewal window)",
             kind, cn, app::renew_window_pct);
  return true;
}

// Loads a stored certificate (DER blob) by its SHA-256 fingerprint.
std::optional<Botan::X509_Certificate> load_cert(Botan::SQL_Database &db,
                                                 const std::string &fp) {
  auto q = db.new_statement(
      "SELECT certificate FROM certificates WHERE fingerprint=?1");
  q->bind(1, fp);
  if (!q->step())
    return std::nullopt;
  const auto [blob, len] = q->get_blob(0);
  return Botan::X509_Certificate(std::vector<uint8_t>(blob, blob + len));
}

// The full config, recorded at init and enforced on every later run: every
// field is locked, re-init to change anything. ee_valid_days is issuance
// policy but locked too - per-issuance flexibility is --valid, which may
// request anything up to it (the policy is the ceiling).
std::vector<std::pair<std::string, std::string>>
locked_config(const cfg::Config &c) {
  return {
      {"org_name", c.org_name},
      {"country_code", c.country_code},
      {"repository_host", c.repository_host},
      {"root_arc_oid", c.root_arc_oid},
      {"root_ca_cn", c.root_ca_cn},
      {"root_ca_curve", c.root_ca_curve},
      {"root_ca_digest", c.root_ca_digest},
      {"root_ca_valid_days", std::to_string(c.root_ca_valid_days)},
      {"root_ca_slug_prefix", c.root_ca_slug_prefix},
      {"root_ca_slug", c.root_ca_slug},
      {"signing_ca_cn", c.signing_ca_cn},
      {"signing_ca_curve", c.signing_ca_curve},
      {"signing_ca_digest", c.signing_ca_digest},
      {"signing_ca_valid_days", std::to_string(c.signing_ca_valid_days)},
      {"signing_ca_slug_prefix", c.signing_ca_slug_prefix},
      {"signing_ca_slug", c.signing_ca_slug},
      {"ee_curve", c.ee_curve},
      {"ee_digest", c.ee_digest},
      {"key_backend", c.key_backend},
      {"pkcs11_module", c.pkcs11_module},
      {"pkcs11_token_label", c.pkcs11_token_label},
      {"ee_valid_days", std::to_string(c.ee_valid_days)},
  };
}

// Env var naming the CA secret for this backend (for error messages).
const char *secret_env(const cfg::Config &config) {
  return config.key_backend == "pkcs11" ? app::pin_env : app::passphrase_env;
}

// Adopt-or-generate one CA key on the token: use the existing keypair labeled
// `label` (validated against `curve`), or generate a fresh token-resident one.
std::shared_ptr<const Botan::Private_Key>
token_ca_key(p11::Token &token, const std::string &label,
             const std::string &curve) {
  if (auto key = token.find_keypair(label, curve)) {
    log::info("using existing token keypair '{}'", label);
    return key;
  }
  log::info("generating keypair '{}' on the token", label);
  return token.generate_keypair(label, curve);
}

// Loads one CA private key: from the store (backend "internal",
// passphrase-encrypted) or from the token (label = the CA's slug). `token`
// owns the PKCS#11 session and must outlive the returned key; an already
// open session is reused, so several keys cost a single login.
std::shared_ptr<const Botan::Private_Key>
ca_key(const cfg::Config &config, Botan::Certificate_Store_In_SQL &store,
       const Botan::X509_Certificate &cert, const std::string &slug,
       const std::string &curve, std::string_view secret,
       std::optional<p11::Token> &token) {
  if (config.key_backend == "pkcs11") {
    if (!token)
      token.emplace(config, secret, /*read_write=*/false);
    auto key = token->find_keypair(slug, curve);
    if (!key)
      throw std::runtime_error(
          std::format("CA key '{}' not found on the token", slug));
    return key;
  }
  return store.find_key(cert);
}

// The signing CA key, the one every issuance/revocation needs.
std::shared_ptr<const Botan::Private_Key>
signing_key(const cfg::Config &config, Botan::Certificate_Store_In_SQL &store,
            const Botan::X509_Certificate &sign_cert, const std::string &slug,
            std::string_view secret, std::optional<p11::Token> &token) {
  return ca_key(config, store, sign_cert, slug, config.signing_ca_curve, secret,
                token);
}

bool create(const cfg::Config &config, const fs::path &db_path,
            std::string_view secret) {
  Botan::AutoSeeded_RNG rng;

  // Keys per backend: token-resident (adopted by label, or generated on the
  // token), or in-memory ECDSA later persisted encrypted into the store.
  std::optional<p11::Token> token;
  std::shared_ptr<const Botan::Private_Key> root_key, sign_key;
  if (config.key_backend == "pkcs11") {
    token.emplace(config, secret, /*read_write=*/true);
    root_key = token_ca_key(*token, config.root_ca_slug, config.root_ca_curve);
    sign_key =
        token_ca_key(*token, config.signing_ca_slug, config.signing_ca_curve);
  } else {
    root_key = std::make_shared<Botan::ECDSA_PrivateKey>(
        rng, Botan::EC_Group::from_name(config.root_ca_curve));
    sign_key = std::make_shared<Botan::ECDSA_PrivateKey>(
        rng, Botan::EC_Group::from_name(config.signing_ca_curve));
  }

  auto root_opts = ca_options(config, config.root_ca_cn,
                              config.root_ca_valid_days, std::nullopt);
  auto root_cert = Botan::X509::create_self_signed_cert(
      root_opts, *root_key, config.root_ca_digest, rng);

  auto sign_opts = ca_options(config, config.signing_ca_cn,
                              config.signing_ca_valid_days, size_t{0});
  sign_opts.add_ex_constraint(Botan::OID("1.3.6.1.5.5.7.3.1")); // serverAuth
  sign_opts.add_ex_constraint(Botan::OID("1.3.6.1.5.5.7.3.2")); // clientAuth
  add_signing_pointer_extensions(sign_opts, config, config.root_ca_slug);
  auto sign_req = Botan::X509::create_cert_req(sign_opts, *sign_key,
                                               config.signing_ca_digest, rng);

  Botan::X509_CA issuer(root_cert, *root_key, config.root_ca_digest, rng);
  auto now = Clock::now();
  Botan::X509_Time nb(now);
  Botan::X509_Time na(now + std::chrono::days(config.signing_ca_valid_days));
  auto sign_cert = issuer.sign_request(sign_req, rng, nb, na);

  auto db = open_store(db_path);
  // 0600: holds the encrypted CA keys, and the store must stay writable.
  set_perms(db_path, fs::perms::owner_read | fs::perms::owner_write);
  // New stores start in WAL directly (see begin_write).
  db->new_statement("PRAGMA journal_mode=WAL")->spin();
  Botan::Certificate_Store_In_SQL store(db, secret, rng);
  store.insert_cert(root_cert);
  store.insert_cert(sign_cert);
  if (config.key_backend != "pkcs11") { // pkcs11: keys never leave the token
    store.insert_key(root_cert, *root_key);
    store.insert_key(sign_cert, *sign_key);
  }

  db->create_table(std::format("CREATE TABLE IF NOT EXISTS {} ("
                               "key TEXT PRIMARY KEY, value TEXT NOT NULL)",
                               app::config_table));
  for (const auto &[k, v] : locked_config(config)) {
    auto cins = db->new_statement(std::format(
        "INSERT INTO {} (key, value) VALUES (?1, ?2)", app::config_table));
    cins->bind(1, k);
    cins->bind(2, v);
    cins->spin();
  }

  ensure_cert_index(*db);
  index_cert(*db, root_cert, "root");
  index_cert(*db, sign_cert, "signing");
  // Generation 1 of each CA; generation is signed from the store.
  ensure_ca_index(*db, config);

  const fs::path ca_dir = db_path.parent_path() / "ca";
  fs::create_directories(ca_dir);
  set_perms(ca_dir, fs::perms::owner_all);
  const std::string &root_s = config.root_ca_slug;
  const std::string &sign_s = config.signing_ca_slug;
  if (!write_pem(ca_dir / (root_s + ".pem"), root_cert) ||
      !write_pem(ca_dir / (sign_s + ".pem"), sign_cert) ||
      !write_der(ca_dir / (root_s + ".crt"), root_cert) ||
      !write_der(ca_dir / (sign_s + ".crt"), sign_cert))
    log::warn("could not write CA cert exports under {}", ca_dir.string());

  // Empty signed CRLs so the CDP URLs resolve before any revocation.
  Botan::X509_CA sign_ca(sign_cert, *sign_key, config.signing_ca_digest, rng);
  const fs::path root_crl = ca_dir / (root_s + ".crl");
  const fs::path sign_crl = ca_dir / (sign_s + ".crl");
  if (!write_der(root_crl,
                 issuer.new_crl(
                     rng, crl_next_update(root_cert,
                                          app::root_crl_next_update_days))) ||
      !write_der(sign_crl, sign_ca.new_crl(
                               rng, crl_next_update(
                                        sign_cert, app::crl_next_update_days))))
    log::warn("could not write CRLs under {}", ca_dir.string());

  log::info("created 2-tier CA: root '{}' + signing '{}'", config.root_ca_cn,
            config.signing_ca_cn);
  return true;
}

} // namespace

bool is_initialized(const fs::path &store_dir) {
  const fs::path db = store_path(store_dir);

  std::error_code ec;
  if (!fs::exists(db, ec)) {
    if (ec)
      log::fatal("filesystem error: {}: {}", db.c_str(), ec.message());
    return false; // will initialize
  }

  try {
    auto h = open_store(db);
    auto locked = [&](const char *key) {
      auto s = h->new_statement(
          std::format("SELECT value FROM {} WHERE key=?1", app::config_table));
      s->bind(1, key);
      return s->step() ? s->get_str(0) : std::string();
    };
    const std::string root_cn = locked("root_ca_cn");
    const std::string sign_cn = locked("signing_ca_cn");

    if (root_cn.empty() || sign_cn.empty())
      return false;
    // Initialization is a historical fact about the ceremony, what
    // later became of an anchor does not unmake it. What still
    // matters is that the anchors the config names were actually issued.
    auto anchor = [&](const std::string &kind, const std::string &cn) {
      auto s = h->new_statement(
          "SELECT 1 FROM cert_index WHERE kind=?1 AND cn=?2 LIMIT 1");
      s->bind(1, kind);
      s->bind(2, cn);
      return s->step();
    };
    return anchor("root", root_cn) && anchor("signing", sign_cn);
  } catch (const std::exception &) {
    return false; // unreadable store => not initialized
  }
}

bool init(const cfg::Config &config, const fs::path &store_dir,
          std::string_view secret) {
  const fs::path db_path = store_path(store_dir);
  if (is_initialized(store_dir)) {
    log::error("already initialized ({})", db_path.string());
    return false;
  }
  // Not initialized: init starts from a store_dir that is absent or an empty
  // directory. Whatever else occupies it (a truncated db, stray leftovers) is
  // the user's to move or back up - the filesystem is left alone.
  std::error_code ec;
  const bool present = fs::exists(store_dir, ec);
  const bool usable = !present || (fs::is_directory(store_dir, ec) &&
                                   fs::is_empty(store_dir, ec));
  if (ec)
    log::fatal("filesystem error: {}: {}", store_dir.c_str(), ec.message());
  if (!usable) {
    log::error("{} exists but is not an initialized store; move it away and "
               "re-run '{} init'",
               store_dir.string(), app::name);
    return false;
  }

  // The CA secret is settled before anything touches the filesystem: a
  // failed precondition must not leave a store_dir (or a log file) behind.
  std::string generated;
  if (config.key_backend == "pkcs11") {
    // The token PIN cannot be invented for the user.
    if (secret.empty()) {
      log::error("{} not set (token user PIN)", app::pin_env);
      return false;
    }
  } else if (secret.empty()) {
    Botan::AutoSeeded_RNG rng;
    std::vector<uint8_t> raw(app::passphrase_bytes);
    rng.randomize(raw.data(), raw.size());
    generated = Botan::hex_encode(raw);
    secret = generated;
    log::to_stdout("\n=== GENERATED CA PASSPHRASE (shown once) ===\n{}\n"
                   "Store it now; set {} to it on future runs.\n"
                   "============================================\n\n",
                   generated, app::passphrase_env);
  }

  fs::create_directories(store_dir);
  set_perms(store_dir, fs::perms::owner_all);
  try {
    return create(config, db_path, secret);
  } catch (const std::exception &e) {
    log::error("CA init failed: {}", e.what());
    return false;
  }
}

bool renew_signing_ca(const cfg::Config &config, const fs::path &store_dir,
                      std::string_view secret, const std::string &new_cn) {
  const fs::path db = store_path(store_dir);
  if (!fs::exists(db)) {
    log::error("not initialized; run '{} init'", app::name);
    return false;
  }
  if (secret.empty()) {
    log::error("{} not set (the root key signs the new generation)",
               secret_env(config));
    return false;
  }
  if (new_cn.empty()) {
    log::error("--new-cn is required (the new generation's common name)");
    return false;
  }

  Botan::AutoSeeded_RNG rng;
  auto dbh = open_store(db);
  Botan::Certificate_Store_In_SQL store(dbh, secret, rng);
  ensure_ca_index(*dbh, config);

  const CaGen root = active_ca(*dbh, config, "root");
  const CaGen incumbent = active_ca(*dbh, config, "signing");
  const CaGen next{incumbent.gen + 1, new_cn,
                   config.signing_ca_slug_prefix +
                       std::to_string(incumbent.gen + 1)};

  // A CN identifies a generation in `cert_index` and in the CRL's issuer
  // field, so generations must not share one.
  if (new_cn == root.cn || gen_by_cn(*dbh, "signing", new_cn) ||
      gen_by_cn(*dbh, "root", new_cn)) {
    log::error("CN '{}' already names a CA generation; pick another", new_cn);
    return false;
  }

  const fs::path ca_dir = store_dir / "ca";
  const fs::path pem = ca_dir / (next.slug + ".pem");
  if (fs::exists(pem))
    log::warn("{} exists from an unfinished renewal; replacing it",
              pem.string());

  auto root_cert = load_ca_cert(store_dir, root.slug);
  if (!root_cert) {
    log::error("root CA cert not found under {}", ca_dir.string());
    return false;
  }
  // The new generation gets the configured signing validity, and a CA may
  // no more outlive its issuer than a leaf may: a root too close to its
  // own notAfter must itself be rotated first.
  if (outlives_issuer(*root_cert, std::chrono::seconds(std::chrono::days(
                                      config.signing_ca_valid_days))))
    return false;

  std::optional<p11::Token> token;
  std::shared_ptr<const Botan::Private_Key> root_key, new_key;
  try {
    if (config.key_backend == "pkcs11") {
      // Read-write: the new generation's keypair is generated on the token
      // under its own label (adopted if a previous attempt left it there).
      token.emplace(config, secret, /*read_write=*/true);
      root_key = token_ca_key(*token, root.slug, config.root_ca_curve);
      new_key = token_ca_key(*token, next.slug, config.signing_ca_curve);
    } else {
      root_key = store.find_key(*root_cert);
      new_key = std::make_shared<Botan::ECDSA_PrivateKey>(
          rng, Botan::EC_Group::from_name(config.signing_ca_curve));
    }
  } catch (const std::exception &e) {
    log::error("cannot load the root key (wrong {}?): {}",
               config.key_backend == "internal" ? "passphrase" : "user PIN",
               e.what());
    return false;
  }
  if (!root_key || !new_key) {
    log::error("root key missing from store");
    return false;
  }

  // Same profile as the generation the ceremony created, pointing at the
  // active root: only the name, the key and the generation change.
  auto opts =
      ca_options(config, next.cn, config.signing_ca_valid_days, size_t{0});
  opts.add_ex_constraint(Botan::OID("1.3.6.1.5.5.7.3.1")); // serverAuth
  opts.add_ex_constraint(Botan::OID("1.3.6.1.5.5.7.3.2")); // clientAuth
  add_signing_pointer_extensions(opts, config, root.slug);
  auto req = Botan::X509::create_cert_req(opts, *new_key,
                                          config.signing_ca_digest, rng);
  Botan::X509_CA issuer(*root_cert, *root_key, config.root_ca_digest, rng);
  const auto tp = Clock::now();
  auto cert = issuer.sign_request(
      req, rng, Botan::X509_Time(tp),
      Botan::X509_Time(tp + std::chrono::days(config.signing_ca_valid_days)));

  // Artifacts before the database: an interrupted renewal that leaves
  // files behind is recoverable (the next run replaces them), while an
  // active generation without a published certificate would break issuance.
  Botan::X509_CA new_ca(cert, *new_key, config.signing_ca_digest, rng);
  if (!write_pem(pem, cert) ||
      !write_der(ca_dir / (next.slug + ".crt"), cert) ||
      !write_der(ca_dir / (next.slug + ".crl"),
                 new_ca.new_crl(
                     rng, crl_next_update(cert, app::crl_next_update_days)))) {
    log::error("could not write the new CA artifacts under {}",
               ca_dir.string());
    return false;
  }

  begin_write(*dbh);
  store.insert_cert(cert);
  if (config.key_backend != "pkcs11")
    store.insert_key(cert, *new_key);
  ensure_cert_index(*dbh);
  index_cert(*dbh, cert, "signing");
  auto retire = dbh->new_statement("UPDATE ca_cert_index SET status='retiring' "
                                   "WHERE kind='signing' AND status='active'");
  retire->spin();
  auto ins = dbh->new_statement("INSERT INTO ca_cert_index "
                                "(kind,gen,cn,slug,status) "
                                "VALUES ('signing',?1,?2,?3,'active')");
  ins->bind(1, static_cast<std::size_t>(next.gen));
  ins->bind(2, next.cn);
  ins->bind(3, next.slug);
  ins->spin();
  commit_write(*dbh);

  log::info("renewed the signing CA: generation {} '{}' ({}) is now active; "
            "'{}' is retiring and keeps publishing its CRL",
            next.gen, next.cn, next.slug, incumbent.cn);
  log::to_stdout("{}", next.cn);
  return true;
}

std::optional<cfg::Config> load_config(const fs::path &store_dir) {
  const fs::path db = store_path(store_dir);
  if (!fs::exists(db))
    return std::nullopt;
  auto dbh = open_store(db);
  std::map<std::string, std::string> m;
  auto sel = dbh->new_statement(
      std::format("SELECT key, value FROM {}", app::config_table));
  while (sel->step())
    m.emplace(sel->get_str(0), sel->get_str(1));
  if (m.empty())
    return std::nullopt;

  auto S = [&](const char *k) {
    auto it = m.find(k);
    return it != m.end() ? it->second : std::string();
  };
  auto I = [&](const char *k) {
    auto it = m.find(k);
    return it != m.end() ? std::stoi(it->second) : 0;
  };
  cfg::Config c;
  c.org_name = S("org_name");
  c.country_code = S("country_code");
  c.repository_host = S("repository_host");
  c.root_arc_oid = S("root_arc_oid");
  c.root_ca_cn = S("root_ca_cn");
  c.root_ca_curve = S("root_ca_curve");
  c.root_ca_digest = S("root_ca_digest");
  c.root_ca_valid_days = I("root_ca_valid_days");
  // The prefix is the rotation baseline; the
  // derived slug is the operative identity for files, URLs and key
  // labels. Both are snapshotted at init and read back here.
  c.root_ca_slug_prefix = S("root_ca_slug_prefix");
  c.root_ca_slug = S("root_ca_slug");
  c.signing_ca_cn = S("signing_ca_cn");
  c.signing_ca_curve = S("signing_ca_curve");
  c.signing_ca_digest = S("signing_ca_digest");
  c.signing_ca_valid_days = I("signing_ca_valid_days");
  c.signing_ca_slug_prefix = S("signing_ca_slug_prefix");
  c.signing_ca_slug = S("signing_ca_slug");
  c.ee_curve = S("ee_curve");
  c.ee_digest = S("ee_digest");
  c.ee_valid_days = I("ee_valid_days");
  c.key_backend = S("key_backend");
  c.pkcs11_module = S("pkcs11_module");
  c.pkcs11_token_label = S("pkcs11_token_label");
  return c;
}

void reconcile(const cfg::Config &file, const cfg::Config &eff) {
  // The whole config is locked: warn and ignore any change; DB stays
  // authoritative. Re-init to change anything.
  const auto fk = locked_config(file);
  const auto ek = locked_config(eff);
  for (std::size_t i = 0; i < fk.size(); ++i)
    if (fk[i].second != ek[i].second)
      log::warn("ignoring readonly params changed in {}.toml ({}), check "
                "against 'get config'",
                app::name, fk[i].first);
}

namespace {

// --valid must sit inside the policy: at least the 5-minute floor (below
// it clock skew kills the certificate on arrival), at most the effective
// ee_valid_days ceiling - shorter than policy is always allowed.
bool check_valid_override(const cfg::Config &config, std::chrono::seconds v) {
  if (v >= std::chrono::minutes(app::min_valid_override_minutes) &&
      v <= std::chrono::days(config.ee_valid_days))
    return true;
  log::error("--valid must be in [{}m, {}d] (ee_valid_days is the ceiling)",
             app::min_valid_override_minutes, config.ee_valid_days);
  return false;
}

} // namespace

bool issue_ee(const cfg::Config &config, const fs::path &store_dir,
              std::string_view secret, Profile profile, const std::string &cn,
              const std::vector<San> &extra_sans,
              std::optional<std::chrono::seconds> valid_override) {
  const std::chrono::seconds validity = valid_override.value_or(
      std::chrono::seconds(std::chrono::days(config.ee_valid_days)));
  if (valid_override && !check_valid_override(config, *valid_override))
    return false;

  std::vector<San> sans;
  if (profile == Profile::Server) {
    // The CN becomes a dNSName SAN (IA5String): hostname ASCII only. A client
    // CN stays DN-only (UTF8String), so it is free-form.
    if (!dns_safe(cn)) {
      log::error("server CN must be an ASCII hostname [A-Za-z0-9.*-]: '{}'",
                 cn);
      return false;
    }
    sans.push_back({San::Type::Dns, cn}); // server always gets DNS:CN
  } else if (extra_sans.empty()) {
    log::error("client profile requires at least one --san");
    return false;
  }
  sans.insert(sans.end(), extra_sans.begin(), extra_sans.end());

  // dns/email SANs are IA5String (ASCII); IDN hosts must come as punycode.
  for (const auto &s : sans) {
    if (s.type == San::Type::Dns && !dns_safe(s.value)) {
      log::error("--san dns must be an ASCII hostname (IDN: use punycode): {}",
                 s.value);
      return false;
    }
    if (s.type == San::Type::Email && !ascii_graphic(s.value)) {
      log::error("--san email must be printable ASCII (IA5String): {}",
                 s.value);
      return false;
    }
    if (s.type == San::Type::Uri && !valid_uri_san(s.value))
      return false;
  }
  // At most one URI SAN: an X509-SVID "MUST contain exactly one URI SAN,
  // and by extension, exactly one SPIFFE ID" - a second one would make
  // the certificate unusable as an SVID.
  if (std::count_if(sans.begin(), sans.end(), [](const San &s) {
        return s.type == San::Type::Uri;
      }) > 1) {
    log::error("at most one --san uri per certificate (an X509-SVID carries "
               "exactly one URI SAN)");
    return false;
  }

  const fs::path db = store_path(store_dir);
  if (!fs::exists(db)) {
    log::error("not initialized; run '{} init'", app::name);
    return false;
  }
  if (secret.empty()) {
    log::error("{} not set (required to sign with the CA)", secret_env(config));
    return false;
  }

  Botan::AutoSeeded_RNG rng;
  auto dbh = open_store(db);
  Botan::Certificate_Store_In_SQL store(dbh, secret, rng);

  const CaGen sign = active_ca(*dbh, config, "signing");
  auto sign_cert = load_ca_cert(store_dir, sign.slug);
  if (!sign_cert) {
    log::error("signing CA cert not found under {}",
               (store_dir / "ca").string());
    return false;
  }
  if (outlives_issuer(*sign_cert, validity))
    return false;

  // Fail fast before the signing key lookup (seconds on NK HSM): uniqueness
  // is answerable from cert_index alone. The write lock below repeats the
  // check authoritatively.
  auto duplicate = [&] {
    return blocking_duplicate(*dbh, cn, profile_name(profile));
  };
  if (duplicate())
    return false;

  std::optional<p11::Token> token; // must outlive sign_key (owns the session)
  std::shared_ptr<const Botan::Private_Key> sign_key;
  try {
    sign_key = signing_key(config, store, *sign_cert, sign.slug, secret, token);
  } catch (const std::exception &e) {
    log::error("cannot load signing key (wrong {}?): {}",
               config.key_backend == "internal" ? "passphrase" : "user PIN",
               e.what());
    return false;
  }
  if (!sign_key) {
    log::error("signing key missing from store");
    return false;
  }

  // Uniqueness via cert_index: the write lock spans check + insert so a
  // concurrent create for the same (cn, profile) cannot slip between them.
  begin_write(*dbh);
  ensure_cert_index(*dbh); // idempotent: guarantees the full schema
  if (duplicate())
    return false;

  Botan::EC_Group grp = Botan::EC_Group::from_name(config.ee_curve);
  Botan::ECDSA_PrivateKey ee_key(rng, grp);

  Botan::X509_Cert_Options o("", static_cast<uint32_t>(validity.count()));
  o.common_name = cn; // leaf DN: CN only
  o.add_constraints(Botan::Key_Constraints(
      static_cast<uint32_t>(Botan::Key_Constraints::DigitalSignature)));
  o.add_ex_constraint(Botan::OID(
      profile == Profile::Server ? "1.3.6.1.5.5.7.3.1" : "1.3.6.1.5.5.7.3.2"));

  Botan::AlternativeName an;
  for (const auto &s : sans) {
    switch (s.type) {
    case San::Type::Dns:
      an.add_dns(s.value);
      break;
    case San::Type::Email:
      an.add_email(s.value);
      break;
    case San::Type::Ip:
      if (auto v = parse_ipv4(s.value)) {
        an.add_ipv4_address(*v);
      } else {
        log::error("invalid IPv4 in --san: {}", s.value);
        return false;
      }
      break;
    case San::Type::Uri:
      an.add_uri(s.value);
      break;
    }
  }
  o.extensions.add_new(
      std::make_unique<Botan::Cert_Extension::Subject_Alternative_Name>(an));
  add_ee_pointer_extensions(o.extensions, config, profile, sign.slug);

  auto req = Botan::X509::create_cert_req(o, ee_key, config.ee_digest, rng);
  Botan::X509_CA issuer(*sign_cert, *sign_key, config.ee_digest, rng);
  const auto tp = Clock::now();
  auto ee_cert = issuer.sign_request(req, rng, Botan::X509_Time(tp),
                                     Botan::X509_Time(tp + validity));

  store.insert_cert(ee_cert);
  index_cert(*dbh, ee_cert, profile_name(profile));
  commit_write(*dbh);

  const fs::path ee_dir = store_dir / "ee";
  fs::create_directories(ee_dir);
  set_perms(ee_dir, fs::perms::owner_all);
  const std::string name = slug(cn);
  bool ok = write_pem(ee_dir / (name + ".crt"), ee_cert);
  if (!write_file(ee_dir / (name + ".key"), Botan::PKCS8::PEM_encode(ee_key)))
    ok = false;
  if (!ok)
    log::warn("could not write EE artifacts under {}", ee_dir.string());

  log::info("issued {} certificate for CN '{}'", profile_name(profile), cn);
  return true;
}

namespace {

// The enrollment table backs identity validation for CSR signing: one row per
// identity, holding at most one pending nonce. `consumed` defaults to true so
// a row is in the "nothing pending" state until a `get nonce`.
void ensure_enrollment(Botan::SQL_Database &db) {
  db.create_table("CREATE TABLE IF NOT EXISTS enrollment ("
                  "id TEXT PRIMARY KEY, role TEXT NOT NULL DEFAULT 'ee', "
                  "created INTEGER NOT NULL, nonce TEXT, "
                  "issued INTEGER NOT NULL DEFAULT 0, "
                  "validity INTEGER NOT NULL DEFAULT 0, "
                  "consumed INTEGER NOT NULL DEFAULT 1)");
}

// Resolves the sign --csr argument: inline PEM (a single-line paste works -
// Botan's PEM decoder ignores line structure), "-" for stdin, or a file path.
// PEM vs DER is auto-detected later, at PKCS#10 decode.
std::optional<std::string> read_csr_bytes(const std::string &src) {
  if (src.starts_with("-----BEGIN"))
    return src;
  std::string bytes;
  if (src == "-") {
    bytes.assign(std::istreambuf_iterator<char>(std::cin),
                 std::istreambuf_iterator<char>());
  } else {
    std::ifstream in(src, std::ios::binary);
    if (!in) {
      log::error("cannot read CSR file: {}", src);
      return std::nullopt;
    }
    bytes.assign(std::istreambuf_iterator<char>(in),
                 std::istreambuf_iterator<char>());
  }
  if (bytes.empty()) {
    log::error("empty CSR ({})", src == "-" ? "stdin" : src);
    return std::nullopt;
  }
  return bytes;
}

} // namespace

bool enroll(const fs::path &store_dir, const std::string &id) {
  if (id.empty() || id.size() > 256) {
    log::error("--id must be 1..256 characters");
    return false;
  }
  for (char ch : id) {
    const auto c = static_cast<unsigned char>(ch);
    if (c < 0x20 || c == 0x7f) {
      log::error("--id must not contain control characters");
      return false;
    }
  }

  const fs::path db = store_path(store_dir);
  if (!fs::exists(db)) {
    log::error("not initialized; run '{} init'", app::name);
    return false;
  }
  auto dbh = open_store(db);
  begin_write(*dbh);
  ensure_enrollment(*dbh);
  {
    auto q = dbh->new_statement("SELECT 1 FROM enrollment WHERE id=?1");
    q->bind(1, id);
    if (q->step()) {
      log::error("identity '{}' is already enrolled", id);
      return false;
    }
  }
  auto ins = dbh->new_statement(
      "INSERT INTO enrollment (id, role, created) VALUES (?1, 'ee', ?2)");
  ins->bind(1, id);
  ins->bind(2, now_epoch());
  ins->spin();
  commit_write(*dbh);

  log::info("enrolled identity '{}'", id);
  return true;
}

bool get_nonce(const fs::path &store_dir, const std::string &id) {
  const fs::path db = store_path(store_dir);
  if (!fs::exists(db)) {
    log::error("not initialized; run '{} init'", app::name);
    return false;
  }
  auto dbh = open_store(db);
  begin_write(*dbh);
  ensure_enrollment(*dbh);

  auto q = dbh->new_statement("SELECT IFNULL(nonce,''), issued, validity, "
                              "consumed FROM enrollment WHERE id=?1");
  q->bind(1, id);
  if (!q->step()) {
    log::error("identity '{}' is not enrolled; run '{} enroll --id ...'", id,
               app::name);
    return false;
  }
  const std::string pending = q->get_str(0);
  const std::size_t issued = q->get_size_t(1);
  const std::size_t validity_min = q->get_size_t(2);
  const bool consumed = q->get_size_t(3) != 0;

  // Return the pending nonce while it has usable life left - at least
  // max(nonce_rotate_floor_secs, nonce_rotate_pct% of its validity);
  // otherwise rotate. Rotation is free (nothing references the old nonce),
  // so no grace-period corner cases.
  const std::size_t now = now_epoch();
  const std::size_t expiry = issued + validity_min * 60;
  const std::size_t min_left =
      std::max<std::size_t>(app::nonce_rotate_floor_secs,
                            validity_min * 60 * app::nonce_rotate_pct / 100);
  if (!pending.empty() && !consumed && now < expiry &&
      expiry - now >= min_left) {
    commit_write(*dbh);
    log::to_stdout("{}", pending);
    return true;
  }

  Botan::AutoSeeded_RNG rng;
  std::vector<uint8_t> raw(app::nonce_bytes);
  rng.randomize(raw.data(), raw.size());
  const std::string nonce = Botan::hex_encode(raw, /*uppercase=*/false);
  auto u = dbh->new_statement("UPDATE enrollment SET nonce=?1, issued=?2, "
                              "validity=?3, consumed=0 WHERE id=?4");
  u->bind(1, nonce);
  u->bind(2, now);
  u->bind(3, static_cast<std::size_t>(app::max_nonce_validity));
  u->bind(4, id);
  u->spin();
  commit_write(*dbh);

  log::to_stdout("{}", nonce);
  return true;
}

bool sign_csr(const cfg::Config &config, const fs::path &store_dir,
              std::string_view secret, Profile profile, const std::string &id,
              const std::string &nonce, const std::string &csr_src,
              std::optional<std::chrono::seconds> valid_override) {
  if (valid_override && !check_valid_override(config, *valid_override))
    return false;
  // Parse and police the CSR first: cheap failures before any store or key
  // access. PKCS#10 decode already verifies the CSR's self-signature
  // (proof-of-possession); a tampered or unsigned request never parses.
  auto bytes = read_csr_bytes(csr_src);
  if (!bytes)
    return false;
  std::optional<Botan::PKCS10_Request> req;
  try {
    Botan::DataSource_Memory src(*bytes);
    req.emplace(src);
  } catch (const std::exception &e) {
    log::error("invalid CSR (parse or signature): {}", e.what());
    return false;
  }

  // Key policy: same as internally generated EE keys.
  auto pub = req->subject_public_key();
  const auto *ec = dynamic_cast<const Botan::EC_PublicKey *>(pub.get());
  if (pub->algo_name() != "ECDSA" || !ec ||
      ec->domain() != Botan::EC_Group::from_name(config.ee_curve)) {
    log::error("CSR public key must be ECDSA on {}", config.ee_curve);
    return false;
  }

  // Subject DN: only the CN is honored; the leaf DN is rebuilt as CN-only.
  // Modern ACME clients (certbot) send SAN-only CSRs with an empty subject:
  // the CN is derived from the first dns SAN then (deterministic - Botan
  // keeps SANs sorted). More than one CN stays an error.
  const auto cns = req->subject_dn().get_attribute("X520.CommonName");
  if (cns.size() > 1) {
    log::error("CSR subject must contain at most one CN (got {})", cns.size());
    return false;
  }
  std::string cn;
  if (cns.size() == 1) {
    cn = cns[0];
  } else if (!req->subject_alt_name().dns().empty()) {
    cn = *req->subject_alt_name().dns().begin();
    log::info("CSR has no subject CN; using dns SAN '{}' as the CN", cn);
  } else {
    log::error("CSR has neither a subject CN nor a dns SAN");
    return false;
  }
  if (req->subject_dn().count() > 1)
    log::warn("ignoring non-CN subject attributes in the CSR");

  // SANs: only dns/email/IPv4/URI are honored (same types as --san); the
  // remaining entry types (directoryName, IPv6, otherName) are not
  // implemented and are dropped.
  const auto &csr_san = req->subject_alt_name();
  const std::size_t supported = csr_san.dns().size() + csr_san.email().size() +
                                csr_san.ipv4_address().size() +
                                csr_san.uris().size();
  if (csr_san.count() > supported)
    log::warn("ignoring {} SAN entries in the CSR; only dns, email, IPv4 "
              "and URI are implemented",
              csr_san.count() - supported);
  for (const auto &d : csr_san.dns())
    if (!dns_safe(d)) {
      log::error("CSR dns SAN must be an ASCII hostname (IDN: use punycode): "
                 "{}",
                 d);
      return false;
    }
  for (const auto &m : csr_san.email())
    if (!ascii_graphic(m)) {
      log::error("CSR email SAN must be printable ASCII (IA5String): {}", m);
      return false;
    }
  if (csr_san.uris().size() > 1) {
    log::error("CSR carries {} URI SANs; at most one is allowed (an "
               "X509-SVID carries exactly one)",
               csr_san.uris().size());
    return false;
  }
  for (const auto &u : csr_san.uris())
    if (!valid_uri_san(u))
      return false;

  Botan::AlternativeName an;
  if (profile == Profile::Server) {
    // Same CN rules as issue_ee: hostname CN, always present as DNS:CN (the
    // set-backed AlternativeName dedups if the CSR listed it too).
    if (!dns_safe(cn)) {
      log::error("server CN must be an ASCII hostname [A-Za-z0-9.*-]: '{}'",
                 cn);
      return false;
    }
    an.add_dns(cn);
  } else if (supported == 0) {
    log::error("client profile requires at least one SAN in the CSR");
    return false;
  }
  for (const auto &d : csr_san.dns())
    an.add_dns(d);
  for (const auto &m : csr_san.email())
    an.add_email(m);
  for (uint32_t ip : csr_san.ipv4_address())
    an.add_ipv4_address(ip);
  for (const auto &u : csr_san.uris())
    an.add_uri(u);

  const fs::path db = store_path(store_dir);
  if (!fs::exists(db)) {
    log::error("not initialized; run '{} init'", app::name);
    return false;
  }
  if (secret.empty()) {
    log::error("{} not set (required to sign with the CA)", secret_env(config));
    return false;
  }

  Botan::AutoSeeded_RNG rng;
  auto dbh = open_store(db);
  Botan::Certificate_Store_In_SQL store(dbh, secret, rng);

  const CaGen sign = active_ca(*dbh, config, "signing");
  auto sign_cert = load_ca_cert(store_dir, sign.slug);
  if (!sign_cert) {
    log::error("signing CA cert not found under {}",
               (store_dir / "ca").string());
    return false;
  }
  const std::chrono::seconds validity = valid_override.value_or(
      std::chrono::seconds(std::chrono::days(config.ee_valid_days)));
  if (outlives_issuer(*sign_cert, validity))
    return false;

  // Fail fast before the signing key lookup (seconds on NK HSM): nonce and
  // uniqueness are answerable from the DB alone. The write lock below
  // repeats both checks authoritatively.
  ensure_cert_index(*dbh);
  ensure_enrollment(*dbh);
  auto nonce_rejected = [&] {
    auto q = dbh->new_statement("SELECT IFNULL(nonce,''), issued, validity, "
                                "consumed FROM enrollment WHERE id=?1");
    q->bind(1, id);
    if (!q->step()) {
      log::error("identity '{}' is not enrolled", id);
      return true;
    }
    const std::string want = q->get_str(0);
    const std::size_t issued = q->get_size_t(1);
    const std::size_t validity_min = q->get_size_t(2);
    const bool consumed = q->get_size_t(3) != 0;
    // One combined error: no oracle about which of the checks failed.
    if (want.empty() || consumed || now_epoch() >= issued + validity_min * 60 ||
        nonce != want) {
      log::error("invalid, expired or consumed nonce for id '{}'", id);
      return true;
    }
    return false;
  };
  auto duplicate = [&] {
    return blocking_duplicate(*dbh, cn, profile_name(profile));
  };
  if (nonce_rejected() || duplicate())
    return false;

  std::optional<p11::Token> token; // owns the session; must outlive sign_key
  std::shared_ptr<const Botan::Private_Key> sign_key;
  try {
    sign_key = signing_key(config, store, *sign_cert, sign.slug, secret, token);
  } catch (const std::exception &e) {
    log::error("cannot load signing key (wrong {}?): {}",
               config.key_backend == "internal" ? "passphrase" : "user PIN",
               e.what());
    return false;
  }
  if (!sign_key) {
    log::error("signing key missing from store");
    return false;
  }

  // Nonce check, uniqueness check, insert and nonce consumption share one
  // write lock: a nonce cannot be double-spent and a concurrent create for
  // the same (cn, profile) cannot slip in between.
  begin_write(*dbh);
  ensure_cert_index(*dbh);
  ensure_enrollment(*dbh);
  if (nonce_rejected() || duplicate())
    return false;

  // The CA dictates every extension - nothing is copied from the CSR. Botan's
  // X509_CA::sign_request would honor requested extensions (including
  // basicConstraints CA:true), so the cert is built via make_cert with a
  // policy-owned extension set instead. Same profile as issue_ee.
  Botan::Extensions ext;
  ext.add_new(std::make_unique<Botan::Cert_Extension::Basic_Constraints>(false),
              true);
  ext.add_new(
      std::make_unique<Botan::Cert_Extension::Key_Usage>(Botan::Key_Constraints(
          static_cast<uint32_t>(Botan::Key_Constraints::DigitalSignature))),
      true);
  ext.add_new(std::make_unique<Botan::Cert_Extension::Extended_Key_Usage>(
      std::vector<Botan::OID>{Botan::OID(profile == Profile::Server
                                             ? "1.3.6.1.5.5.7.3.1"
                                             : "1.3.6.1.5.5.7.3.2")}));
  ext.add_new(std::make_unique<Botan::Cert_Extension::Authority_Key_ID>(
      sign_cert->subject_key_id()));
  ext.add_new(std::make_unique<Botan::Cert_Extension::Subject_Key_ID>(
      req->raw_public_key(), config.ee_digest));
  ext.add_new(
      std::make_unique<Botan::Cert_Extension::Subject_Alternative_Name>(an));
  add_ee_pointer_extensions(ext, config, profile, sign.slug);

  Botan::X509_DN subject;
  subject.add_attribute("X520.CommonName", cn); // leaf DN: CN only

  Botan::X509_CA issuer(*sign_cert, *sign_key, config.ee_digest, rng);
  const auto tp = Clock::now();
  auto ee_cert = Botan::X509_CA::make_cert(
      issuer.signature_op(), rng, issuer.algorithm_identifier(),
      req->raw_public_key(), Botan::X509_Time(tp),
      Botan::X509_Time(tp + validity), sign_cert->subject_dn(), subject, ext);

  store.insert_cert(ee_cert);
  index_cert(*dbh, ee_cert, profile_name(profile));
  auto consume =
      dbh->new_statement("UPDATE enrollment SET consumed=1 WHERE id=?1");
  consume->bind(1, id);
  consume->spin();
  commit_write(*dbh);

  // No <store>/ee/ artifacts: the CA never sees the requester's private key,
  // and delivery is `get <profile> --cn`. stdout carries just the CN so the
  // output pipes into it.
  log::info("issued {} certificate for CN '{}' from CSR (requested by '{}')",
            profile_name(profile), cn, id);
  log::to_stdout("{}", cn);
  return true;
}

bool revoke(const cfg::Config &config, const fs::path &store_dir,
            std::string_view secret, const std::string &target,
            const std::string &cn, const std::string &reason_str,
            const std::string &serial) {
  const auto reason = parse_reason(reason_str);
  if (!reason) {
    log::error("unknown --reason '{}'", reason_str);
    return false;
  }

  const fs::path db = store_path(store_dir);
  if (!fs::exists(db)) {
    log::error("not initialized; run '{} init'", app::name);
    return false;
  }
  if (secret.empty()) {
    log::error("{} not set (required to sign the CRL)", secret_env(config));
    return false;
  }

  Botan::AutoSeeded_RNG rng;
  auto dbh = open_store(db);
  Botan::Certificate_Store_In_SQL store(dbh, secret, rng);

  // Fail fast before the signing key lookup (seconds on NK HSM): whether an
  // active cert exists is answerable from cert_index alone. The write lock
  // below repeats the lookup authoritatively.
  const std::string &subject_cn = cn;
  const std::string sel = normalize_serial(serial);
  auto lookup = [&](Botan::SQL_Database &d) {
    return sel.empty() ? active_fp(d, subject_cn, target)
                       : active_fp_by_serial(d, sel, target);
  };
  auto not_found = [&] {
    if (sel.empty())
      log::error("no active {} certificate for CN '{}'", target, subject_cn);
    else
      log::error("no active {} certificate with serial {}", target, sel);
  };
  const auto first_fp = lookup(*dbh);
  if (!first_fp) {
    not_found();
    return false;
  }

  // The CRL to rewrite is the ISSUING generation's, not whichever signs
  // today: after a rotation the two differ for everything issued before
  // it, and an entry on the wrong CRL revokes nothing.
  auto issued_by = load_cert(*dbh, *first_fp);
  if (!issued_by) {
    log::error("certificate {} is indexed but missing from the store",
               *first_fp);
    return false;
  }
  const std::string issuer_cn =
      issued_by->issuer_dn().get_first_attribute("X520.CommonName");
  CaGen sign = active_ca(*dbh, config, "signing");
  if (auto g = gen_by_cn(*dbh, "signing", issuer_cn)) {
    sign = *g;
  } else if (issuer_cn != sign.cn) {
    log::error("no CA generation matches the issuer of this certificate "
               "(issuer CN '{}')",
               issuer_cn);
    return false;
  }

  auto sign_cert = load_ca_cert(store_dir, sign.slug);
  if (!sign_cert) {
    log::error("CA certificate not found for {} under {}", sign.slug,
               (store_dir / "ca").string());
    return false;
  }

  const fs::path crl_path = store_dir / "ca" / (sign.slug + ".crl");
  if (!fs::exists(crl_path)) {
    log::error("CRL not found at {}", crl_path.string());
    return false;
  }

  std::optional<p11::Token> token; // must outlive sign_key (owns the session)
  std::shared_ptr<const Botan::Private_Key> sign_key;
  try {
    sign_key = signing_key(config, store, *sign_cert, sign.slug, secret, token);
  } catch (const std::exception &e) {
    log::error("cannot load signing key (wrong {}?): {}",
               config.key_backend == "internal" ? "passphrase" : "user PIN",
               e.what());
    return false;
  }
  if (!sign_key) {
    log::error("signing key missing from store");
    return false;
  }

  // Lock before reading the CRL: concurrent revokes serialize on the DB write
  // lock, so read-CRL -> write-CRL -> update-index cannot lose an entry.
  begin_write(*dbh);
  ensure_cert_index(*dbh);
  Botan::X509_CRL prev(crl_path.string());

  auto fp = lookup(*dbh);
  auto target_cert = fp ? load_cert(*dbh, *fp) : std::nullopt;
  if (!target_cert) {
    not_found();
    return false;
  }

  Botan::X509_CA ca(*sign_cert, *sign_key, config.signing_ca_digest, rng);
  // Carry forward only the unexpired entries (RFC 5280 3.3 pruning), plus
  // the new one; make_crl continues the crlNumber that update_crl would.
  auto entries = prune_crl_entries(*dbh, prev);
  const std::size_t pruned = prev.get_revoked().size() - entries.size();
  entries.emplace_back(*target_cert, *reason);
  const auto updated = ca.make_crl(
      entries, prev.crl_number() + 1, rng, std::chrono::system_clock::now(),
      std::chrono::seconds(
          crl_next_update(*sign_cert, app::crl_next_update_days)));
  if (!write_der(crl_path, updated)) {
    log::error("could not write CRL {}", crl_path.string());
    return false;
  }

  // Keep the Botan store's own revocation state in sync (its `revoked` table
  // feeds generate_crls()); cert_index stays the query path.
  store.revoke_cert(*target_cert, *reason);
  auto u = dbh->new_statement("UPDATE cert_index SET status='revoked', "
                              "revoked_at=?1, reason=?2 WHERE fingerprint=?3");
  u->bind(1, now_epoch());
  u->bind(2, static_cast<std::size_t>(*reason));
  u->bind(3, *fp);
  u->spin();
  commit_write(*dbh);

  log::info("revoked {} certificate for CN '{}' (serial {}, reason: {})",
            target,
            target_cert->subject_dn().get_first_attribute("X520.CommonName"),
            Botan::hex_encode(target_cert->serial_number()), reason_str);
  if (pruned)
    log::info("pruned {} expired entries from CRL #{}", pruned,
              updated.crl_number());
  return true;
}

bool revoke_ca(const cfg::Config &config, const fs::path &store_dir,
               std::string_view secret, const std::string &selector,
               const std::string &reason_str) {
  const auto reason = parse_reason(reason_str);
  if (!reason) {
    log::error("unknown --reason '{}'", reason_str);
    return false;
  }
  const fs::path db = store_path(store_dir);
  if (!fs::exists(db)) {
    log::error("not initialized; run '{} init'", app::name);
    return false;
  }
  if (secret.empty()) {
    log::error("{} not set (the root key signs its CRL)", secret_env(config));
    return false;
  }
  if (selector.empty()) {
    log::error("revoke ca needs --cn (a signing CA generation, or "
               "signing-ca for the active one)");
    return false;
  }

  Botan::AutoSeeded_RNG rng;
  auto dbh = open_store(db);
  Botan::Certificate_Store_In_SQL store(dbh, secret, rng);
  ensure_ca_index(*dbh, config);

  const CaGen root = active_ca(*dbh, config, "root");
  const CaGen active = active_ca(*dbh, config, "signing");

  // A self-signed root cannot be revoked by anything below it: dropping a
  // trust anchor is the relying parties' job, not the CA's.
  if (selector == "root-ca" || selector == root.cn) {
    log::error("the root cannot be revoked; remove it from the trust stores "
               "and re-initialize");
    return false;
  }
  const std::string cn = selector == "signing-ca" ? active.cn : selector;
  const auto victim = gen_by_cn(*dbh, "signing", cn);
  if (!victim) {
    log::error("no signing CA generation named '{}'", cn);
    return false;
  }
  // Revoking the issuer of record would leave nothing to issue with, and
  // the successor is one command away with the root key already in hand.
  if (victim->gen == active.gen) {
    log::error("'{}' is the active issuer; run '{} renew signing-ca "
               "--new-cn <name>' first, then revoke it",
               cn, app::name);
    return false;
  }

  auto root_cert = load_ca_cert(store_dir, root.slug);
  auto victim_cert = load_ca_cert(store_dir, victim->slug);
  if (!root_cert || !victim_cert) {
    log::error("CA certificates not found under {}",
               (store_dir / "ca").string());
    return false;
  }
  const std::string victim_fp = victim_cert->fingerprint("SHA-256");
  auto already = dbh->new_statement(
      "SELECT 1 FROM cert_index WHERE fingerprint=?1 AND status='revoked'");
  already->bind(1, victim_fp);
  if (already->step()) {
    log::error("'{}' is already revoked", cn);
    return false;
  }
  const fs::path root_crl_path = store_dir / "ca" / (root.slug + ".crl");
  if (!fs::exists(root_crl_path)) {
    log::error("root CRL not found at {}", root_crl_path.string());
    return false;
  }

  std::optional<p11::Token> token; // must outlive root_key (owns the session)
  std::shared_ptr<const Botan::Private_Key> root_key;
  try {
    root_key = ca_key(config, store, *root_cert, root.slug,
                      config.root_ca_curve, secret, token);
  } catch (const std::exception &e) {
    log::error("cannot load the root key (wrong {}?): {}",
               config.key_backend == "internal" ? "passphrase" : "user PIN",
               e.what());
    return false;
  }
  if (!root_key) {
    log::error("root key missing from store");
    return false;
  }

  // Same lock discipline as revoke: read-CRL -> write-CRL under the write
  // lock. This is the entry that stops the root CRL being structurally
  // empty, and it carries a fresh nextUpdate of its own, so nothing needs
  // a separate root refresh afterwards - only publication.
  begin_write(*dbh);
  ensure_cert_index(*dbh);
  Botan::X509_CRL prev(root_crl_path.string());
  Botan::X509_CA root_ca(*root_cert, *root_key, config.root_ca_digest, rng);
  // Same RFC 5280 3.3 pruning as the signing CRL: an expired signing CA
  // generation leaves the root CRL after its final scheduled appearance.
  auto entries = prune_crl_entries(*dbh, prev);
  const std::size_t pruned = prev.get_revoked().size() - entries.size();
  entries.emplace_back(*victim_cert, *reason);
  const auto updated = root_ca.make_crl(
      entries, prev.crl_number() + 1, rng, std::chrono::system_clock::now(),
      std::chrono::seconds(
          crl_next_update(*root_cert, app::root_crl_next_update_days)));
  if (!write_der(root_crl_path, updated)) {
    log::error("could not write the root CRL {}", root_crl_path.string());
    return false;
  }

  store.revoke_cert(*victim_cert, *reason);
  auto u = dbh->new_statement("UPDATE cert_index SET status='revoked', "
                              "revoked_at=?1, reason=?2 WHERE fingerprint=?3");
  u->bind(1, now_epoch());
  u->bind(2, static_cast<std::size_t>(*reason));
  u->bind(3, victim_fp);
  u->spin();
  auto g = dbh->new_statement("UPDATE ca_cert_index SET status='revoked' "
                              "WHERE kind='signing' AND gen=?1");
  g->bind(1, static_cast<std::size_t>(victim->gen));
  g->spin();
  commit_write(*dbh);

  log::info("revoked signing CA generation {} '{}' (serial {}, reason: {}); "
            "root CRL #{} carries it",
            victim->gen, victim->cn,
            Botan::hex_encode(victim_cert->serial_number()), reason_str,
            updated.crl_number());
  if (pruned)
    log::info("pruned {} expired entries from the root CRL", pruned);
  log::to_stderr("publish the root CRL now: relying parties may keep serving "
                 "a cached one until its nextUpdate ({})",
                 updated.next_update().readable_string());
  return true;
}

bool refresh_crl(const cfg::Config &config, const fs::path &store_dir,
                 std::string_view secret, CrlScope scope) {
  const fs::path db = store_path(store_dir);
  if (!fs::exists(db)) {
    log::error("not initialized; run '{} init'", app::name);
    return false;
  }
  if (secret.empty()) {
    log::error("{} not set (required to sign the CRL)", secret_env(config));
    return false;
  }

  const bool do_root = scope != CrlScope::Signing;
  const bool do_sign = scope != CrlScope::Root;

  Botan::AutoSeeded_RNG rng;
  auto dbh = open_store(db);
  Botan::Certificate_Store_In_SQL store(dbh, secret, rng);
  const fs::path ca_dir = store_dir / "ca";

  // One job per CA whose CRL is re-signed. A rotation leaves retiring
  // generations behind and each keeps publishing its own CRL until the
  // last certificate it issued is gone, so the scope is a set, not one CA.
  struct Job {
    CaGen ca;
    std::string curve, digest;
    int horizon;
    Botan::X509_Certificate cert;
    std::shared_ptr<const Botan::Private_Key> key;
  };
  std::vector<Job> jobs;
  const std::size_t now = now_epoch();
  auto collect = [&](const std::string &kind, const std::string &curve,
                     const std::string &digest, int horizon) {
    for (const CaGen &g : live_cas(*dbh, config, kind)) {
      auto cert = load_ca_cert(store_dir, g.slug);
      const fs::path crl_path = ca_dir / (g.slug + ".crl");
      if (!cert || !fs::exists(crl_path)) {
        log::error("{} CA artifacts not found for {} under {}", kind, g.slug,
                   ca_dir.string());
        return false;
      }
      // An expired CA cannot promise a future publication (crl_next_update
      // clamps to its notAfter), so re-signing would only churn crlNumber.
      if (static_cast<std::size_t>(cert->not_after().time_since_epoch()) <=
          now) {
        log::info("skipping {}: expired {}", g.slug,
                  cert->not_after().readable_string());
        continue;
      }
      jobs.push_back({g, curve, digest, horizon, *cert, nullptr});
    }
    return true;
  };
  if (do_root && !collect("root", config.root_ca_curve, config.root_ca_digest,
                          app::root_crl_next_update_days))
    return false;
  if (do_sign && !collect("signing", config.signing_ca_curve,
                          config.signing_ca_digest, app::crl_next_update_days))
    return false;
  if (jobs.empty()) {
    log::warn("no CA in scope has a CRL to refresh");
    return true;
  }

  // Keys through one token session (backend pkcs11), loaded before the
  // write lock: a NK HSM lookup takes seconds and other writers wait at most
  // busy_timeout. The signing scope never touches the root key - that is
  // the point of the separate root cadence (root_crl_next_update_days vs
  // crl_next_update_days).
  std::optional<p11::Token> token;
  try {
    for (Job &j : jobs)
      j.key = ca_key(config, store, j.cert, j.ca.slug, j.curve, secret, token);
  } catch (const std::exception &e) {
    log::error("cannot load CA key (wrong {}?): {}",
               config.key_backend == "internal" ? "passphrase" : "user PIN",
               e.what());
    return false;
  }
  for (const Job &j : jobs)
    if (!j.key) {
      log::error("CA key missing from store: {}", j.ca.slug);
      return false;
    }

  // Same lock discipline as revoke: read-CRL -> write-CRL under the write
  // lock, so a refresh cannot lose a concurrent revocation's entry. The
  // refresh re-signs the unexpired revocation set (RFC 5280 3.3 pruning,
  // see ca.h crl_entry_prunable) with crlNumber+1 and a fresh
  // thisUpdate/nextUpdate.
  begin_write(*dbh);
  bool ok = true;
  std::string refreshed;
  for (const Job &j : jobs) {
    const fs::path crl_path = ca_dir / (j.ca.slug + ".crl");
    Botan::X509_CRL prev(crl_path.string());
    Botan::X509_CA ca(j.cert, *j.key, j.digest, rng);
    const auto kept = prune_crl_entries(*dbh, prev);
    const std::size_t pruned = prev.get_revoked().size() - kept.size();
    const auto next = ca.make_crl(
        kept, prev.crl_number() + 1, rng, std::chrono::system_clock::now(),
        std::chrono::seconds(crl_next_update(j.cert, j.horizon)));
    ok = write_der(crl_path, next) && ok;
    refreshed += std::format(
        "{}{} #{}{}", refreshed.empty() ? "" : ", ", j.ca.slug,
        next.crl_number(),
        pruned ? std::format(" (pruned {})", pruned) : std::string());
  }
  commit_write(*dbh);
  if (!ok) {
    log::error("could not write CRLs under {}", ca_dir.string());
    return false;
  }

  log::info("refreshed CRLs ({})", refreshed);
  return true;
}

bool get_cert(const cfg::Config &config, const fs::path &store_dir,
              const std::string &target, const std::string &selector,
              const std::string &encoding) {
  const fs::path db = store_path(store_dir);
  if (!fs::exists(db)) {
    log::error("not initialized; run '{} init'", app::name);
    return false;
  }

  // `config` is the effective config.
  if (target == "config") {
    std::print("# showing configuration stored in database\n");
    std::print("org_name = \"{}\"\n", config.org_name);
    std::print("country_code = \"{}\"\n", config.country_code);
    std::print("repository_host = \"{}\"\n", config.repository_host);
    std::print("root_ca_cn = \"{}\"\n", config.root_ca_cn);
    std::print("root_ca_curve = \"{}\"\n", config.root_ca_curve);
    std::print("root_ca_digest = \"{}\"\n", config.root_ca_digest);
    std::print("root_ca_valid_days = {}\n", config.root_ca_valid_days);
    std::print("root_ca_slug_prefix = \"{}\"\n", config.root_ca_slug_prefix);
    std::print("root_ca_slug = \"{}\"\n", config.root_ca_slug);
    std::print("signing_ca_cn = \"{}\"\n", config.signing_ca_cn);
    std::print("signing_ca_curve = \"{}\"\n", config.signing_ca_curve);
    std::print("signing_ca_digest = \"{}\"\n", config.signing_ca_digest);
    std::print("signing_ca_valid_days = {}\n", config.signing_ca_valid_days);
    std::print("signing_ca_slug_prefix = \"{}\"\n",
               config.signing_ca_slug_prefix);
    std::print("signing_ca_slug = \"{}\"\n", config.signing_ca_slug);
    std::print("ee_curve = \"{}\"\n", config.ee_curve);
    std::print("ee_digest = \"{}\"\n", config.ee_digest);
    std::print("ee_valid_days = {}\n", config.ee_valid_days);
    if (!config.root_arc_oid.empty())
      std::print("root_arc_oid = \"{}\"\n", config.root_arc_oid);
    std::print("key_backend = \"{}\"\n", config.key_backend);
    if (config.key_backend == "pkcs11") {
      std::print("pkcs11_module = \"{}\"\n", config.pkcs11_module);
      std::print("pkcs11_token_label = \"{}\"\n", config.pkcs11_token_label);
    }
    return true;
  }

  // A CRL is a file artifact; the file is named by the CA slug, guaranteed
  // to be ASCII. The aliases follow the active generation; an explicit CN
  // reaches whichever generation carries it.
  if (target == "crl") {
    auto dbh_crl = open_store(db);
    const CaGen root = active_ca(*dbh_crl, config, "root");
    const CaGen sign = active_ca(*dbh_crl, config, "signing");
    std::string ca_slug;
    if (selector == "root-ca" || selector == root.cn)
      ca_slug = root.slug;
    else if (selector == "signing-ca" || selector == sign.cn)
      ca_slug = sign.slug;
    else {
      log::error("crl --cn must be root-ca or signing-ca");
      return false;
    }
    const fs::path crl_path = store_dir / "ca" / (ca_slug + ".crl");
    if (!fs::exists(crl_path)) {
      log::error("CRL not found: {}", crl_path.string());
      return false;
    }
    emit(Botan::X509_CRL(crl_path.string()), encoding);
    return true;
  }

  auto dbh = open_store(db);

  // Resolve to a single fingerprint via cert_index, then load the DER.
  std::optional<std::string> fp;
  if (target == "ca") {
    std::string cn = selector, kind;
    if (selector == "root-ca") {
      cn = active_ca(*dbh, config, "root").cn;
      kind = "root";
    } else if (selector == "signing-ca") {
      cn = active_ca(*dbh, config, "signing").cn;
      kind = "signing";
    }
    auto q = dbh->new_statement(
        kind.empty()
            ? "SELECT fingerprint FROM cert_index WHERE cn=?1 AND "
              "kind IN ('root','signing') LIMIT 1"
            : "SELECT fingerprint FROM cert_index WHERE cn=?1 AND kind=?2 "
              "LIMIT 1");
    q->bind(1, cn);
    if (!kind.empty())
      q->bind(2, kind);
    if (q->step())
      fp = q->get_str(0);
  } else {
    fp = active_fp(*dbh, selector, target);
  }

  auto found = fp ? load_cert(*dbh, *fp) : std::nullopt;
  if (!found) {
    log::error("no matching {} certificate for '{}'", target, selector);
    return false;
  }

  emit(*found, encoding);
  return true;
}

bool list_certs(const fs::path &store_dir, const std::string &filter, int days,
                const std::string &cn, bool tsv, int limit) {
  const fs::path db = store_path(store_dir);
  if (!fs::exists(db)) {
    log::error("not initialized; run '{} init'", app::name);
    return false;
  }
  auto dbh = open_store(db);

  const std::size_t now = now_epoch();
  const std::size_t win = util::days_to_seconds(days);
  const std::string cols =
      "SELECT cn,kind,serial,not_after,status FROM cert_index ";
  // One row past the cap detects truncation without a COUNT(*) pass; the
  // ORDER BY of every filter rides an index, so the query stops early.
  const std::string lim = limit > 0 ? std::format(" LIMIT {}", limit + 1) : "";
  std::shared_ptr<Botan::SQL_Database::Statement> q;
  if (filter == "expiring") {
    q = dbh->new_statement(cols +
                           "WHERE status='active' AND not_after>?1 AND "
                           "not_after<=?2 ORDER BY not_after DESC" +
                           lim);
    q->bind(1, now);
    q->bind(2, now + win);
  } else if (filter == "expired") {
    q = dbh->new_statement(cols +
                           "WHERE status='active' AND not_after<?1 AND "
                           "not_after>=?2 ORDER BY not_after DESC" +
                           lim);
    q->bind(1, now);
    q->bind(2, now - win);
  } else if (filter == "revoked") {
    q = dbh->new_statement(cols +
                           "WHERE status='revoked' AND revoked_at>=?1 "
                           "ORDER BY revoked_at DESC" +
                           lim);
    q->bind(1, now - win);
  } else if (filter == "last") {
    q = dbh->new_statement(cols +
                           "WHERE not_before<=?1 AND not_before>=?2 "
                           "ORDER BY not_before DESC" +
                           lim);
    q->bind(1, now);
    q->bind(2, now - win);
  } else { // cn
    // A CA alias selects the role, so every generation of it is listed; a
    // literal CN selects exactly that name.
    if (cn == "root-ca" || cn == "signing-ca") {
      q = dbh->new_statement(cols + "WHERE kind=?1 ORDER BY not_before DESC" +
                             lim);
      q->bind(1, std::string(cn == "root-ca" ? "root" : "signing"));
    } else {
      q = dbh->new_statement(cols + "WHERE cn=?1 ORDER BY not_before DESC" +
                             lim);
      q->bind(1, cn);
    }
  }

  struct Row {
    std::string cn, kind, serial, expires, status;
  };
  std::vector<Row> rows;
  while (q->step()) {
    const std::string serial = q->get_str(2);
    const std::size_t na = q->get_size_t(3);
    const std::string st = q->get_str(4);
    rows.push_back(
        {q->get_str(0), q->get_str(1),
         serial.size() > app::list_serial_hex
             ? serial.substr(0, app::list_serial_hex)
             : serial,
         fmt_epoch(na),
         st == "revoked" ? "revoked" : (na < now ? "expired" : "active")});
  }
  bool truncated = false;
  if (limit > 0 && rows.size() > static_cast<std::size_t>(limit)) {
    rows.resize(static_cast<std::size_t>(limit));
    truncated = true;
  }

  if (tsv) {
    std::print("CN\tKIND\tSERIAL\tEXPIRES\tSTATUS\n");
    for (const auto &r : rows)
      std::print("{}\t{}\t{}\t{}\t{}\n", r.cn, r.kind, r.serial, r.expires,
                 r.status);
  } else {
    std::size_t wc = 2, wk = 4, ws = 6, we = 7;
    for (const auto &r : rows) {
      wc = std::max(wc, r.cn.size());
      wk = std::max(wk, r.kind.size());
      ws = std::max(ws, r.serial.size());
      we = std::max(we, r.expires.size());
    }
    std::print("{:<{}}  {:<{}}  {:<{}}  {:<{}}  {}\n", "CN", wc, "KIND", wk,
               "SERIAL", ws, "EXPIRES", we, "STATUS");
    for (const auto &r : rows)
      std::print("{:<{}}  {:<{}}  {:<{}}  {:<{}}  {}\n", r.cn, wc, r.kind, wk,
                 r.serial, ws, r.expires, we, r.status);
  }
  if (truncated)
    log::to_stderr("(truncated at {} rows)", limit);
  return true;
}

} // namespace ca

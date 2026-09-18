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
#include "profile.h"
#include "store.h"
#include "util.h"
#include "x509ext.h"

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
void begin_write(store::Database &db) {
  // WAL: a long-running reader never blocks a CA write
  // and vice versa.
  db.stmt("PRAGMA journal_mode=WAL")->spin();
  db.stmt("BEGIN IMMEDIATE")->spin();
}

void commit_write(store::Database &db) { db.stmt("COMMIT")->spin(); }

} // namespace

// Definitions for ca_detail.h - shared with the load-test seeder (seed.cpp).
namespace detail {

std::filesystem::path store_path(const std::filesystem::path &store_dir) {
  return store_dir / app::store_file;
}

std::shared_ptr<store::Database> open_store(const fs::path &db) {
  auto dbh = std::make_shared<store::Database>(db.string());
  dbh->stmt(std::format("PRAGMA busy_timeout = {}", app::store_busy_timeout_ms))
      ->spin();
  return dbh;
}

Botan::X509_DN subject_dn(const cfg::Pki &pki, const std::string &cn,
                          bool simple) {
  Botan::X509_DN dn;
  if (!simple) {
    // add_attribute ignores an empty value, so an unset field would simply
    // be absent; both are required by the config loader.
    dn.add_attribute("X520.Country", pki.country_code);
    dn.add_attribute("X520.Organization", pki.org_name);
  }
  dn.add_attribute("X520.CommonName", cn);
  return dn;
}

std::optional<Botan::X509_Certificate> load_ca_cert(const fs::path &store_dir,
                                                    const std::string &slug) {
  const fs::path p = store_dir / "ca" / (slug + ".pem");
  if (!fs::exists(p))
    return std::nullopt;
  return Botan::X509_Certificate(p.string());
}

// The generation-1 identity, straight from the locked config.
CaGen config_gen(const cfg::Config &config, const std::string &purpose) {
  if (purpose == "root")
    return CaGen{"root", 1, config.root.cn, config.root.slug};
  if (const cfg::SigningCa *ca = config.ca(purpose))
    return CaGen{purpose, 1, ca->cn, ca->slug};
  // A purpose the config does not describe: name it and let the caller
  // fail on the missing artifacts, rather than quietly using another CA.
  return CaGen{purpose, 1, "", ""};
}

std::string kind_of(const std::string &purpose) {
  return purpose == "root" ? "root" : "signing";
}

bool has_table(store::Database &db, const std::string &name) {
  auto s =
      db.stmt("SELECT 1 FROM sqlite_master WHERE type='table' AND name=?1");
  s->bind(1, name);
  return s->step();
}

bool has_ca_index(store::Database &db) {
  return has_table(db, "ca_cert_index");
}

void ensure_ca_index(store::Database &db, const cfg::Config &config) {
  db.create_table("CREATE TABLE IF NOT EXISTS ca_cert_index ("
                  "kind TEXT NOT NULL, purpose TEXT NOT NULL, "
                  "gen INTEGER NOT NULL, cn TEXT NOT NULL, "
                  "slug TEXT NOT NULL, status TEXT NOT NULL, "
                  "PRIMARY KEY (kind, purpose, gen))");
  db.stmt("CREATE INDEX IF NOT EXISTS cai_kps ON "
          "ca_cert_index(kind, purpose, status)")
      ->spin();
  // Generation 1 of every CA the ceremony created; record it so later
  // generations have a predecessor to succeed. Backfilled only for CAs
  // whose certificate the store actually holds: the file may declare a
  // purpose that no ceremony has created yet (see add_signing_ca), and
  // that one must not appear here as an active generation.
  const bool indexed = has_table(db, "cert_index");
  std::vector<std::string> purposes{"root"};
  for (const auto &[purpose, ca] : config.cas)
    purposes.push_back(purpose);
  for (const std::string &purpose : purposes) {
    auto q = db.stmt("SELECT 1 FROM ca_cert_index WHERE purpose=?1");
    q->bind(1, purpose);
    if (q->step())
      continue;
    const CaGen g = config_gen(config, purpose);
    if (!indexed)
      continue;
    auto issued = db.stmt("SELECT 1 FROM cert_index WHERE cn=?1 AND "
                          "kind IN ('root','signing') LIMIT 1");
    issued->bind(1, g.cn);
    if (!issued->step())
      continue;
    auto ins = db.stmt("INSERT INTO ca_cert_index "
                       "(kind,purpose,gen,cn,slug,status) "
                       "VALUES (?1,?2,?3,?4,?5,'active')");
    ins->bind(1, kind_of(purpose));
    ins->bind(2, purpose);
    ins->bind(3, static_cast<std::size_t>(g.gen));
    ins->bind(4, g.cn);
    ins->bind(5, g.slug);
    ins->spin();
  }
}

std::optional<CaGen> gen_by_cn(store::Database &db, const std::string &kind,
                               const std::string &cn) {
  if (!has_ca_index(db))
    return std::nullopt;
  auto q = db.stmt("SELECT purpose,gen,cn,slug FROM ca_cert_index "
                   "WHERE kind=?1 AND cn=?2 LIMIT 1");
  q->bind(1, kind);
  q->bind(2, cn);
  if (!q->step())
    return std::nullopt;
  return CaGen{q->get_str(0), static_cast<int>(q->get_size_t(1)), q->get_str(2),
               q->get_str(3)};
}

std::vector<CaGen> live_cas(store::Database &db, const cfg::Config &config,
                            const std::string &kind) {
  std::vector<CaGen> out;
  if (has_ca_index(db)) {
    // Ordered by purpose then generation: a scope covering several issuing
    // CAs walks each one's lineage oldest first.
    auto q = db.stmt("SELECT purpose,gen,cn,slug FROM ca_cert_index "
                     "WHERE kind=?1 AND status IN "
                     "('active','retiring') ORDER BY purpose, gen");
    q->bind(1, kind);
    while (q->step())
      out.push_back({q->get_str(0), static_cast<int>(q->get_size_t(1)),
                     q->get_str(2), q->get_str(3)});
  }
  if (out.empty()) {
    if (kind == "root")
      out.push_back(config_gen(config, "root"));
    else
      for (const auto &[purpose, ca] : config.cas)
        out.push_back(config_gen(config, purpose));
  }
  return out;
}

CaGen active_ca(store::Database &db, const cfg::Config &config,
                const std::string &purpose) {
  if (!has_ca_index(db))
    return config_gen(config, purpose);
  auto q = db.stmt("SELECT purpose,gen,cn,slug FROM ca_cert_index "
                   "WHERE purpose=?1 AND status='active' "
                   "ORDER BY gen DESC LIMIT 1");
  q->bind(1, purpose);
  if (!q->step())
    return config_gen(config, purpose);
  return CaGen{q->get_str(0), static_cast<int>(q->get_size_t(1)), q->get_str(2),
               q->get_str(3)};
}

void ensure_cert_index(store::Database &db) {
  db.create_table(
      "CREATE TABLE IF NOT EXISTS cert_index ("
      "fingerprint TEXT PRIMARY KEY, cn TEXT NOT NULL, kind TEXT NOT NULL, "
      "serial TEXT NOT NULL, not_before INTEGER NOT NULL, "
      "not_after INTEGER NOT NULL, status TEXT NOT NULL, "
      "revoked_at INTEGER NOT NULL DEFAULT 0, "
      "reason INTEGER NOT NULL DEFAULT 0, "
      "purpose TEXT NOT NULL DEFAULT '', "
      // Subject/authority key identifiers: the issuer link a chain walk
      // follows. Empty when the certificate carries no such extension.
      "ski TEXT NOT NULL DEFAULT '', "
      "aki TEXT NOT NULL DEFAULT '')");
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
        "CREATE INDEX IF NOT EXISTS ci_knb ON cert_index(kind, not_before)",
        "CREATE INDEX IF NOT EXISTS ci_ski ON cert_index(ski)"})
    db.stmt(ix)->spin();
}

void index_cert(store::Database &db, const Botan::X509_Certificate &c,
                const std::string &kind, const std::string &purpose,
                const std::string &status, std::size_t revoked_at) {
  auto s = db.stmt(
      "INSERT OR REPLACE INTO cert_index "
      "(fingerprint,cn,kind,serial,not_before,not_after,status,revoked_at,"
      "ski,aki,purpose) "
      "VALUES (?1,?2,?3,?4,?5,?6,?7,?8,?9,?10,?11)");
  s->bind(1, c.fingerprint("SHA-256"));
  s->bind(2, c.subject_dn().get_first_attribute("X520.CommonName"));
  s->bind(3, kind);
  s->bind(4, Botan::hex_encode(c.serial_number()));
  s->bind(5, static_cast<std::size_t>(c.not_before().time_since_epoch()));
  s->bind(6, static_cast<std::size_t>(c.not_after().time_since_epoch()));
  s->bind(7, status);
  s->bind(8, revoked_at);
  s->bind(9, Botan::hex_encode(c.subject_key_id()));
  s->bind(10, Botan::hex_encode(c.authority_key_id()));
  s->bind(11, purpose);
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
  log::error("requested validity ends after the signing CA expires ({})",
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
std::vector<Botan::CRL_Entry> prune_crl_entries(store::Database &db,
                                                const Botan::X509_CRL &prev) {
  const auto tu =
      static_cast<std::size_t>(prev.this_update().time_since_epoch());
  std::unordered_set<std::string> prunable;
  auto q = db.stmt(
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

// The extensions every CA certificate carries. path_limit: a value sets the
// BasicConstraints path length, nullopt means CA:TRUE with none, as real
// roots do. `issuer` is the CA signing this certificate, nullptr for the
// self-signed root, whose authorityKeyIdentifier is its own subject one.
//
// Built as an extension set rather than an X509_Cert_Options because the
// options struct also owns the subject DN and orders it CN-first; every
// certificate here is minted through X509_CA::make_cert instead, which
// takes the DN detail::subject_dn builds.
Botan::Extensions ca_extensions(const std::vector<uint8_t> &pub_key,
                                const std::string &digest,
                                std::optional<std::size_t> path_limit,
                                const Botan::X509_Certificate *issuer) {
  Botan::Extensions ext;
  ext.add_new(std::make_unique<Botan::Cert_Extension::Basic_Constraints>(
                  true, path_limit),
              true);
  ext.add_new(std::make_unique<Botan::Cert_Extension::Key_Usage>(
                  Botan::Key_Constraints::ca_constraints()),
              true);
  auto skid =
      std::make_unique<Botan::Cert_Extension::Subject_Key_ID>(pub_key, digest);
  ext.add_new(std::make_unique<Botan::Cert_Extension::Authority_Key_ID>(
      issuer ? issuer->subject_key_id() : skid->get_key_id()));
  ext.add_new(std::move(skid));
  return ext;
}

// AIA (caIssuers -> root), CDP (root CRL), and CertificatePolicies for
// the signing CA. Pointers reference the root (issuer of this cert). URLs are
// flat under repository_host; cert/CRL file names come from the root CA slug.
// Revocation is CRL-only in this PKI (no OCSP), so AIA carries no OCSP URI;
// the signing CA's status channel is the root CRL (CDP).
// Policies only when root_arc_oid is configured.
void add_signing_pointer_extensions(Botan::Extensions &ext,
                                    const cfg::Config &config,
                                    const std::string &root_slug) {
  const std::string base = "http://" + config.pki.repository_host;
  const std::string &root = root_slug;

  ext.add_new(
      std::make_unique<Botan::Cert_Extension::Authority_Information_Access>(
          std::vector<std::string>{},
          std::vector<std::string>{base + "/" + root + ".crt"}));

  Botan::AlternativeName cdp;
  cdp.add_uri(base + "/" + root + ".crl");
  using DP = Botan::Cert_Extension::CRL_Distribution_Points::Distribution_Point;
  ext.add_new(std::make_unique<Botan::Cert_Extension::CRL_Distribution_Points>(
      std::vector<DP>{DP(cdp)}));

  if (!config.pki.arc_oid.empty())
    ext.add_new(std::make_unique<Botan::Cert_Extension::Certificate_Policies>(
        std::vector<Botan::OID>{Botan::OID(config.pki.arc_oid + ".1.1"),
                                Botan::OID(config.pki.arc_oid + ".1.2")}));
}

// EE pointers reference the issuing CA (issuer of the leaf), plus the one
// policy OID the profile carries. Revocation is CRL-only in this PKI (no
// OCSP), so AIA carries only caIssuers; the leaf's status channel is the
// issuing CRL (CDP).
void add_ee_pointer_extensions(Botan::Extensions &ext,
                               const cfg::Config &config,
                               const profile::Def &prof,
                               const std::string &issuer_slug) {
  const std::string base = "http://" + config.pki.repository_host;
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

  if (!config.pki.arc_oid.empty())
    ext.add_new(std::make_unique<Botan::Cert_Extension::Certificate_Policies>(
        std::vector<Botan::OID>{
            Botan::OID(config.pki.arc_oid + std::string(prof.policy_suffix))}));
}

// The key usage a profile's leaf carries: always digitalSignature, plus
// keyAgreement for a profile whose key also does ECDH.
Botan::Key_Constraints ee_constraints(const profile::Def &prof) {
  uint32_t bits =
      static_cast<uint32_t>(Botan::Key_Constraints::DigitalSignature);
  if (prof.key_agreement)
    bits |= static_cast<uint32_t>(Botan::Key_Constraints::KeyAgreement);
  return Botan::Key_Constraints(bits);
}

// Everything a leaf carries, for both issuance paths: `create`, which
// generates the key, and `sign`, which takes one from a CSR. The CA
// dictates every extension - on the CSR path nothing is copied from the
// request, and on the `create` path the certificate is built directly
// rather than round-tripped through a PKCS#10 request the CA would sign
// with a key it just generated itself.
//
// One function because the two lists must not drift: they are the same
// certificate shape, differing only in where the public key came from.
Botan::Extensions ee_extensions(const cfg::Config &config,
                                const profile::Def &prof,
                                const Botan::X509_Certificate &issuer,
                                const std::string &issuer_slug,
                                const std::vector<uint8_t> &pub_key,
                                const std::string &digest,
                                const Botan::AlternativeName &san) {
  Botan::Extensions ext;
  ext.add_new(std::make_unique<Botan::Cert_Extension::Basic_Constraints>(false),
              true);
  ext.add_new(
      std::make_unique<Botan::Cert_Extension::Key_Usage>(ee_constraints(prof)),
      true);
  ext.add_new(std::make_unique<Botan::Cert_Extension::Extended_Key_Usage>(
      std::vector<Botan::OID>{Botan::OID(std::string(prof.eku))}));
  ext.add_new(std::make_unique<Botan::Cert_Extension::Authority_Key_ID>(
      issuer.subject_key_id()));
  ext.add_new(
      std::make_unique<Botan::Cert_Extension::Subject_Key_ID>(pub_key, digest));
  ext.add_new(
      std::make_unique<Botan::Cert_Extension::Subject_Alternative_Name>(san));
  add_ee_pointer_extensions(ext, config, prof, issuer_slug);
  return ext;
}

// The profile a request names, or nullptr with the error logged. Unknown
// names cannot reach here from the CLI, but the API is public.
const profile::Def *profile_def(const std::string &name) {
  const profile::Def *p = profile::find(name);
  if (!p)
    log::error("unknown profile '{}'", name);
  return p;
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
std::optional<std::string> active_fp(store::Database &db, const std::string &cn,
                                     const std::string &kind) {
  auto q = db.stmt(
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
std::optional<std::string> active_fp_by_serial(store::Database &db,
                                               const std::string &serial,
                                               const std::string &kind) {
  auto q = db.stmt(
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
bool blocking_duplicate(store::Database &db, const std::string &cn,
                        const std::string &kind) {
  auto q =
      db.stmt("SELECT COUNT(*) FROM cert_index WHERE cn=?1 AND kind=?2 AND "
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
std::optional<Botan::X509_Certificate> load_cert(store::Database &db,
                                                 const std::string &fp) {
  auto q = db.stmt("SELECT certificate FROM certificates WHERE fingerprint=?1");
  q->bind(1, fp);
  if (!q->step())
    return std::nullopt;
  const auto [blob, len] = q->get_blob(0);
  return Botan::X509_Certificate(std::vector<uint8_t>(blob, blob + len));
}

std::string ski_of(const Botan::X509_Certificate &c) {
  return Botan::hex_encode(c.subject_key_id());
}

std::string aki_of(const Botan::X509_Certificate &c) {
  return Botan::hex_encode(c.authority_key_id());
}

// The issuers above `cert`, nearest first, stopping before the self-signed
// anchor: relying parties already hold that one, and RFC 8555 wants it left
// out of a served chain. Each step resolves a certificate's aki to the ski of
// its issuer, the way path building does.
//
// One ski means one certificate. Cross-signing puts two on the same key
// and the visited set is what bounds the walk; cross-certificates make the
// issuer a graph, not a tree.
std::vector<Botan::X509_Certificate>
issuers_above(store::Database &db, Botan::X509_Certificate cert) {
  std::vector<Botan::X509_Certificate> out;
  std::unordered_set<std::string> seen{cert.fingerprint("SHA-256")};
  for (;;) {
    const std::string aki = aki_of(cert);
    if (aki.empty() || aki == ski_of(cert)) // self-signed: nothing above it
      return out;
    auto q = db.stmt("SELECT fingerprint FROM cert_index WHERE ski=?1 "
                     "ORDER BY fingerprint LIMIT 1");
    q->bind(1, aki);
    if (!q->step())
      return out;
    const std::string fp = q->get_str(0);
    if (!seen.insert(fp).second)
      return out;
    auto issuer = load_cert(db, fp);
    if (!issuer || ski_of(*issuer) == aki_of(*issuer))
      return out;
    out.push_back(*issuer);
    cert = *issuer;
  }
}

using KeyValues = std::vector<std::pair<std::string, std::string>>;

std::string join_profiles(const std::vector<std::string> &profiles) {
  std::string out;
  for (const auto &p : profiles) {
    if (!out.empty())
      out += ',';
    out += p;
  }
  return out;
}

std::vector<std::string> split_profiles(const std::string &s) {
  std::vector<std::string> out;
  for (std::size_t i = 0; i < s.size();) {
    const std::size_t end = s.find(',', i);
    out.push_back(s.substr(i, end - i));
    if (end == std::string::npos)
      break;
    i = end + 1;
  }
  return out;
}

// The sections that exist once per store, recorded at init and enforced on
// every later run: locked, re-init to change any of them. Keys are dotted
// section paths so a warning names the section the operator has to look at.
KeyValues locked_global(const cfg::Config &c) {
  return {
      {"pki.org_name", c.pki.org_name},
      {"pki.country_code", c.pki.country_code},
      {"pki.repository_host", c.pki.repository_host},
      {"pki.arc_oid", c.pki.arc_oid},
      {"pkcs11.module", c.pkcs11.module},
      {"pkcs11.token_label", c.pkcs11.token_label},
      {"root.cn", c.root.cn},
      {"root.curve", c.root.curve},
      {"root.digest", c.root.digest},
      {"root.valid_days", std::to_string(c.root.valid_days)},
      {"root.slug_prefix", c.root.slug_prefix},
      {"root.slug", c.root.slug},
      {"root.key_backend", c.root.key_backend},
      {"root.token_label", c.root.token_label},
  };
}

// One issuing CA's locked fields. Stored as its own row rather than folded
// into the global snapshot, so a CA declared after init can be locked when
// it is created instead of forcing a re-init.
KeyValues locked_purpose(const cfg::SigningCa &ca) {
  return {
      {"profiles", join_profiles(ca.profiles)},
      {"cn", ca.cn},
      {"curve", ca.curve},
      {"digest", ca.digest},
      {"valid_days", std::to_string(ca.valid_days)},
      {"slug_prefix", ca.slug_prefix},
      {"slug", ca.slug},
      {"key_backend", ca.key_backend},
      {"token_label", ca.token_label},
      {"ee_curve", ca.ee_curve},
      {"ee_digest", ca.ee_digest},
      {"ee_valid_days", std::to_string(ca.ee_valid_days)},
      {"simple_dn", ca.simple_dn ? "1" : "0"},
      {"permitted_dns", join_profiles(ca.permitted_dns)},
      {"permitted_email", join_profiles(ca.permitted_email)},
  };
}

void ensure_purpose_table(store::Database &db) {
  db.create_table(std::format("CREATE TABLE IF NOT EXISTS {} ("
                              "purpose TEXT PRIMARY KEY, profiles TEXT NOT "
                              "NULL, cn TEXT NOT NULL, curve TEXT NOT NULL, "
                              "digest TEXT NOT NULL, valid_days INTEGER NOT "
                              "NULL, slug_prefix TEXT NOT NULL, slug TEXT NOT "
                              "NULL, key_backend TEXT NOT NULL, token_label "
                              "TEXT NOT NULL, ee_curve TEXT NOT NULL, "
                              "ee_digest TEXT NOT NULL, ee_valid_days INTEGER "
                              "NOT NULL, simple_dn INTEGER NOT NULL DEFAULT 0, "
                              "permitted_dns TEXT NOT NULL "
                              "DEFAULT '', permitted_email TEXT NOT NULL "
                              "DEFAULT '')",
                              app::purpose_table));
}

// Locks one issuing CA. Called for each CA the ceremony creates, and later
// for each one added to an initialized store.
void lock_purpose(store::Database &db, const cfg::SigningCa &ca) {
  auto ins = db.stmt(
      std::format("INSERT INTO {} (purpose,profiles,cn,curve,digest,valid_days,"
                  "slug_prefix,slug,key_backend,token_label,ee_curve,ee_digest,"
                  "ee_valid_days,simple_dn,permitted_dns,permitted_email) "
                  "VALUES (?1,?2,?3,?4,?5,?6,?7,?8,?9,?10,?11,?12,?13,?14,?15,"
                  "?16)",
                  app::purpose_table));
  ins->bind(1, ca.purpose);
  ins->bind(2, join_profiles(ca.profiles));
  ins->bind(3, ca.cn);
  ins->bind(4, ca.curve);
  ins->bind(5, ca.digest);
  ins->bind(6, static_cast<std::size_t>(ca.valid_days));
  ins->bind(7, ca.slug_prefix);
  ins->bind(8, ca.slug);
  ins->bind(9, ca.key_backend);
  ins->bind(10, ca.token_label);
  ins->bind(11, ca.ee_curve);
  ins->bind(12, ca.ee_digest);
  ins->bind(13, static_cast<std::size_t>(ca.ee_valid_days));
  ins->bind(14, static_cast<std::size_t>(ca.simple_dn ? 1 : 0));
  ins->bind(15, join_profiles(ca.permitted_dns));
  ins->bind(16, join_profiles(ca.permitted_email));
  ins->spin();
}

// Which CA a key operation is about: the parameters that stay constant
// across that CA's generations (see the layout matrix in config.cpp). The
// per-generation identity is CaGen, whose slug is both the artifact base
// name and the on-token key label, so it stays a separate argument - one
// spec spans every generation of its CA. The root is a spec like any
// other; `root` selects which secret and which token label it uses.
struct CaSpec {
  std::string purpose; // "root" or the issuing CA's purpose
  std::string curve, digest;
  std::string key_backend, token_label;
  bool root = false;
};

CaSpec root_spec(const cfg::Config &c) {
  return {"root",
          c.root.curve,
          c.root.digest,
          c.root.key_backend,
          c.root.token_label,
          true};
}

// A declared issuing CA's spec. Total by construction: the caller has
// already resolved which CA it means (by purpose, or by the profile being
// issued) and owns the error message when there is none.
CaSpec spec_of(const cfg::SigningCa &ca) {
  return {ca.purpose,     ca.curve,       ca.digest,
          ca.key_backend, ca.token_label, false};
}

// The CA that issues `profile`, or nullptr with the error already logged.
const cfg::SigningCa *issuer_for(const cfg::Config &config,
                                 const std::string &profile) {
  const cfg::SigningCa *ca = config.ca_for_profile(profile);
  if (!ca)
    log::error("no configured CA issues the '{}' profile", profile);
  return ca;
}

// The configuration of the CA a stored generation belongs to.
const cfg::SigningCa *config_of(const cfg::Config &config, const CaGen &gen) {
  const cfg::SigningCa *ca = config.ca(gen.purpose);
  if (!ca)
    log::error("the store holds CA '{}' but no [ca.{}] section describes it",
               gen.cn, gen.purpose);
  return ca;
}

std::string_view ca_secret(const CaSpec &ca, const ca::Secrets &s) {
  if (ca.key_backend == "pkcs11")
    return ca.root ? s.root_pin : s.pin;
  return s.passphrase;
}

// Env var(s) naming this CA's secret (for error messages). The root PIN
// falls back to the signing PIN, so name both.
std::string secret_env(const CaSpec &ca) {
  if (ca.key_backend != "pkcs11")
    return app::passphrase_env;
  return ca.root ? std::format("{} (or {})", app::root_pin_env, app::pin_env)
                 : std::string(app::pin_env);
}

// "passphrase"/"user PIN" for wrong-secret error messages; the two-CA
// variant covers operations that load more than one CA key.
const char *secret_word(const CaSpec &ca) {
  return ca.key_backend == "pkcs11" ? "user PIN" : "passphrase";
}

const char *secret_word_both(const CaSpec &a, const CaSpec &b) {
  return a.key_backend == b.key_backend ? secret_word(a)
                                        : "passphrase or user PIN";
}

// One session per token label: CAs sharing a label (the single-token
// layout) share the first session opened, so several keys still cost a
// single login. Must outlive every key handed out - the keys reference
// the session.
using TokenSessions = std::map<std::string, p11::Token>;

// The session is opened with the mode the first caller asked for: a
// read-only request on an already-open read-write session gets the
// read-write one.
p11::Token &open_token(const cfg::Config &config, const ca::Secrets &secrets,
                       TokenSessions &tokens, const CaSpec &ca,
                       bool read_write) {
  return tokens
      .try_emplace(ca.token_label, config, ca.token_label,
                   ca_secret(ca, secrets), read_write)
      .first->second;
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

// Loads one CA private key per its CA's backend: from the store
// ("internal", passphrase-encrypted) or from that CA's token, where `slug`
// (the generation's) is the key label. `tokens` owns the PKCS#11 sessions
// and must outlive the returned key; open sessions are reused (see
// open_token).
std::shared_ptr<const Botan::Private_Key>
ca_key(const cfg::Config &config, const ca::Secrets &secrets,
       Botan::Certificate_Store_In_SQL &store,
       const Botan::X509_Certificate &cert, const CaSpec &ca,
       const std::string &slug, TokenSessions &tokens) {
  if (ca.key_backend == "pkcs11") {
    auto key = open_token(config, secrets, tokens, ca, /*read_write=*/false)
                   .find_keypair(slug, ca.curve);
    if (!key)
      throw std::runtime_error(
          std::format("CA key '{}' not found on the token", slug));
    return key;
  }
  return store.find_key(cert);
}

// Adopt-or-generate one CA key per its backend: token-resident (adopted by
// label, or generated on that CA's token), or in-memory ECDSA that the
// caller persists encrypted into the store.
std::shared_ptr<const Botan::Private_Key>
make_ca_key(const cfg::Config &config, const ca::Secrets &secrets,
            TokenSessions &tokens, const CaSpec &spec, const std::string &slug,
            Botan::RandomNumberGenerator &rng) {
  if (spec.key_backend == "pkcs11")
    return token_ca_key(
        open_token(config, secrets, tokens, spec, /*read_write=*/true), slug,
        spec.curve);
  return std::make_shared<Botan::ECDSA_PrivateKey>(
      rng, Botan::EC_Group::from_name(spec.curve));
}

// The certificate profile of an issuing CA: pathlen 0, the EKUs it may
// issue - so the CA can never be broader than its purpose - and the
// pointers at the root that signed it. Shared by the init ceremony,
// `add signing-ca` and the rotation, which mint the same shape and differ
// only in the generation and in what they record afterwards.
Botan::Extensions signing_ca_extensions(const cfg::Config &config,
                                        const cfg::SigningCa &ca,
                                        const Botan::X509_Certificate &root,
                                        const std::string &root_slug,
                                        const std::vector<uint8_t> &pub_key,
                                        const std::string &digest) {
  auto ext = ca_extensions(pub_key, digest, std::size_t{0}, &root);
  // The union of what its profiles need, plus each profile's companion EKU
  // (profile.h), and nothing else: verifiers that intersect the EKU sets
  // along a chain refuse a leaf this CA had no business signing, and they
  // refuse it at the CA's depth even when the leaf itself looks fine.
  std::vector<Botan::OID> ekus;
  for (const auto &name : ca.profiles)
    if (const profile::Def *p = profile::find(name)) {
      ekus.emplace_back(std::string(p->eku));
      if (!p->ca_companion_eku.empty())
        ekus.emplace_back(std::string(p->ca_companion_eku));
    }
  ext.add_new(
      std::make_unique<Botan::Cert_Extension::Extended_Key_Usage>(ekus));
  // nameConstraints, when declared: the EKU bounds what a certificate may
  // be used for, this bounds who it may be issued to. Critical, as RFC
  // 5280 requires - a verifier that cannot understand the limit must
  // refuse the chain rather than ignore it.
  if (!ca.permitted_dns.empty() || !ca.permitted_email.empty())
    ext.add_new(
        std::make_unique<x509ext::Name_Constraints>(
            x509ext::NameConstraints{ca.permitted_dns, ca.permitted_email}),
        true);
  add_signing_pointer_extensions(ext, config, root_slug);
  return ext;
}

// The published artifacts of one CA generation: certificate (PEM for the
// store's own lookups, DER for the repository) plus an empty signed CRL, so
// the CDP URL resolves before any revocation.
bool write_ca_artifacts(const fs::path &ca_dir, const std::string &slug,
                        const Botan::X509_Certificate &cert,
                        Botan::X509_CA &issuer, int crl_horizon,
                        Botan::RandomNumberGenerator &rng) {
  return write_pem(ca_dir / (slug + ".pem"), cert) &&
         write_der(ca_dir / (slug + ".crt"), cert) &&
         write_der(ca_dir / (slug + ".crl"),
                   issuer.new_crl(rng, crl_next_update(cert, crl_horizon)));
}

bool create(const cfg::Config &config, const fs::path &db_path,
            const ca::Secrets &secrets) {
  Botan::AutoSeeded_RNG rng;

  const CaSpec root_ca = root_spec(config);
  TokenSessions tokens;
  auto root_key =
      make_ca_key(config, secrets, tokens, root_ca, config.root.slug, rng);

  // The anchor, self-signed. X509::create_self_signed_cert cannot be used:
  // it builds the subject DN through X509_Cert_Options and would order it
  // CN-first, so the certificate is assembled here from the same parts -
  // with the issuer DN equal to the subject, as self-signing means.
  const auto now = Clock::now();
  const std::vector<uint8_t> root_pub = Botan::X509::BER_encode(*root_key);
  auto root_signer = Botan::X509_Object::choose_sig_format(
      *root_key, rng, config.root.digest, "");
  const Botan::X509_DN root_dn =
      subject_dn(config.pki, config.root.cn, /*simple=*/false);
  auto root_cert = Botan::X509_CA::make_cert(
      *root_signer, rng, root_signer->algorithm_identifier(), root_pub,
      Botan::X509_Time(now),
      Botan::X509_Time(now + std::chrono::days(config.root.valid_days)),
      root_dn, root_dn,
      ca_extensions(root_pub, config.root.digest, std::nullopt, nullptr));

  // One generation-1 certificate per declared issuing CA, all signed in
  // this single root ceremony.
  struct Minted {
    const cfg::SigningCa *cfg;
    std::shared_ptr<const Botan::Private_Key> key;
    Botan::X509_Certificate cert;
  };
  Botan::X509_CA issuer(root_cert, *root_key, config.root.digest, rng);
  std::vector<Minted> minted;
  for (const auto &[purpose, ca] : config.cas) {
    auto key = make_ca_key(config, secrets, tokens, spec_of(ca), ca.slug, rng);
    const std::vector<uint8_t> pub = Botan::X509::BER_encode(*key);
    auto cert = Botan::X509_CA::make_cert(
        issuer.signature_op(), rng, issuer.algorithm_identifier(), pub,
        Botan::X509_Time(now),
        Botan::X509_Time(now + std::chrono::days(ca.valid_days)),
        root_cert.subject_dn(), subject_dn(config.pki, ca.cn, /*simple=*/false),
        signing_ca_extensions(config, ca, root_cert, config.root.slug, pub,
                              config.root.digest));
    minted.push_back({&ca, std::move(key), std::move(cert)});
  }

  auto db = open_store(db_path);
  // 0600: holds the encrypted CA keys, and the store must stay writable.
  set_perms(db_path, fs::perms::owner_read | fs::perms::owner_write);
  // New stores start in WAL directly (see begin_write).
  db->stmt("PRAGMA journal_mode=WAL")->spin();
  Botan::Certificate_Store_In_SQL store(db, secrets.passphrase, rng);
  store.insert_cert(root_cert);
  // pkcs11 keys never leave their token; internal keys persist encrypted.
  if (root_ca.key_backend == "internal")
    store.insert_key(root_cert, *root_key);
  for (const Minted &m : minted) {
    store.insert_cert(m.cert);
    if (m.cfg->key_backend == "internal")
      store.insert_key(m.cert, *m.key);
  }

  db->create_table(std::format("CREATE TABLE IF NOT EXISTS {} ("
                               "key TEXT PRIMARY KEY, value TEXT NOT NULL)",
                               app::config_table));
  for (const auto &[k, v] : locked_global(config)) {
    auto cins = db->stmt(std::format(
        "INSERT INTO {} (key, value) VALUES (?1, ?2)", app::config_table));
    cins->bind(1, k);
    cins->bind(2, v);
    cins->spin();
  }
  // Every issuing CA the ceremony creates is locked as its own section.
  ensure_purpose_table(*db);
  for (const auto &[purpose, ca] : config.cas)
    lock_purpose(*db, ca);

  ensure_cert_index(*db);
  index_cert(*db, root_cert, "root", "root");
  for (const Minted &m : minted)
    index_cert(*db, m.cert, "signing", m.cfg->purpose);
  // Generation 1 of each CA; generation is signed from the store.
  ensure_ca_index(*db, config);

  const fs::path ca_dir = db_path.parent_path() / "ca";
  fs::create_directories(ca_dir);
  set_perms(ca_dir, fs::perms::owner_all);
  if (!write_ca_artifacts(ca_dir, config.root.slug, root_cert, issuer,
                          app::root_crl_next_update_days, rng))
    log::warn("could not write the root artifacts under {}", ca_dir.string());
  std::string names;
  for (const Minted &m : minted) {
    Botan::X509_CA self(m.cert, *m.key, m.cfg->digest, rng);
    if (!write_ca_artifacts(ca_dir, m.cfg->slug, m.cert, self,
                            app::crl_next_update_days, rng))
      log::warn("could not write the artifacts of '{}' under {}", m.cfg->cn,
                ca_dir.string());
    names += std::format("{}{} ({})", names.empty() ? "" : ", ", m.cfg->cn,
                         m.cfg->purpose);
  }

  log::info("created 2-tier CA: {} (root), {}", config.root.cn, names);
  return true;
}

// Both root ceremonies need the root key and the new CA's own secret.
bool ceremony_secrets_ok(const cfg::Config &config, const Secrets &secrets,
                         const cfg::SigningCa &ca_cfg, const char *what) {
  const CaSpec root_ca = root_spec(config);
  const CaSpec sign_ca = spec_of(ca_cfg);
  if (ca_secret(root_ca, secrets).empty()) {
    log::error("{} not set (the root key signs {})", secret_env(root_ca), what);
    return false;
  }
  if (ca_secret(sign_ca, secrets).empty()) {
    log::error("{} not set (the new CA key needs it)", secret_env(sign_ca));
    return false;
  }
  return true;
}

struct MintedGen {
  Botan::X509_Certificate cert;
  std::shared_ptr<const Botan::Private_Key> key;
};

// One root ceremony minting generation `next` of the issuing CA described
// by `ca_cfg`: brings the root key online, creates the new CA key, signs
// the certificate and publishes its artifacts. Shared by `add signing-ca`
// (generation 1 of a new purpose) and `renew signing-ca` (the successor of
// an existing one) - the same ceremony, differing only in what the caller
// records afterwards, so the caller owns the transaction.
std::optional<MintedGen>
mint_ca_generation(const cfg::Config &config, const fs::path &store_dir,
                   const Secrets &secrets, const cfg::SigningCa &ca_cfg,
                   const CaGen &root, const CaGen &next,
                   Botan::Certificate_Store_In_SQL &store,
                   Botan::RandomNumberGenerator &rng) {
  const CaSpec root_ca = root_spec(config);
  const CaSpec sign_ca = spec_of(ca_cfg);
  const fs::path ca_dir = store_dir / "ca";

  auto root_cert = load_ca_cert(store_dir, root.slug);
  if (!root_cert) {
    log::error("root CA cert not found under {}", ca_dir.string());
    return std::nullopt;
  }
  // A CA may no more outlive its issuer than a leaf may: a root too close
  // to its own notAfter must itself be rotated first.
  if (outlives_issuer(*root_cert, std::chrono::seconds(
                                      std::chrono::days(ca_cfg.valid_days))))
    return std::nullopt;

  TokenSessions tokens;
  std::shared_ptr<const Botan::Private_Key> root_key, new_key;
  try {
    // Read-write sessions: with a pkcs11 backend the new generation's
    // keypair is generated on its token under its own label (adopted if a
    // previous attempt left it there).
    if (root_ca.key_backend == "pkcs11")
      root_key = token_ca_key(
          open_token(config, secrets, tokens, root_ca, /*read_write=*/true),
          root.slug, root_ca.curve);
    else
      root_key = store.find_key(*root_cert);
    new_key = make_ca_key(config, secrets, tokens, sign_ca, next.slug, rng);
  } catch (const std::exception &e) {
    log::error("cannot load the CA keys (wrong {}?): {}",
               secret_word_both(root_ca, sign_ca), e.what());
    return std::nullopt;
  }
  if (!root_key || !new_key) {
    log::error("root key missing from store");
    return std::nullopt;
  }

  Botan::X509_CA issuer(*root_cert, *root_key, config.root.digest, rng);
  const auto tp = Clock::now();
  const std::vector<uint8_t> pub = Botan::X509::BER_encode(*new_key);
  auto cert = Botan::X509_CA::make_cert(
      issuer.signature_op(), rng, issuer.algorithm_identifier(), pub,
      Botan::X509_Time(tp),
      Botan::X509_Time(tp + std::chrono::days(ca_cfg.valid_days)),
      root_cert->subject_dn(),
      subject_dn(config.pki, next.cn, /*simple=*/false),
      signing_ca_extensions(config, ca_cfg, *root_cert, root.slug, pub,
                            config.root.digest));

  // Artifacts before the database: an interrupted ceremony that leaves
  // files behind is recoverable (the next run replaces them), while an
  // active generation without a published certificate would break issuance.
  Botan::X509_CA self(cert, *new_key, ca_cfg.digest, rng);
  if (!write_ca_artifacts(ca_dir, next.slug, cert, self,
                          app::crl_next_update_days, rng)) {
    log::error("could not write the new CA artifacts under {}",
               ca_dir.string());
    return std::nullopt;
  }
  return MintedGen{std::move(cert), std::move(new_key)};
}

// Resolves a CA selector to a generation CN: a literal CN passes through,
// "root-ca" and "<purpose>-ca" name that CA's active generation.
// "signing-ca" keeps working while exactly one issuing CA exists; with
// several it names nothing in particular and the aliases are listed
// instead. Empty on failure, with the error already logged.
std::string resolve_ca_cn(store::Database &db, const cfg::Config &config,
                          const std::string &selector) {
  if (selector == "root-ca")
    return active_ca(db, config, "root").cn;
  if (selector.ends_with("-ca")) {
    const std::string purpose = selector.substr(0, selector.size() - 3);
    if (config.ca(purpose))
      return active_ca(db, config, purpose).cn;
  }
  if (selector == "signing-ca") {
    if (config.cas.size() == 1)
      return active_ca(db, config, config.cas.begin()->first).cn;
    std::string aliases;
    for (const auto &[purpose, ca] : config.cas)
      aliases += std::format("{}{}-ca", aliases.empty() ? "" : ", ", purpose);
    log::error("'signing-ca' is ambiguous with more than one issuing CA; "
               "use one of: {})",
               aliases, selector);
    return {};
  }
  return selector; // a literal generation CN
}

// A CN identifies a generation in cert_index and in a CRL's issuer field,
// so no two CAs, of any purpose or generation, may share one.
bool cn_is_taken(store::Database &db, const CaGen &root,
                 const std::string &cn) {
  if (cn == root.cn || gen_by_cn(db, "signing", cn) ||
      gen_by_cn(db, "root", cn)) {
    log::error("CN '{}' already names a CA generation; pick another", cn);
    return true;
  }
  return false;
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
      auto s = h->stmt(
          std::format("SELECT value FROM {} WHERE key=?1", app::config_table));
      s->bind(1, key);
      return s->step() ? s->get_str(0) : std::string();
    };
    const std::string root_cn = locked("root.cn");
    if (root_cn.empty())
      return false;
    // Initialization is a historical fact about the ceremony, what
    // later became of an anchor does not unmake it. What still
    // matters is that the anchors the config names were actually issued.
    auto anchor = [&](const std::string &kind, const std::string &cn) {
      auto s =
          h->stmt("SELECT 1 FROM cert_index WHERE kind=?1 AND cn=?2 LIMIT 1");
      s->bind(1, kind);
      s->bind(2, cn);
      return s->step();
    };
    if (!anchor("root", root_cn))
      return false;
    // Every locked issuing CA must have been issued too; a store with no
    // issuing CA at all never finished a ceremony.
    auto cas = h->stmt(std::format("SELECT cn FROM {}", app::purpose_table));
    bool any = false;
    while (cas->step()) {
      any = true;
      if (!anchor("signing", cas->get_str(0)))
        return false;
    }
    return any;
  } catch (const std::exception &) {
    return false; // unreadable store => not initialized
  }
}

bool init(const cfg::Config &config, const fs::path &store_dir,
          const Secrets &secrets) {
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

  // The CA secrets are settled before anything touches the filesystem: a
  // failed precondition must not leave a store_dir (or a log file) behind.
  std::vector<CaSpec> specs{root_spec(config)};
  for (const auto &[purpose, ca] : config.cas)
    specs.push_back(spec_of(ca));
  Secrets eff = secrets;
  for (const CaSpec &ca : specs) // a token PIN cannot be invented
    if (ca.key_backend == "pkcs11" && ca_secret(ca, eff).empty()) {
      log::error("{} not set (token user PIN)", secret_env(ca));
      return false;
    }
  std::string generated;
  const bool any_internal =
      std::any_of(specs.begin(), specs.end(), [](const CaSpec &ca) {
        return ca.key_backend == "internal";
      });
  if (any_internal && eff.passphrase.empty()) {
    Botan::AutoSeeded_RNG rng;
    std::vector<uint8_t> raw(app::passphrase_bytes);
    rng.randomize(raw.data(), raw.size());
    generated = Botan::hex_encode(raw);
    eff.passphrase = generated;
    log::to_stdout(
        "\n┌──────────────────────────────────────────────────────────────────┐"
        "\n"
        "│                 {} (shown once)                 │\n"
        "├──────────────────────────────────────────────────────────────────┤\n"
        "│ {} │\n"
        "└──────────────────────────────────────────────────────────────────┘"
        "\n",
        app::passphrase_env, generated);
  }

  fs::create_directories(store_dir);
  set_perms(store_dir, fs::perms::owner_all);
  try {
    return create(config, db_path, eff);
  } catch (const std::exception &e) {
    log::error("CA init failed: {}", e.what());
    return false;
  }
}

bool add_signing_ca(const cfg::Config &config, const fs::path &store_dir,
                    const Secrets &secrets, const std::string &purpose) {
  const fs::path db = store_path(store_dir);
  if (!fs::exists(db)) {
    log::error("not initialized; run '{} init'", app::name);
    return false;
  }
  if (purpose.empty()) {
    log::error("--purpose is required (the [ca.<purpose>] section to create)");
    return false;
  }
  // The file declares what may exist; the store records what does. This
  // command is exactly the gap between the two.
  const cfg::SigningCa *ca_cfg = config.ca(purpose);
  if (!ca_cfg) {
    log::error("no [ca.{}] section in {}.toml; declare it first", purpose,
               app::name);
    return false;
  }
  if (!ceremony_secrets_ok(config, secrets, *ca_cfg, "the new CA"))
    return false;

  Botan::AutoSeeded_RNG rng;
  auto dbh = open_store(db);
  Botan::Certificate_Store_In_SQL store(dbh, secrets.passphrase, rng);
  ensure_ca_index(*dbh, config);
  ensure_purpose_table(*dbh);

  auto held = dbh->stmt(
      std::format("SELECT 1 FROM {} WHERE purpose=?1", app::purpose_table));
  held->bind(1, purpose);
  if (held->step()) {
    log::error("the store already holds CA '{}'; use '{} renew signing-ca "
               "--purpose {}' to rotate it",
               purpose, app::name, purpose);
    return false;
  }

  const CaGen root = active_ca(*dbh, config, "root");
  const CaGen next{purpose, 1, ca_cfg->cn, ca_cfg->slug};
  if (cn_is_taken(*dbh, root, next.cn))
    return false;

  auto minted = mint_ca_generation(config, store_dir, secrets, *ca_cfg, root,
                                   next, store, rng);
  if (!minted)
    return false;

  begin_write(*dbh);
  store.insert_cert(minted->cert);
  if (ca_cfg->key_backend == "internal")
    store.insert_key(minted->cert, *minted->key);
  ensure_cert_index(*dbh);
  index_cert(*dbh, minted->cert, "signing", purpose);
  // The section is locked as it is created, which is the whole point: the
  // rest of the config stays frozen and this one joins it.
  lock_purpose(*dbh, *ca_cfg);
  auto ins = dbh->stmt("INSERT INTO ca_cert_index "
                       "(kind,purpose,gen,cn,slug,status) "
                       "VALUES ('signing',?1,1,?2,?3,'active')");
  ins->bind(1, purpose);
  ins->bind(2, next.cn);
  ins->bind(3, next.slug);
  ins->spin();
  commit_write(*dbh);

  log::info("added issuing CA '{}' ({}) as {}, issuing {}", next.cn, purpose,
            next.slug, join_profiles(ca_cfg->profiles));
  log::to_stdout("{}", next.cn);
  return true;
}

bool renew_signing_ca(const cfg::Config &config, const fs::path &store_dir,
                      const Secrets &secrets, const std::string &purpose,
                      const std::string &new_cn) {
  const fs::path db = store_path(store_dir);
  if (!fs::exists(db)) {
    log::error("not initialized; run '{} init'", app::name);
    return false;
  }
  const cfg::SigningCa *ca_cfg = config.ca(purpose);
  if (!ca_cfg) {
    log::error("no CA named '{}'; run '{} get config' for the purposes this "
               "store holds",
               purpose, app::name);
    return false;
  }
  if (!ceremony_secrets_ok(config, secrets, *ca_cfg, "the new generation"))
    return false;
  if (new_cn.empty()) {
    log::error("--new-cn is required (the new generation's common name)");
    return false;
  }

  Botan::AutoSeeded_RNG rng;
  auto dbh = open_store(db);
  Botan::Certificate_Store_In_SQL store(dbh, secrets.passphrase, rng);
  ensure_ca_index(*dbh, config);

  const CaGen root = active_ca(*dbh, config, "root");
  const CaGen incumbent = active_ca(*dbh, config, purpose);
  const CaGen next{purpose, incumbent.gen + 1, new_cn,
                   ca_cfg->slug_prefix + std::to_string(incumbent.gen + 1)};
  if (cn_is_taken(*dbh, root, new_cn))
    return false;

  const fs::path pem = store_dir / "ca" / (next.slug + ".pem");
  if (fs::exists(pem))
    log::warn("{} exists from an unfinished renewal; replacing it",
              pem.string());

  auto minted = mint_ca_generation(config, store_dir, secrets, *ca_cfg, root,
                                   next, store, rng);
  if (!minted)
    return false;
  const Botan::X509_Certificate &cert = minted->cert;

  begin_write(*dbh);
  store.insert_cert(cert);
  if (ca_cfg->key_backend == "internal")
    store.insert_key(cert, *minted->key);
  ensure_cert_index(*dbh);
  index_cert(*dbh, cert, "signing", next.purpose);
  // Only this CA's lineage retires: a rotation of one issuing CA leaves
  // every other purpose alone.
  auto retire = dbh->stmt("UPDATE ca_cert_index SET status='retiring' "
                          "WHERE purpose=?1 AND status='active'");
  retire->bind(1, next.purpose);
  retire->spin();
  auto ins = dbh->stmt("INSERT INTO ca_cert_index "
                       "(kind,purpose,gen,cn,slug,status) "
                       "VALUES ('signing',?1,?2,?3,?4,'active')");
  ins->bind(1, next.purpose);
  ins->bind(2, static_cast<std::size_t>(next.gen));
  ins->bind(3, next.cn);
  ins->bind(4, next.slug);
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
  auto sel =
      dbh->stmt(std::format("SELECT key, value FROM {}", app::config_table));
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
  c.pki.org_name = S("pki.org_name");
  c.pki.country_code = S("pki.country_code");
  c.pki.repository_host = S("pki.repository_host");
  c.pki.arc_oid = S("pki.arc_oid");
  c.pkcs11.module = S("pkcs11.module");
  c.pkcs11.token_label = S("pkcs11.token_label");
  c.root.cn = S("root.cn");
  c.root.curve = S("root.curve");
  c.root.digest = S("root.digest");
  c.root.valid_days = I("root.valid_days");
  // The prefix is the rotation baseline; the
  // derived slug is the operative identity for files, URLs and key
  // labels. Both are snapshotted at init and read back here.
  c.root.slug_prefix = S("root.slug_prefix");
  c.root.slug = S("root.slug");
  c.root.key_backend = S("root.key_backend");
  c.root.token_label = S("root.token_label");

  auto cas = dbh->stmt(std::format(
      "SELECT purpose,profiles,cn,curve,digest,valid_days,slug_prefix,slug,"
      "key_backend,token_label,ee_curve,ee_digest,ee_valid_days,simple_dn,"
      "permitted_dns,permitted_email FROM {}",
      app::purpose_table));
  while (cas->step()) {
    cfg::SigningCa ca;
    ca.purpose = cas->get_str(0);
    ca.profiles = split_profiles(cas->get_str(1));
    ca.cn = cas->get_str(2);
    ca.curve = cas->get_str(3);
    ca.digest = cas->get_str(4);
    ca.valid_days = static_cast<int>(cas->get_size_t(5));
    ca.slug_prefix = cas->get_str(6);
    ca.slug = cas->get_str(7);
    ca.key_backend = cas->get_str(8);
    ca.token_label = cas->get_str(9);
    ca.ee_curve = cas->get_str(10);
    ca.ee_digest = cas->get_str(11);
    ca.ee_valid_days = static_cast<int>(cas->get_size_t(12));
    ca.simple_dn = cas->get_size_t(13) != 0;
    ca.permitted_dns = split_profiles(cas->get_str(14));
    ca.permitted_email = split_profiles(cas->get_str(15));
    c.cas.emplace(ca.purpose, std::move(ca));
  }
  // An initialized store always holds at least one issuing CA; without one
  // nothing downstream (config.signing()) has an issuer to name.
  if (c.cas.empty())
    return std::nullopt;
  return c;
}

void reconcile(const cfg::Config &file, const cfg::Config &eff) {
  // Every materialized section is locked: warn and ignore any change, the
  // DB stays authoritative. Re-init to change a locked section.
  auto compare = [](const std::string &section, const KeyValues &fk,
                    const KeyValues &ek) {
    for (std::size_t i = 0; i < fk.size(); ++i)
      if (fk[i].second != ek[i].second)
        log::warn("ignoring readonly params changed in {}.toml ([{}] {}), "
                  "check against 'get config'",
                  app::name, section, fk[i].first);
  };
  compare("pki/pkcs11/root", locked_global(file), locked_global(eff));

  // Per section from here: a purpose the file declares but the store has
  // never created is not a change to ignore, it is work not done yet.
  for (const auto &[purpose, ca] : file.cas) {
    auto it = eff.cas.find(purpose);
    if (it == eff.cas.end()) {
      log::to_stderr("[ca.{}] is declared but not created in the store",
                     purpose);
      continue;
    }
    compare("ca." + purpose, locked_purpose(ca), locked_purpose(it->second));
  }
  for (const auto &[purpose, ca] : eff.cas)
    if (!file.cas.contains(purpose))
      log::warn("[ca.{}] exists in the store but is missing from {}.toml; "
                "check against 'get config'",
                purpose, app::name);
}

namespace {

// --valid must sit inside the policy: at least the 5-minute floor (below
// it clock skew kills the certificate on arrival), at most the effective
// ee_valid_days ceiling - shorter than policy is always allowed.
bool check_valid_override(const cfg::SigningCa &ca, std::chrono::seconds v) {
  if (v >= std::chrono::minutes(app::min_valid_override_minutes) &&
      v <= std::chrono::days(ca.ee_valid_days))
    return true;
  log::error("--valid must be in [{}m, {}d] (up to ee_valid_days)",
             app::min_valid_override_minutes, ca.ee_valid_days);
  return false;
}

} // namespace

bool issue_ee(const cfg::Config &config, const fs::path &store_dir,
              const Secrets &secrets, const std::string &profile,
              const std::string &cn, const std::vector<San> &extra_sans,
              std::optional<std::chrono::seconds> valid_override) {
  const profile::Def *prof = profile_def(profile);
  if (!prof)
    return false;
  // A profile whose key must never reach the CA is CSR-only: `create`
  // would generate it here and leave it on disk.
  if (prof->csr_only) {
    log::error("the '{}' profile is issued from a CSR only", profile);
    return false;
  }
  // The profile picks the issuer, and with it the EE policy: curve, digest
  // and validity ceiling all belong to the CA that will sign.
  const cfg::SigningCa *ca_cfg = issuer_for(config, profile);
  if (!ca_cfg)
    return false;
  const std::chrono::seconds validity = valid_override.value_or(
      std::chrono::seconds(std::chrono::days(ca_cfg->ee_valid_days)));
  if (valid_override && !check_valid_override(*ca_cfg, *valid_override))
    return false;

  std::vector<San> sans;
  switch (prof->subject) {
  case profile::Subject::DnsCn:
    // The CN becomes a dNSName SAN (IA5String): hostname ASCII only. Other
    // profiles keep the CN DN-only (UTF8String), so it is free-form.
    if (!dns_safe(cn)) {
      log::error("{} CN must be an ASCII hostname [A-Za-z0-9.*-]: '{}'",
                 profile, cn);
      return false;
    }
    sans.push_back({San::Type::Dns, cn});
    break;
  case profile::Subject::RequireSan:
    if (extra_sans.empty()) {
      log::error("the '{}' profile requires at least one --san", profile);
      return false;
    }
    break;
  case profile::Subject::EmailMatchingCn:
    // The mailbox is the identity: it must be in the certificate as an
    // rfc822Name, and the CN must name the same mailbox.
    if (std::none_of(extra_sans.begin(), extra_sans.end(), [&](const San &s) {
          return s.type == San::Type::Email && s.value == cn;
        })) {
      log::error("the '{}' profile requires --san email:<cn>", profile);
      return false;
    }
    break;
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
  const CaSpec sign_ca = spec_of(*ca_cfg);
  if (ca_secret(sign_ca, secrets).empty()) {
    log::error("{} not set (required to sign with the CA)",
               secret_env(sign_ca));
    return false;
  }

  Botan::AutoSeeded_RNG rng;
  auto dbh = open_store(db);
  Botan::Certificate_Store_In_SQL store(dbh, secrets.passphrase, rng);

  const CaGen sign = active_ca(*dbh, config, ca_cfg->purpose);
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
  auto duplicate = [&] { return blocking_duplicate(*dbh, cn, profile); };
  if (duplicate())
    return false;

  TokenSessions tokens; // must outlive sign_key (owns the session)
  std::shared_ptr<const Botan::Private_Key> sign_key;
  try {
    sign_key =
        ca_key(config, secrets, store, *sign_cert, sign_ca, sign.slug, tokens);
  } catch (const std::exception &e) {
    log::error("cannot load signing key (wrong {}?): {}", secret_word(sign_ca),
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

  Botan::EC_Group grp = Botan::EC_Group::from_name(ca_cfg->ee_curve);
  Botan::ECDSA_PrivateKey ee_key(rng, grp);

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

  // The same extension set and the same DN rules as the CSR path; only the
  // origin of the key differs.
  const std::vector<uint8_t> pub = Botan::X509::BER_encode(ee_key);
  Botan::X509_CA issuer(*sign_cert, *sign_key, ca_cfg->ee_digest, rng);
  const auto tp = Clock::now();
  auto ee_cert = Botan::X509_CA::make_cert(
      issuer.signature_op(), rng, issuer.algorithm_identifier(), pub,
      Botan::X509_Time(tp), Botan::X509_Time(tp + validity),
      sign_cert->subject_dn(), subject_dn(config.pki, cn, ca_cfg->simple_dn),
      ee_extensions(config, *prof, *sign_cert, sign.slug, pub,
                    ca_cfg->ee_digest, an));

  store.insert_cert(ee_cert);
  index_cert(*dbh, ee_cert, profile, sign.purpose);
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

  log::info("issued {} certificate for CN '{}'", profile, cn);
  return true;
}

namespace {

// The enrollment table backs identity validation for CSR signing: one row per
// identity, holding at most one pending nonce. `consumed` defaults to true so
// a row is in the "nothing pending" state until a `get nonce`.
void ensure_enrollment(store::Database &db) {
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
    auto q = dbh->stmt("SELECT 1 FROM enrollment WHERE id=?1");
    q->bind(1, id);
    if (q->step()) {
      log::error("identity '{}' is already enrolled", id);
      return false;
    }
  }
  auto ins = dbh->stmt(
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

  auto q = dbh->stmt("SELECT IFNULL(nonce,''), issued, validity, "
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
  auto u = dbh->stmt("UPDATE enrollment SET nonce=?1, issued=?2, "
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
              const Secrets &secrets, const std::string &profile,
              const std::string &id, const std::string &nonce,
              const std::string &csr_src,
              std::optional<std::chrono::seconds> valid_override) {
  const profile::Def *prof = profile_def(profile);
  if (!prof)
    return false;
  const cfg::SigningCa *ca_cfg = issuer_for(config, profile);
  if (!ca_cfg)
    return false;
  if (valid_override && !check_valid_override(*ca_cfg, *valid_override))
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
      ec->domain() != Botan::EC_Group::from_name(ca_cfg->ee_curve)) {
    log::error("CSR public key must be ECDSA on {}", ca_cfg->ee_curve);
    return false;
  }

  // Subject DN: only the CN is taken from the request, and even that only
  // as a name - the DN itself is rebuilt from the CA's own configuration
  // (detail::subject_dn), so an organization or country the requester
  // asked for is never the one that ends up in the certificate.
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
    log::warn("ignoring the CSR's subject attributes other than the CN; the "
              "CA builds the subject DN");

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
  switch (prof->subject) {
  case profile::Subject::DnsCn:
    // Same CN rules as issue_ee: hostname CN, always present as DNS:CN (the
    // set-backed AlternativeName dedups if the CSR listed it too).
    if (!dns_safe(cn)) {
      log::error("{} CN must be an ASCII hostname [A-Za-z0-9.*-]: '{}'",
                 profile, cn);
      return false;
    }
    an.add_dns(cn);
    break;
  case profile::Subject::RequireSan:
    if (supported == 0) {
      log::error("the '{}' profile requires at least one SAN in the CSR",
                 profile);
      return false;
    }
    break;
  case profile::Subject::EmailMatchingCn: {
    const auto &mails = csr_san.email();
    if (std::find(mails.begin(), mails.end(), cn) == mails.end()) {
      log::error("the '{}' profile requires the CSR to carry an rfc822Name "
                 "SAN equal to the CN",
                 profile);
      return false;
    }
    break;
  }
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
  const CaSpec sign_ca = spec_of(*ca_cfg);
  if (ca_secret(sign_ca, secrets).empty()) {
    log::error("{} not set (required to sign with the CA)",
               secret_env(sign_ca));
    return false;
  }

  Botan::AutoSeeded_RNG rng;
  auto dbh = open_store(db);
  Botan::Certificate_Store_In_SQL store(dbh, secrets.passphrase, rng);

  const CaGen sign = active_ca(*dbh, config, ca_cfg->purpose);
  auto sign_cert = load_ca_cert(store_dir, sign.slug);
  if (!sign_cert) {
    log::error("signing CA cert not found under {}",
               (store_dir / "ca").string());
    return false;
  }
  const std::chrono::seconds validity = valid_override.value_or(
      std::chrono::seconds(std::chrono::days(ca_cfg->ee_valid_days)));
  if (outlives_issuer(*sign_cert, validity))
    return false;

  // Fail fast before the signing key lookup (seconds on NK HSM): nonce and
  // uniqueness are answerable from the DB alone. The write lock below
  // repeats both checks authoritatively.
  ensure_cert_index(*dbh);
  ensure_enrollment(*dbh);
  auto nonce_rejected = [&] {
    auto q = dbh->stmt("SELECT IFNULL(nonce,''), issued, validity, "
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
  auto duplicate = [&] { return blocking_duplicate(*dbh, cn, profile); };
  if (nonce_rejected() || duplicate())
    return false;

  TokenSessions tokens; // owns the sessions; must outlive sign_key
  std::shared_ptr<const Botan::Private_Key> sign_key;
  try {
    sign_key =
        ca_key(config, secrets, store, *sign_cert, sign_ca, sign.slug, tokens);
  } catch (const std::exception &e) {
    log::error("cannot load signing key (wrong {}?): {}", secret_word(sign_ca),
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

  // The CA dictates every extension and the whole subject - nothing is
  // copied from the CSR. Botan's X509_CA::sign_request would honor
  // requested extensions (including basicConstraints CA:true) and the
  // request's own DN, so the cert is built via make_cert instead.
  Botan::X509_CA issuer(*sign_cert, *sign_key, ca_cfg->ee_digest, rng);
  const auto tp = Clock::now();
  auto ee_cert = Botan::X509_CA::make_cert(
      issuer.signature_op(), rng, issuer.algorithm_identifier(),
      req->raw_public_key(), Botan::X509_Time(tp),
      Botan::X509_Time(tp + validity), sign_cert->subject_dn(),
      subject_dn(config.pki, cn, ca_cfg->simple_dn),
      ee_extensions(config, *prof, *sign_cert, sign.slug, req->raw_public_key(),
                    ca_cfg->ee_digest, an));

  store.insert_cert(ee_cert);
  index_cert(*dbh, ee_cert, profile, sign.purpose);
  auto consume = dbh->stmt("UPDATE enrollment SET consumed=1 WHERE id=?1");
  consume->bind(1, id);
  consume->spin();
  commit_write(*dbh);

  // No <store>/ee/ artifacts: the CA never sees the requester's private key,
  // and delivery is `get <profile> --cn`. stdout carries just the CN so the
  // output pipes into it.
  log::info("issued {} certificate for CN '{}' from CSR (requested by '{}')",
            profile, cn, id);
  log::to_stdout("{}", cn);
  return true;
}

bool revoke(const cfg::Config &config, const fs::path &store_dir,
            const Secrets &secrets, const std::string &target,
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
  Botan::AutoSeeded_RNG rng;
  auto dbh = open_store(db);
  Botan::Certificate_Store_In_SQL store(dbh, secrets.passphrase, rng);

  // Fail fast before the signing key lookup (seconds on NK HSM): whether an
  // active cert exists is answerable from cert_index alone. The write lock
  // below repeats the lookup authoritatively.
  const std::string &subject_cn = cn;
  const std::string sel = normalize_serial(serial);
  auto lookup = [&](store::Database &d) {
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
  auto issuing = gen_by_cn(*dbh, "signing", issuer_cn);
  if (!issuing) {
    log::error("no CA generation matches the issuer of this certificate "
               "(issuer CN '{}')",
               issuer_cn);
    return false;
  }
  const CaGen sign = *issuing;
  // The entry goes on the issuing generation's CRL, so it is that CA's key
  // and secret that matter - not those of whichever CA issues the profile
  // today.
  const cfg::SigningCa *ca_cfg = config_of(config, sign);
  if (!ca_cfg)
    return false;
  const CaSpec sign_ca = spec_of(*ca_cfg);
  if (ca_secret(sign_ca, secrets).empty()) {
    log::error("{} not set (required to sign the CRL)", secret_env(sign_ca));
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

  TokenSessions tokens; // must outlive sign_key (owns the sessions)
  std::shared_ptr<const Botan::Private_Key> sign_key;
  try {
    sign_key =
        ca_key(config, secrets, store, *sign_cert, sign_ca, sign.slug, tokens);
  } catch (const std::exception &e) {
    log::error("cannot load signing key (wrong {}?): {}", secret_word(sign_ca),
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

  Botan::X509_CA ca(*sign_cert, *sign_key, ca_cfg->digest, rng);
  // Carry forward only the unexpired entries (RFC 5280 3.3 pruning), plus
  // the new one; make_crl continues the crlNumber that update_crl would.
  auto entries = prune_crl_entries(*dbh, prev);
  const std::size_t pruned = prev.get_revoked().size() - entries.size();
  entries.emplace_back(*target_cert, *reason);
  const auto updated = ca.make_crl(entries, prev.crl_number() + 1, rng,
                                   std::chrono::system_clock::now(),
                                   std::chrono::seconds(crl_next_update(
                                       *sign_cert, app::crl_next_update_days)));
  if (!write_der(crl_path, updated)) {
    log::error("could not write CRL {}", crl_path.string());
    return false;
  }

  // Keep the Botan store's own revocation state in sync (its `revoked` table
  // feeds generate_crls()); cert_index stays the query path.
  store.revoke_cert(*target_cert, *reason);
  auto u = dbh->stmt("UPDATE cert_index SET status='revoked', "
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
               const Secrets &secrets, const std::string &selector,
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
  const CaSpec root_ca = root_spec(config);
  if (ca_secret(root_ca, secrets).empty()) {
    log::error("{} not set (the root key signs its CRL)", secret_env(root_ca));
    return false;
  }
  if (selector.empty()) {
    log::error("revoke ca needs --cn (an issuing CA generation's name, or "
               "<purpose>-ca for that CA's active generation)");
    return false;
  }

  Botan::AutoSeeded_RNG rng;
  auto dbh = open_store(db);
  Botan::Certificate_Store_In_SQL store(dbh, secrets.passphrase, rng);
  ensure_ca_index(*dbh, config);

  const CaGen root = active_ca(*dbh, config, "root");

  // A self-signed root cannot be revoked by anything below it: dropping a
  // trust anchor is the relying parties' job, not the CA's.
  if (selector == "root-ca" || selector == root.cn) {
    log::error("the root cannot be revoked; remove it from the trust stores "
               "and re-initialize");
    return false;
  }
  const std::string cn = resolve_ca_cn(*dbh, config, selector);
  if (cn.empty())
    return false;
  const auto victim = gen_by_cn(*dbh, "signing", cn);
  if (!victim) {
    log::error("no issuing CA generation named '{}'", cn);
    return false;
  }
  // Revoking the issuer of record would leave that purpose with nothing to
  // issue with, and the successor is one command away with the root key
  // already in hand. Only the victim's own lineage matters here.
  const CaGen active = active_ca(*dbh, config, victim->purpose);
  if (victim->gen == active.gen) {
    log::error("'{}' is the active issuer for '{}'; run '{} renew signing-ca "
               "--purpose {} --new-cn <name>' first, then revoke it",
               cn, victim->purpose, app::name, victim->purpose);
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
  auto already = dbh->stmt(
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

  TokenSessions tokens; // must outlive root_key (owns the sessions)
  std::shared_ptr<const Botan::Private_Key> root_key;
  try {
    root_key =
        ca_key(config, secrets, store, *root_cert, root_ca, root.slug, tokens);
  } catch (const std::exception &e) {
    log::error("cannot load the root key (wrong {}?): {}", secret_word(root_ca),
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
  // `root_ca` is the CaSpec here, so the CRL issuer is named for its role.
  Botan::X509_CA root_issuer(*root_cert, *root_key, config.root.digest, rng);
  // Same RFC 5280 3.3 pruning as the signing CRL: an expired signing CA
  // generation leaves the root CRL after its final scheduled appearance.
  auto entries = prune_crl_entries(*dbh, prev);
  const std::size_t pruned = prev.get_revoked().size() - entries.size();
  entries.emplace_back(*victim_cert, *reason);
  const auto updated = root_issuer.make_crl(
      entries, prev.crl_number() + 1, rng, std::chrono::system_clock::now(),
      std::chrono::seconds(
          crl_next_update(*root_cert, app::root_crl_next_update_days)));
  if (!write_der(root_crl_path, updated)) {
    log::error("could not write the root CRL {}", root_crl_path.string());
    return false;
  }

  store.revoke_cert(*victim_cert, *reason);
  auto u = dbh->stmt("UPDATE cert_index SET status='revoked', "
                     "revoked_at=?1, reason=?2 WHERE fingerprint=?3");
  u->bind(1, now_epoch());
  u->bind(2, static_cast<std::size_t>(*reason));
  u->bind(3, victim_fp);
  u->spin();
  auto g = dbh->stmt("UPDATE ca_cert_index SET status='revoked' "
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
                 const Secrets &secrets, CrlScope scope) {
  const fs::path db = store_path(store_dir);
  if (!fs::exists(db)) {
    log::error("not initialized; run '{} init'", app::name);
    return false;
  }

  const bool do_root = scope != CrlScope::Signing;
  const bool do_sign = scope != CrlScope::Root;
  std::vector<CaSpec> specs{root_spec(config)};
  for (const auto &[purpose, ca] : config.cas)
    specs.push_back(spec_of(ca));
  // Each scope needs only its own CAs' secrets; that is the point of the
  // separate cadences (a signing run works with the root token in the safe).
  for (const CaSpec &ca : specs)
    if ((ca.root ? do_root : do_sign) && ca_secret(ca, secrets).empty()) {
      log::error("{} not set (required to sign the CRL)", secret_env(ca));
      return false;
    }

  Botan::AutoSeeded_RNG rng;
  auto dbh = open_store(db);
  Botan::Certificate_Store_In_SQL store(dbh, secrets.passphrase, rng);
  const fs::path ca_dir = store_dir / "ca";

  // One job per CA whose CRL is re-signed. A rotation leaves retiring
  // generations behind and each keeps publishing its own CRL until the
  // last certificate it issued is gone, so the scope is a set, not one CA.
  struct Job {
    CaGen ca;
    CaSpec spec;
    int horizon;
    Botan::X509_Certificate cert;
    std::shared_ptr<const Botan::Private_Key> key;
  };
  std::vector<Job> jobs;
  const std::size_t now = now_epoch();
  // A scope is a kind ("root" or every issuing CA), so each generation
  // carries the purpose that says which CA's key and digest sign it.
  auto collect = [&](const std::string &kind, int horizon) {
    for (const CaGen &g : live_cas(*dbh, config, kind)) {
      CaSpec spec = root_spec(config);
      if (kind != "root") {
        const cfg::SigningCa *ca_cfg = config_of(config, g);
        if (!ca_cfg)
          return false;
        spec = spec_of(*ca_cfg);
      }
      auto cert = load_ca_cert(store_dir, g.slug);
      const fs::path crl_path = ca_dir / (g.slug + ".crl");
      if (!cert || !fs::exists(crl_path)) {
        log::error("{} CA artifacts not found for {} under {}", g.purpose,
                   g.slug, ca_dir.string());
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
      jobs.push_back({g, spec, horizon, *cert, nullptr});
    }
    return true;
  };
  if (do_root && !collect("root", app::root_crl_next_update_days))
    return false;
  if (do_sign && !collect("signing", app::crl_next_update_days))
    return false;
  if (jobs.empty()) {
    log::warn("no CA in scope has a CRL to refresh");
    return true;
  }

  // Keys through per-kind token sessions (backend pkcs11), loaded before the
  // write lock: a NK HSM lookup takes seconds and other writers wait at most
  // busy_timeout. The signing scope never touches the root key or its token
  // - that is the point of the separate root cadence
  // (root_crl_next_update_days vs crl_next_update_days).
  TokenSessions tokens;
  try {
    for (Job &j : jobs)
      j.key = ca_key(config, secrets, store, j.cert, j.spec, j.ca.slug, tokens);
  } catch (const std::exception &e) {
    log::error("cannot load CA key (wrong {}?): {}",
               secret_word_both(specs.front(), specs.back()), e.what());
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
    Botan::X509_CA ca(j.cert, *j.key, j.spec.digest, rng);
    const auto kept = prune_crl_entries(*dbh, prev);
    const std::size_t pruned = prev.get_revoked().size() - kept.size();
    const auto next = ca.make_crl(
        kept, prev.crl_number() + 1, rng, std::chrono::system_clock::now(),
        std::chrono::seconds(crl_next_update(j.cert, j.horizon)));
    ok = write_der(crl_path, next) && ok;
    refreshed += std::format("{}{} #{}{}", refreshed.empty() ? "" : ", ",
                             j.ca.slug, next.crl_number(),
                             pruned ? std::format(" (pruned {})", pruned)
                                    : std::string());
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
              const std::string &encoding, bool chain) {
  const fs::path db = store_path(store_dir);
  if (!fs::exists(db)) {
    log::error("not initialized; run '{} init'", app::name);
    return false;
  }

  // `config` is the effective config: the locked snapshot, printed back in
  // the sections it was declared in, so the output is a valid yca.toml.
  if (target == "config") {
    std::print("# showing configuration stored in database\n");
    std::print("\n[pki]\norg_name = \"{}\"\ncountry_code = \"{}\"\n"
               "repository_host = \"{}\"\n",
               config.pki.org_name, config.pki.country_code,
               config.pki.repository_host);
    if (!config.pki.arc_oid.empty())
      std::print("arc_oid = \"{}\"\n", config.pki.arc_oid);

    const bool root_p11 = config.root.key_backend == "pkcs11";
    bool any_p11 = root_p11;
    for (const auto &[purpose, ca] : config.cas)
      any_p11 = any_p11 || ca.key_backend == "pkcs11";
    if (any_p11) {
      std::print("\n[pkcs11]\nmodule = \"{}\"\n", config.pkcs11.module);
      if (!config.pkcs11.token_label.empty())
        std::print("token_label = \"{}\"\n", config.pkcs11.token_label);
    }

    // The resolved token label is printed on every token-held CA, not the
    // default it may have come from: the snapshot records what was used.
    auto print_ca = [](const std::string &section, const cfg::CaFields &ca) {
      std::print("\n[{}]\ncn = \"{}\"\ncurve = \"{}\"\ndigest = \"{}\"\n"
                 "valid_days = {}\nslug_prefix = \"{}\"\nslug = \"{}\"\n"
                 "key_backend = \"{}\"\n",
                 section, ca.cn, ca.curve, ca.digest, ca.valid_days,
                 ca.slug_prefix, ca.slug, ca.key_backend);
      if (ca.key_backend == "pkcs11")
        std::print("token_label = \"{}\"\n", ca.token_label);
    };
    print_ca("root", config.root);
    for (const auto &[purpose, ca] : config.cas) {
      print_ca("ca." + purpose, ca);
      std::print("profiles = [");
      for (std::size_t i = 0; i < ca.profiles.size(); ++i)
        std::print("{}\"{}\"", i ? ", " : "", ca.profiles[i]);
      std::print("]\nee_curve = \"{}\"\nee_digest = \"{}\"\n"
                 "ee_valid_days = {}\n",
                 ca.ee_curve, ca.ee_digest, ca.ee_valid_days);
      // Only when set: false is the default, so omitting it re-parses the
      // same way and keeps the snapshot free of noise.
      if (ca.simple_dn)
        std::print("simple_dn = true\n");
      auto print_subtrees = [](const char *key,
                               const std::vector<std::string> &v) {
        if (v.empty())
          return;
        std::print("{} = [", key);
        for (std::size_t i = 0; i < v.size(); ++i)
          std::print("{}\"{}\"", i ? ", " : "", v[i]);
        std::print("]\n");
      };
      print_subtrees("permitted_dns", ca.permitted_dns);
      print_subtrees("permitted_email", ca.permitted_email);
    }
    return true;
  }

  // A CRL is a file artifact; the file is named by the CA slug, guaranteed
  // to be ASCII. The aliases follow the active generation; an explicit CN
  // reaches whichever generation carries it.
  if (target == "crl") {
    auto dbh_crl = open_store(db);
    const std::string cn = resolve_ca_cn(*dbh_crl, config, selector);
    if (cn.empty())
      return false;
    const CaGen root = active_ca(*dbh_crl, config, "root");
    std::string ca_slug;
    if (cn == root.cn)
      ca_slug = root.slug;
    else if (auto g = gen_by_cn(*dbh_crl, "signing", cn))
      ca_slug = g->slug;
    else {
      log::error("crl --cn must be root-ca, <purpose>-ca, or a CA "
                 "generation's CN");
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
    std::string cn = resolve_ca_cn(*dbh, config, selector), kind;
    if (cn.empty())
      return false;
    if (selector == "root-ca")
      kind = "root";
    else if (selector != cn)
      kind = "signing"; // an alias resolved to a generation of that purpose
    auto q = dbh->stmt(
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
  if (chain)
    for (const auto &issuer : issuers_above(*dbh, *found))
      emit(issuer, encoding);
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
  std::shared_ptr<store::Statement> q;
  if (filter == "expiring") {
    q = dbh->stmt(cols +
                  "WHERE status='active' AND not_after>?1 AND "
                  "not_after<=?2 ORDER BY not_after DESC" +
                  lim);
    q->bind(1, now);
    q->bind(2, now + win);
  } else if (filter == "expired") {
    q = dbh->stmt(cols +
                  "WHERE status='active' AND not_after<?1 AND "
                  "not_after>=?2 ORDER BY not_after DESC" +
                  lim);
    q->bind(1, now);
    q->bind(2, now - win);
  } else if (filter == "revoked") {
    q = dbh->stmt(cols +
                  "WHERE status='revoked' AND revoked_at>=?1 "
                  "ORDER BY revoked_at DESC" +
                  lim);
    q->bind(1, now - win);
  } else if (filter == "last") {
    q = dbh->stmt(cols +
                  "WHERE not_before<=?1 AND not_before>=?2 "
                  "ORDER BY not_before DESC" +
                  lim);
    q->bind(1, now);
    q->bind(2, now - win);
  } else { // cn
    // A CA alias selects the role, so every generation of it is listed; a
    // literal CN selects exactly that name.
    if (cn == "root-ca" || cn == "signing-ca") {
      q = dbh->stmt(cols + "WHERE kind=?1 ORDER BY not_before DESC" + lim);
      q->bind(1, std::string(cn == "root-ca" ? "root" : "signing"));
    } else {
      q = dbh->stmt(cols + "WHERE cn=?1 ORDER BY not_before DESC" + lim);
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

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <chrono>
#include <filesystem>
#include <fstream>
#include <string>

#include <botan/botan_all.h>

#include "app.h"
#include "ca.h"
#include "ca_detail.h"
#include "config.h"
#include "log.h"
#include "util.h"

TEST_CASE("slug lowercases and hyphenates") {
  CHECK(util::slug("ETS Root E1") == "ets-root-e1");
  CHECK(util::slug("CA E1") == "ca-e1");
  CHECK(util::slug("already-lower") == "already-lower");
  CHECK(util::slug("") == "");
}

TEST_CASE("parse_ipv4 accepts valid addresses") {
  CHECK(util::parse_ipv4("0.0.0.0") == 0u);
  CHECK(util::parse_ipv4("10.0.0.5") == 0x0A000005u);
  CHECK(util::parse_ipv4("192.168.1.1") == 0xC0A80101u);
  CHECK(util::parse_ipv4("255.255.255.255") == 0xFFFFFFFFu);
}

TEST_CASE("parse_ipv4 rejects malformed addresses") {
  CHECK_FALSE(util::parse_ipv4("256.0.0.1"));     // octet > 255
  CHECK_FALSE(util::parse_ipv4("1.2.3"));         // too few octets
  CHECK_FALSE(util::parse_ipv4("1.2.3.4.5"));     // too many octets
  CHECK_FALSE(util::parse_ipv4("1.2.3."));        // empty last octet
  CHECK_FALSE(util::parse_ipv4("1.2..4"));        // empty middle octet
  CHECK_FALSE(util::parse_ipv4("1.2.3.a"));       // non-digit
  CHECK_FALSE(util::parse_ipv4("1.2.3.4444"));    // more than 3 digits
  CHECK_FALSE(util::parse_ipv4(""));              // empty
  CHECK_FALSE(util::parse_ipv4("01.2.3.4"));      // leading zero
  CHECK_FALSE(util::parse_ipv4("192.168.001.1")); // leading zeros
}

TEST_CASE("dns_safe / ascii_graphic") {
  CHECK(util::dns_safe("server.ca"));
  CHECK(util::dns_safe("*.wild.ca"));
  CHECK(util::dns_safe("xn--bcher-kva.ca")); // punycode form is fine
  CHECK_FALSE(util::dns_safe(""));
  CHECK_FALSE(util::dns_safe("has space.ca"));
  CHECK_FALSE(util::dns_safe("株.ca")); // raw IDN
  CHECK(util::ascii_graphic("p@example.ca"));
  CHECK_FALSE(util::ascii_graphic("p @example.ca")); // space
  CHECK_FALSE(util::ascii_graphic("пример@example.ca"));
  CHECK_FALSE(util::ascii_graphic(""));
}

TEST_CASE("parse_duration") {
  using std::chrono::seconds;
  CHECK(util::parse_duration("30s") == seconds(30));
  CHECK(util::parse_duration("5m") == seconds(300));
  CHECK(util::parse_duration("1h") == seconds(3600));
  CHECK(util::parse_duration("7d") == seconds(7 * 86400));
  CHECK_FALSE(util::parse_duration(""));
  CHECK_FALSE(util::parse_duration("5"));     // no unit
  CHECK_FALSE(util::parse_duration("h"));     // no number
  CHECK_FALSE(util::parse_duration("5x"));    // unknown unit
  CHECK_FALSE(util::parse_duration("-5m"));   // negative
  CHECK_FALSE(util::parse_duration("0s"));    // zero
  CHECK_FALSE(util::parse_duration("1h30m")); // single unit only
}

namespace {

const std::string VALID = R"(org_name = "Example"
country_code = "CA"
repository_host = "pki.example.ca"
root_ca_cn = "ETS Root E1"
root_ca_curve = "secp384r1"
root_ca_digest = "SHA-384"
root_ca_valid_days = 8192
root_ca_slug = "ets-root-e1"
signing_ca_cn = "CA E1"
signing_ca_curve = "secp384r1"
signing_ca_digest = "SHA-384"
signing_ca_valid_days = 8112
signing_ca_slug = "ca-e1"
ee_curve = "secp256r1"
ee_digest = "SHA-256"
ee_valid_days = 397
ocsp_cn = "CA E1 OCSP Responder"
ocsp_slug = "ca-e1-ocsp-responder"
ocsp_valid_days = 7
root_arc_oid = "1.3.6.1.4.1.00000"
)";

std::string with(const std::string &from, const std::string &to,
                 std::string s = VALID) { // nest calls for multi-field edits
  const auto p = s.find(from);
  if (p != std::string::npos)
    s.replace(p, from.size(), to);
  return s;
}

bool loads(const std::string &toml) {
  const auto path = std::filesystem::temp_directory_path() / "yca_unit.toml";
  std::ofstream(path) << toml;
  const bool ok = cfg::load(path).has_value();
  std::filesystem::remove(path);
  return ok;
}

} // namespace

TEST_CASE("cfg::load accepts a valid config") { CHECK(loads(VALID)); }

TEST_CASE("cfg::load: root_arc_oid is optional") {
  CHECK(loads(with("root_arc_oid = \"1.3.6.1.4.1.00000\"\n", "")));
  // Present means valid: empty or malformed values are still rejected.
  CHECK_FALSE(loads(
      with("root_arc_oid = \"1.3.6.1.4.1.00000\"", "root_arc_oid = \"\"")));
}

TEST_CASE("cfg::load parses values") {
  const auto path = std::filesystem::temp_directory_path() / "yca_unit2.toml";
  std::ofstream(path) << VALID;
  const auto c = cfg::load(path);
  std::filesystem::remove(path);
  REQUIRE(c.has_value());
  CHECK(c->repository_host == "pki.example.ca");
  CHECK(c->ee_valid_days == 397);
  CHECK(c->ocsp_valid_days == 7);
  CHECK(c->root_arc_oid == "1.3.6.1.4.1.00000");
  CHECK(c->ee_curve == "secp256r1");
  CHECK(c->root_ca_curve == "secp384r1");
}

TEST_CASE("cfg::load: only Botan curve names, no prime256v1 alias") {
  CHECK_FALSE(
      loads(with("ee_curve = \"secp256r1\"", "ee_curve = \"prime256v1\"")));
}

TEST_CASE("cfg::load rejects invalid configs") {
  CHECK_FALSE(loads(
      with("root_ca_digest = \"SHA-384\"", "root_ca_digest = \"sha999\"")));
  // Digests are Botan's names verbatim; the old lowercase forms are rejected.
  CHECK_FALSE(loads(
      with("root_ca_digest = \"SHA-384\"", "root_ca_digest = \"sha384\"")));
  CHECK_FALSE(loads(with("ee_digest = \"SHA-256\"", "ee_digest = \"sha256\"")));
  CHECK_FALSE(loads(
      with("root_ca_curve = \"secp384r1\"", "root_ca_curve = \"rsa2048\"")));
  CHECK_FALSE(loads(with("1.3.6.1.4.1.00000", "1.a.b")));
  CHECK_FALSE(loads(with("country_code = \"CA\"", "country_code = \"CAN\"")));
  CHECK_FALSE(loads(with("ee_valid_days = 397", "ee_valid_days = 500")));
  CHECK_FALSE(loads(with("ocsp_valid_days = 7", "ocsp_valid_days = 99")));
  CHECK_FALSE(loads(with("org_name = \"Example\"", "org_name = \"\"")));
  CHECK_FALSE(loads(with("ocsp_cn = \"CA E1 OCSP Responder\"\n", "")));

  // ee and ocsp validities must both sit under the signing CA's lifetime.
  const std::string short_ca =
      with("ee_valid_days = 397", "ee_valid_days = 20",
           with("signing_ca_valid_days = 8112", "signing_ca_valid_days = 30"));
  CHECK(loads(short_ca)); // ocsp 7 < 30: fine
  CHECK_FALSE(loads(with("ocsp_valid_days = 7", "ocsp_valid_days = 35",
                         short_ca))); // in [2,35] but outlives the signing CA
  CHECK_FALSE(loads(with("ee_valid_days = 397", "ee_valid_days = 8112")));
}

TEST_CASE("cfg::load: names are free-form UTF-8, slugs are strict ASCII") {
  // *_cn fields are DN-only (UTF8String): any script is legal.
  CHECK(loads(with("org_name = \"Example\"", "org_name = \"Компания 株\"")));
  CHECK(loads(
      with("root_ca_cn = \"ETS Root E1\"", "root_ca_cn = \"ETS 株 Root E1\"")));
  CHECK(loads(
      with("ocsp_cn = \"CA E1 OCSP Responder\"", "ocsp_cn = \"OCSP ﺵﺮﻛﺓ\"")));
  // Declared slugs go verbatim into URLs/file names: lowercase ASCII only.
  CHECK_FALSE(loads(
      with("root_ca_slug = \"ets-root-e1\"", "root_ca_slug = \"ets-株\"")));
  CHECK_FALSE(loads(with("root_ca_slug = \"ets-root-e1\"",
                         "root_ca_slug = \"ETS Root E1\""))); // case + spaces
  CHECK_FALSE(loads(with("root_ca_slug = \"ets-root-e1\"",
                         "root_ca_slug = \"ets_root_e1\""))); // kebab-case only
  CHECK_FALSE(loads(with("signing_ca_slug = \"ca-e1\"",
                         "signing_ca_slug = \"ets-root-e1\""))); // collision
  CHECK_FALSE(loads(with("signing_ca_slug = \"ca-e1\"\n", ""))); // required
}

TEST_CASE("cfg::load: key_backend defaults to internal") {
  const auto path = std::filesystem::temp_directory_path() / "yca_unit3.toml";
  std::ofstream(path) << VALID;
  const auto c = cfg::load(path);
  std::filesystem::remove(path);
  REQUIRE(c.has_value());
  CHECK(c->key_backend == "internal");
}

TEST_CASE("cfg::load: pkcs11 backend requires module and token label") {
  const std::string P11 = VALID +
                          "key_backend = \"pkcs11\"\n"
                          "pkcs11_module = \"/usr/lib/opensc-pkcs11.so\"\n";
  CHECK(loads(P11 + "pkcs11_token_label = \"yts\"\n"));
  CHECK_FALSE(loads(VALID + "key_backend = \"pkcs11\"\n")); // missing pkcs11_*
  CHECK_FALSE(loads(VALID + "key_backend = \"tpm\"\n"));    // unknown backend
  // pkcs11_* without the pkcs11 backend is a mistake, not silently ignored.
  CHECK_FALSE(loads(VALID + "pkcs11_module = \"/usr/lib/x.so\"\n"));
  // PKCS#11 caps the token label at 32 bytes.
  CHECK_FALSE(loads(P11 + "pkcs11_token_label = "
                          "\"012345678901234567890123456789012\"\n"));
}

TEST_CASE("cfg::load validates repository_host as a DNS host, optional :port") {
  auto url = [](const char *v) {
    return loads(with("repository_host = \"pki.example.ca\"",
                      std::string("repository_host = \"") + v + "\""));
  };
  CHECK(url("pki.example.ca"));
  CHECK(url("pki.example.ca:8080"));
  CHECK(url("localhost"));
  CHECK_FALSE(url("pki.example_light.ca"));  // underscore: not a DNS name
  CHECK_FALSE(url("pki.白.ca"));             // raw IDN
  CHECK_FALSE(url("pki..example.ca"));       // empty label
  CHECK_FALSE(url("-pki.example.ca"));       // leading hyphen
  CHECK_FALSE(url("pki.example.ca-"));       // trailing hyphen
  CHECK_FALSE(url("pki.example.ca/pki"));    // no path
  CHECK_FALSE(url("http://pki.example.ca")); // no scheme
  CHECK_FALSE(url("pki.example.ca:0"));      // bad port
  CHECK_FALSE(url("pki.example.ca:65536"));  // bad port
  CHECK_FALSE(url("pki.example.ca:"));       // empty port
}

// --- ca API tests: a real (tiny) PKI in a temp dir, secp256r1 for speed ---

namespace {

constexpr std::string_view kPass = "unit-test-pass";

struct TempPki {
  std::filesystem::path dir;
  cfg::Config config;

  TempPki() {
    log::config().default_level = log::Level::OFF; // keep test output clean
    dir = std::filesystem::temp_directory_path() / "yca_ca_ut";
    std::filesystem::remove_all(dir);
    config.org_name = "Example";
    config.country_code = "CA";
    config.repository_host = "pki.unit.ca";
    config.root_arc_oid = "1.3.6.1.4.1.00000";
    config.root_ca_cn = "UT Root E1";
    config.root_ca_curve = "secp256r1"; // canonical (Config built by hand,
    config.root_ca_digest = "SHA-256";  // no cfg::load validation here)
    config.root_ca_valid_days = 3650;
    config.root_ca_slug = "ut-root-e1";
    config.signing_ca_cn = "UT CA E1";
    config.signing_ca_curve = "secp256r1";
    config.signing_ca_digest = "SHA-256";
    config.signing_ca_valid_days = 3000;
    config.signing_ca_slug = "ut-ca-e1";
    config.ee_curve = "secp256r1";
    config.ee_digest = "SHA-256";
    config.ee_valid_days = 90;
    config.ocsp_cn = "UT OCSP";
    config.ocsp_slug = "ut-ocsp";
    config.ocsp_valid_days = 7;
  }
  ~TempPki() { std::filesystem::remove_all(dir); }
};

} // namespace

TEST_CASE("ca::load_config on an uninitialized dir returns nullopt") {
  TempPki t;
  CHECK_FALSE(ca::load_config(t.dir).has_value());
}

TEST_CASE("ca::is_initialized: active anchors must match the locked config") {
  TempPki t;
  CHECK_FALSE(ca::is_initialized(t.dir)); // absent store
  REQUIRE(ca::init(t.config, t.dir, kPass));
  CHECK(ca::is_initialized(t.dir));

  const auto db = t.dir / "ca-store.db";
  {
    // Locked-config drift: cert_index anchors no longer match ca_config.
    Botan::Sqlite3_Database h(db.string());
    h.new_statement("UPDATE ca_config SET value='Other CN' "
                    "WHERE key='root_ca_cn'")
        ->spin();
  }
  CHECK_FALSE(ca::is_initialized(t.dir));

  {
    std::ofstream wipe(db, std::ios::trunc);
  } // `:> ca-store.db`
  CHECK_FALSE(ca::is_initialized(t.dir));
  std::ofstream(db) << "garbage, not a SQLite file";
  CHECK_FALSE(ca::is_initialized(t.dir));
}

TEST_CASE("ca::init wants an absent or empty store_dir") {
  TempPki t;

  std::filesystem::create_directories(t.dir); // pre-existing but empty: fine
  REQUIRE(ca::init(t.config, t.dir, kPass));
  CHECK(ca::is_initialized(t.dir));
  CHECK_FALSE(ca::init(t.config, t.dir, kPass)); // healthy store: rejected

  {
    std::ofstream wipe(t.dir / "ca-store.db", std::ios::trunc);
  } // dead store
  CHECK_FALSE(ca::init(t.config, t.dir, kPass)); // refused, not repaired
  CHECK(std::filesystem::exists(t.dir / "ca-store.db")); // left untouched

  // Moving the remains away is the user's call; init then starts clean.
  const auto aside = std::filesystem::temp_directory_path() / "yca_ca_ut.dead";
  std::filesystem::remove_all(aside);
  std::filesystem::rename(t.dir, aside);
  REQUIRE(ca::init(t.config, t.dir, kPass));
  CHECK(ca::is_initialized(t.dir));
  CHECK(std::filesystem::exists(aside / "ca-store.db")); // post-mortem intact
  std::filesystem::remove_all(aside);
}

TEST_CASE("ca: CertificatePolicies present only when root_arc_oid is set") {
  {
    TempPki t;
    REQUIRE(ca::init(t.config, t.dir, kPass));
    Botan::X509_Certificate sign((t.dir / "ca" / "ut-ca-e1.pem").string());
    CHECK(sign.certificate_policy_oids().size() == 2);
  }
  {
    TempPki t;
    t.config.root_arc_oid.clear();
    REQUIRE(ca::init(t.config, t.dir, kPass));
    Botan::X509_Certificate sign((t.dir / "ca" / "ut-ca-e1.pem").string());
    CHECK(sign.certificate_policy_oids().empty());
  }
}

TEST_CASE("ca: init + issuance + revocation rules") {
  TempPki t;
  REQUIRE(ca::init(t.config, t.dir, kPass));
  CHECK(std::filesystem::exists(t.dir / "ca-store.db")); // fixed store name
  CHECK_FALSE(ca::init(t.config, t.dir, kPass));         // re-init rejected

  auto eff = ca::load_config(t.dir);
  REQUIRE(eff.has_value());
  CHECK(eff->signing_ca_cn == "UT CA E1"); // DB snapshot round-trips
  CHECK(eff->ee_valid_days == 90);

  // Server: issue; duplicate active rejected; same CN other profile fine.
  CHECK(ca::issue_ee(*eff, t.dir, kPass, ca::Profile::Server, "s.ut.ca", {}));
  CHECK_FALSE(
      ca::issue_ee(*eff, t.dir, kPass, ca::Profile::Server, "s.ut.ca", {}));
  CHECK(ca::issue_ee(*eff, t.dir, kPass, ca::Profile::Client, "s.ut.ca",
                     {{ca::San::Type::Dns, "s.ut.ca"}}));

  // Client requires a SAN; a malformed IPv4 SAN is rejected.
  CHECK_FALSE(
      ca::issue_ee(*eff, t.dir, kPass, ca::Profile::Client, "c.ut.ca", {}));
  CHECK_FALSE(ca::issue_ee(*eff, t.dir, kPass, ca::Profile::Server, "ip.ut.ca",
                           {{ca::San::Type::Ip, "999.1.1.1"}}));

  // Signing requires the right passphrase.
  CHECK_FALSE(
      ca::issue_ee(*eff, t.dir, "wrong", ca::Profile::Server, "w.ut.ca", {}));

  // Revoke: unknown reason rejected; reason names are case-insensitive;
  // nothing left active afterwards; re-issue is then allowed.
  CHECK_FALSE(ca::revoke(*eff, t.dir, kPass, "server", "s.ut.ca", "bogus"));
  CHECK(ca::revoke(*eff, t.dir, kPass, "server", "s.ut.ca", "KeyCompromise"));
  CHECK_FALSE(
      ca::revoke(*eff, t.dir, kPass, "server", "s.ut.ca", "superseded"));
  CHECK(ca::issue_ee(*eff, t.dir, kPass, ca::Profile::Server, "s.ut.ca", {}));

  // OCSP responder is a singleton per active cert.
  CHECK(ca::issue_ocsp(*eff, t.dir, kPass));
  CHECK_FALSE(ca::issue_ocsp(*eff, t.dir, kPass));
  CHECK(ca::revoke(*eff, t.dir, kPass, "ocsp", "", "superseded"));
  CHECK(ca::issue_ocsp(*eff, t.dir, kPass));

  // ASCII rules at issuance: server CN feeds a dNSName SAN (IA5), so it must
  // be a hostname; dns/email SANs likewise. A client CN is DN-only: UTF-8 ok.
  CHECK_FALSE(
      ca::issue_ee(*eff, t.dir, kPass, ca::Profile::Server, "srv★.ut.ca", {}));
  CHECK_FALSE(ca::issue_ee(*eff, t.dir, kPass, ca::Profile::Server, "u.ut.ca",
                           {{ca::San::Type::Dns, "株.ca"}}));
  CHECK_FALSE(ca::issue_ee(*eff, t.dir, kPass, ca::Profile::Client, "Ivan",
                           {{ca::San::Type::Email, "иван@ut.ca"}}));
  CHECK(ca::issue_ee(*eff, t.dir, kPass, ca::Profile::Client, "Иван Петров",
                     {{ca::San::Type::Email, "ivan@ut.ca"}}));

  // --valid override: sub-day testing knob, accepted only in [5m, 1d).
  CHECK(ca::issue_ee(*eff, t.dir, kPass, ca::Profile::Server, "short.ut.ca", {},
                     std::chrono::hours(1)));
  CHECK(ca::issue_ee(*eff, t.dir, kPass, ca::Profile::Server, "min.ut.ca", {},
                     std::chrono::minutes(5)));
  CHECK_FALSE(ca::issue_ee(*eff, t.dir, kPass, ca::Profile::Server,
                           "fast.ut.ca", {}, std::chrono::minutes(4)));
  CHECK_FALSE(ca::issue_ee(*eff, t.dir, kPass, ca::Profile::Server,
                           "long.ut.ca", {}, std::chrono::days(1)));
}

TEST_CASE("ca::reconcile syncs tunables and ignores locked/invalid fields") {
  TempPki t;
  REQUIRE(ca::init(t.config, t.dir, kPass));
  auto eff = ca::load_config(t.dir);
  REQUIRE(eff.has_value());

  cfg::Config file = t.config;
  file.ee_valid_days = 45;   // valid tunable: synced
  file.ocsp_valid_days = 99; // invalid tunable: ignored
  file.org_name = "Other";   // locked field: ignored
  ca::reconcile(file, *eff, t.dir);
  CHECK(eff->ee_valid_days == 45);
  CHECK(eff->ocsp_valid_days == 7);
  CHECK(eff->org_name == "Example");

  // The valid tunable was persisted to the DB.
  auto reloaded = ca::load_config(t.dir);
  REQUIRE(reloaded.has_value());
  CHECK(reloaded->ee_valid_days == 45);
  CHECK(reloaded->ocsp_valid_days == 7);
}

TEST_CASE("ca::reconcile: tunables must fit under the signing CA") {
  TempPki t;
  t.config.signing_ca_valid_days = 20; // short-lived test CA
  t.config.ee_valid_days = 10;
  REQUIRE(ca::init(t.config, t.dir, kPass));
  auto eff = ca::load_config(t.dir);
  REQUIRE(eff.has_value());

  cfg::Config file = *eff;
  file.ocsp_valid_days = 25; // in [2,35] but not under the signing CA: ignored
  file.ee_valid_days = 21;   // likewise
  ca::reconcile(file, *eff, t.dir);
  CHECK(eff->ocsp_valid_days == 7);
  CHECK(eff->ee_valid_days == 10);

  file.ocsp_valid_days = 15; // fits: synced
  ca::reconcile(file, *eff, t.dir);
  CHECK(eff->ocsp_valid_days == 15);
}

TEST_CASE("a leaf may not outlive its issuer") {
  log::config().default_level = log::Level::OFF; // helper logs the refusal
  Botan::AutoSeeded_RNG rng;
  Botan::ECDSA_PrivateKey key(rng, Botan::EC_Group::from_name("secp256r1"));
  Botan::X509_Cert_Options o("UT Short CA", 24 * 60 * 60); // one day
  o.CA_key();
  const auto issuer =
      Botan::X509::create_self_signed_cert(o, key, "SHA-256", rng);
  CHECK(ca::detail::outlives_issuer(issuer, std::chrono::days(2)));
  CHECK_FALSE(ca::detail::outlives_issuer(issuer, std::chrono::hours(12)));
}

TEST_CASE("renewal window: a near-expiry active cert may be superseded") {
  TempPki t;
  REQUIRE(ca::init(t.config, t.dir, kPass));
  auto eff = ca::load_config(t.dir);
  REQUIRE(eff.has_value());
  REQUIRE(
      ca::issue_ee(*eff, t.dir, kPass, ca::Profile::Server, "rw.ut.ca", {}));
  // Fresh cert (100% of its life left): the singleton rule holds.
  CHECK_FALSE(
      ca::issue_ee(*eff, t.dir, kPass, ca::Profile::Server, "rw.ut.ca", {}));

  // Age the active cert via cert_index (the uniqueness source of truth):
  // pretend a 90-day cert with `left` days remaining.
  const auto db = (t.dir / "ca-store.db").string();
  auto age = [&](int left) {
    Botan::Sqlite3_Database h(db);
    auto u = h.new_statement("UPDATE cert_index SET not_before=?1, "
                             "not_after=?2 WHERE cn='rw.ut.ca' AND "
                             "status='active'");
    const auto now = std::chrono::duration_cast<std::chrono::seconds>(
                         std::chrono::system_clock::now().time_since_epoch())
                         .count();
    u->bind(1, static_cast<std::size_t>(now - (90 - left) * 86400));
    u->bind(2, static_cast<std::size_t>(now + left * 86400));
    u->spin();
  };
  age(40); // 44% left: not yet renewable
  CHECK_FALSE(
      ca::issue_ee(*eff, t.dir, kPass, ca::Profile::Server, "rw.ut.ca", {}));
  age(10); // 11% < 33%: the overlapping successor issues
  CHECK(ca::issue_ee(*eff, t.dir, kPass, ca::Profile::Server, "rw.ut.ca", {}));
  // The fresh successor blocks a third one (no runaway chain).
  CHECK_FALSE(
      ca::issue_ee(*eff, t.dir, kPass, ca::Profile::Server, "rw.ut.ca", {}));

  // --serial picks the EXACT cert: revoke the old one, the successor stays.
  auto serial_of = [&](bool oldest) {
    Botan::Sqlite3_Database h(db);
    auto q = h.new_statement(
        std::string("SELECT serial FROM cert_index WHERE cn='rw.ut.ca' AND "
                    "status='active' ORDER BY not_before ") +
        (oldest ? "ASC" : "DESC") + " LIMIT 1");
    REQUIRE(q->step());
    return q->get_str(0);
  };
  const std::string old_serial = serial_of(true);
  const std::string new_serial = serial_of(false);
  REQUIRE(old_serial != new_serial);
  // Friendly input: lowercase + ':' separators + leading zeros normalize.
  std::string sloppy = "00";
  for (std::size_t i = 0; i < old_serial.size(); i += 2) {
    sloppy += ':';
    sloppy += static_cast<char>(std::tolower(old_serial[i]));
    if (i + 1 < old_serial.size())
      sloppy += static_cast<char>(std::tolower(old_serial[i + 1]));
  }
  CHECK(ca::revoke(*eff, t.dir, kPass, "server", "", "superseded", sloppy));
  CHECK(serial_of(false) == new_serial); // successor untouched
  CHECK_FALSE(ca::revoke(*eff, t.dir, kPass, "server", "", "superseded",
                         old_serial)); // already revoked
  CHECK_FALSE(ca::revoke(*eff, t.dir, kPass, "server", "", "superseded",
                         "DEADBEEF")); // no such serial
  // By-CN still revokes the (only remaining) active cert.
  CHECK(ca::revoke(*eff, t.dir, kPass, "server", "rw.ut.ca", "superseded"));
  CHECK_FALSE(
      ca::revoke(*eff, t.dir, kPass, "server", "rw.ut.ca", "superseded"));
}

TEST_CASE("ca::refresh_crl re-signs the scoped CRLs in place") {
  TempPki t;
  REQUIRE(ca::init(t.config, t.dir, kPass));
  auto eff = ca::load_config(t.dir);
  REQUIRE(eff.has_value());
  REQUIRE(ca::issue_ee(*eff, t.dir, kPass, ca::Profile::Server, "r.ut.ca", {}));
  REQUIRE(ca::revoke(*eff, t.dir, kPass, "server", "r.ut.ca", "superseded"));

  const auto root_path = (t.dir / "ca" / "ut-root-e1.crl").string();
  const auto sign_path = (t.dir / "ca" / "ut-ca-e1.crl").string();
  const Botan::X509_CRL root_before(root_path), sign_before(sign_path);

  CHECK_FALSE(ca::refresh_crl(*eff, t.dir, "wrong")); // bad passphrase
  REQUIRE(ca::refresh_crl(*eff, t.dir, kPass));       // default scope: all

  const Botan::X509_CRL root_after(root_path), sign_after(sign_path);
  CHECK(root_after.crl_number() == root_before.crl_number() + 1);
  CHECK(sign_after.crl_number() == sign_before.crl_number() + 1);
  // Same revocation sets: the revoked entry survives, none appears on root.
  CHECK(sign_after.get_revoked().size() == sign_before.get_revoked().size());
  CHECK(root_after.get_revoked().empty());
  CHECK(sign_after.next_update().time_since_epoch() >=
        sign_before.next_update().time_since_epoch());

  // Separate cadences: each CRL promises its own horizon (neither CA is
  // near notAfter here, so no clamping).
  CHECK(sign_after.next_update().time_since_epoch() -
            sign_after.this_update().time_since_epoch() ==
        static_cast<uint64_t>(app::crl_next_update_days) * 24 * 60 * 60);
  CHECK(root_after.next_update().time_since_epoch() -
            root_after.this_update().time_since_epoch() ==
        static_cast<uint64_t>(app::root_crl_next_update_days) * 24 * 60 * 60);

  // Signing scope: signing CRL advances, root CRL stays byte-identical.
  REQUIRE(ca::refresh_crl(*eff, t.dir, kPass, ca::CrlScope::Signing));
  const Botan::X509_CRL root_s(root_path), sign_s(sign_path);
  CHECK(sign_s.crl_number() == sign_after.crl_number() + 1);
  CHECK(root_s.crl_number() == root_after.crl_number());

  // Root scope: the mirror image.
  REQUIRE(ca::refresh_crl(*eff, t.dir, kPass, ca::CrlScope::Root));
  const Botan::X509_CRL root_r(root_path), sign_r(sign_path);
  CHECK(root_r.crl_number() == root_s.crl_number() + 1);
  CHECK(sign_r.crl_number() == sign_s.crl_number());
}

TEST_CASE("CRL nextUpdate is clamped to the issuer's notAfter") {
  Botan::AutoSeeded_RNG rng;
  Botan::ECDSA_PrivateKey key(rng, Botan::EC_Group::from_name("secp256r1"));

  Botan::X509_Cert_Options o("UT Short CA", 24 * 60 * 60); // one day
  o.CA_key();
  const auto short_ca =
      Botan::X509::create_self_signed_cert(o, key, "SHA-256", rng);
  const uint32_t nu =
      ca::detail::crl_next_update(short_ca, app::crl_next_update_days);
  CHECK(nu <= 24u * 60 * 60); // clamped to the issuer's remaining life...
  CHECK(nu > 23u * 60 * 60);  // ...not floored away
  // The clamp also caps the root horizon on a short-lived issuer.
  CHECK(ca::detail::crl_next_update(short_ca, app::root_crl_next_update_days) <=
        24u * 60 * 60);

  Botan::X509_Cert_Options o2("UT Long CA", 3650 * 24 * 60 * 60);
  o2.CA_key();
  const auto long_ca =
      Botan::X509::create_self_signed_cert(o2, key, "SHA-256", rng);
  CHECK(ca::detail::crl_next_update(long_ca, app::crl_next_update_days) ==
        static_cast<uint32_t>(app::crl_next_update_days) * 24u * 60 * 60);
  CHECK(ca::detail::crl_next_update(long_ca, app::root_crl_next_update_days) ==
        static_cast<uint32_t>(app::root_crl_next_update_days) * 24u * 60 * 60);
}

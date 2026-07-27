#include "seed.h"

#include <chrono>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <string>
#include <vector>

#include <botan/botan_all.h>

#include "app.h"
#include "ca_detail.h"
#include "log.h"
#include "util.h"

namespace ca {
namespace {

namespace fs = std::filesystem;
using Clock = std::chrono::system_clock;

} // namespace

using namespace detail;

bool seed(const cfg::Config &config, const fs::path &store_dir,
          std::string_view passphrase, int count, int same_cn) {
  const fs::path db = store_path(store_dir);
  if (!fs::exists(db)) {
    log::error("not initialized; run '{} init'", app::name);
    return false;
  }
  // The seeder issues server/client leaves, so it is that CA it loads.
  const cfg::SigningCa *ca_cfg = config.ca_for_profile("server");
  if (!ca_cfg) {
    log::error("no configured CA issues the 'server' profile");
    return false;
  }
  if (ca_cfg->key_backend == "pkcs11") {
    // Store/query tuning does not need HSM-real signatures, and millions of
    // on-token operations would take days.
    log::error("the seeder supports key_backend = \"internal\" only");
    return false;
  }
  if (passphrase.empty()) {
    log::error("{} not set (required to sign)", app::passphrase_env);
    return false;
  }

  Botan::AutoSeeded_RNG rng;
  auto dbh = open_store(db);
  Botan::Certificate_Store_In_SQL store(dbh, passphrase, rng);

  const CaGen sign = active_ca(*dbh, config, ca_cfg->purpose);
  auto sign_cert = load_ca_cert(store_dir, sign.slug);
  if (!sign_cert) {
    log::error("signing CA cert not found under {}",
               (store_dir / "ca").string());
    return false;
  }
  std::shared_ptr<const Botan::Private_Key> sign_key;
  try {
    sign_key = store.find_key(*sign_cert);
  } catch (const std::exception &e) {
    log::error("cannot load signing key (wrong passphrase?): {}", e.what());
    return false;
  }
  if (!sign_key)
    return false;

  ensure_cert_index(*dbh);
  const std::string &md = ca_cfg->ee_digest;
  Botan::X509_CA issuer(*sign_cert, *sign_key, md, rng);
  Botan::EC_Group grp = Botan::EC_Group::from_name(ca_cfg->ee_curve);
  Botan::ECDSA_PrivateKey ee_key(rng, grp); // reused for all seeded certs

  const auto now = Clock::now();
  const Botan::X509_Time nb(now - std::chrono::days(2));
  const Botan::X509_Time active_na(now +
                                   std::chrono::days(ca_cfg->ee_valid_days));
  const Botan::X509_Time expired_na(now - std::chrono::days(1));

  std::vector<Botan::CRL_Entry> revoked;

  // status: 'a' active, 'e' expired (past not_after), 'r' revoked (future
  // not_after + added to the CRL).
  const std::vector<uint8_t> ee_pub = Botan::X509::BER_encode(ee_key);
  // Deliberately thinner than a real leaf (no SAN, no AIA/CDP, no policy):
  // the seeder fills the store for query tuning, and the subject DN is the
  // one part that has to match, since `list` reads the CN back out of it.
  auto emit_cert = [&](const std::string &cn, bool server, char status) {
    Botan::Extensions ext;
    ext.add_new(
        std::make_unique<Botan::Cert_Extension::Basic_Constraints>(false),
        true);
    ext.add_new(std::make_unique<Botan::Cert_Extension::Key_Usage>(
                    Botan::Key_Constraints(static_cast<uint32_t>(
                        Botan::Key_Constraints::DigitalSignature))),
                true);
    ext.add_new(std::make_unique<Botan::Cert_Extension::Extended_Key_Usage>(
        std::vector<Botan::OID>{
            Botan::OID(server ? "1.3.6.1.5.5.7.3.1" : "1.3.6.1.5.5.7.3.2")}));
    auto cert = Botan::X509_CA::make_cert(
        issuer.signature_op(), rng, issuer.algorithm_identifier(), ee_pub, nb,
        status == 'e' ? expired_na : active_na, sign_cert->subject_dn(),
        subject_dn(config.pki, cn, ca_cfg->simple_dn), ext);
    store.insert_cert(cert);
    index_cert(*dbh, cert, server ? "server" : "client", sign.purpose,
               status == 'r' ? "revoked" : "active",
               status == 'r' ? now_epoch() : 0);
    if (status == 'r') {
      store.revoke_cert(cert, Botan::CRL_Code::Unspecified);
      revoked.emplace_back(cert, Botan::CRL_Code::Unspecified);
    }
  };

  dbh->new_statement("BEGIN")->spin();
  for (int i = 0; i < count; ++i) {
    const bool server = (i % 10) < 6; // 60% server, 40% client
    const int b = i % 20;
    const char status = server ? (b < 6 ? 'a' : (b < 15 ? 'e' : 'r'))
                               : (b < 9 ? 'a' : (b < 15 ? 'e' : 'r'));
    emit_cert((server ? "s" : "c") + std::to_string(i) + ".test.ca", server,
              status);
    if ((i + 1) % 10000 == 0) {
      dbh->new_statement("COMMIT")->spin();
      dbh->new_statement("BEGIN")->spin();
      log::to_stdout("seeded {} / {}", i + 1, count); // progress, not audit
    }
  }
  for (int j = 0; j < same_cn; ++j) // all but the last revoked
    emit_cert("same.test.ca", true, (j == same_cn - 1) ? 'a' : 'r');
  dbh->new_statement("COMMIT")->spin();

  const fs::path crl_path = store_dir / "ca" / (sign.slug + ".crl");
  Botan::X509_CRL prev(crl_path.string());
  Botan::X509_CA crl_issuer(*sign_cert, *sign_key, ca_cfg->digest, rng);
  if (!write_der(crl_path,
                 crl_issuer.update_crl(
                     prev, revoked, rng,
                     crl_next_update(*sign_cert, app::crl_next_update_days))))
    // ERROR, not WARN: warns are file-only and the operator should see this.
    log::error("could not write CRL {}", crl_path.string());

  log::to_stdout("seeded {} certs ({} revoked)", count + same_cn,
                 revoked.size());
  return true;
}

} // namespace ca

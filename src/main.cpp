#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <format>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

#include <CLI/CLI11.hpp>

#include "app.h"
#include "ca.h"
#include "config.h"
#include "log.h"
#include "profile.h"
#include "util.h"

namespace {

std::optional<ca::San> parse_san(const std::string &spec) {
  const auto colon = spec.find(':');
  if (colon == std::string::npos)
    return std::nullopt;
  const std::string type = spec.substr(0, colon);
  std::string value = spec.substr(colon + 1);
  if (value.empty())
    return std::nullopt;
  if (type == "dns")
    return ca::San{ca::San::Type::Dns, std::move(value)};
  if (type == "email")
    return ca::San{ca::San::Type::Email, std::move(value)};
  if (type == "ip")
    return ca::San{ca::San::Type::Ip, std::move(value)};
  if (type == "uri")
    return ca::San{ca::San::Type::Uri, std::move(value)};
  return std::nullopt;
}

// "server|client|email" for a help string.
std::string join(const std::vector<std::string> &v) {
  std::string out;
  for (const auto &s : v)
    out += (out.empty() ? "" : "|") + s;
  return out;
}

} // namespace

int main(int argc, char **argv) {
  CLI::App app{std::string(app::name) + " - 2-tier ECDSA CA"};
  app.fallthrough();
  app.set_version_flag("--version",
                       std::string(app::name) + " version " APP_VERSION);

  std::string config_path = std::string(app::name) + ".toml";
  std::string store_dir{app::store_dir};
  std::string log_path;
  app.add_option("--config", config_path, "config file (default yca.toml)");
  app.add_option("--store", store_dir,
                 std::string("store/artifacts directory (default ") +
                     std::string(app::store_dir) + ")");
  auto *lo_log = app.add_option("--log", log_path,
                                "log file (default yca.log next to the store)");

  auto *init = app.add_subcommand(
      "init", "initialize the PKI (create root + signing CA)");

  auto *create =
      app.add_subcommand("create", "issue an end-entity certificate");
  std::string c_target, c_cn, c_valid;
  std::vector<std::string> c_sans;
  // `create` offers only the profiles whose key it may generate; the rest
  // exist but are CSR-only (see profile.h).
  std::vector<std::string> creatable, signable;
  for (const auto &p : profile::all()) {
    signable.emplace_back(p.name);
    if (!p.csr_only)
      creatable.emplace_back(p.name);
  }
  create->add_option("target", c_target, join(creatable))
      ->required()
      ->check(CLI::IsMember(creatable));
  create->add_option("--cn", c_cn, "common name");
  create->add_option("--san", c_sans,
                     "SAN as type:name (dns|email|ip|uri), repeatable");
  create->add_option("--valid", c_valid,
                     "validity override as <N><s|m|h|d>, in [5m, "
                     "ee_valid_days] (policy is the ceiling)");

  auto *enroll =
      app.add_subcommand("enroll", "enroll an identity for CSR signing");
  std::string e_id;
  enroll->add_option("--id", e_id, "identity to enroll (e.g. an email)")
      ->required();

  auto *sign = app.add_subcommand(
      "sign", "issue an end-entity certificate from a PKCS#10 CSR");
  std::string s_target, s_id, s_nonce, s_csr, s_valid;
  sign->add_option("target", s_target, join(signable))
      ->required()
      ->check(CLI::IsMember(signable));
  sign->add_option("--id", s_id, "enrolled identity")->required();
  sign->add_option("--nonce", s_nonce, "nonce from 'get nonce --id'")
      ->required();
  sign->add_option("--csr", s_csr,
                   "CSR: inline PEM, '-' for stdin, or a file path (PEM/DER)")
      ->required();
  sign->add_option("--valid", s_valid,
                   "validity override as <N><s|m|h|d>, in [5m, "
                   "ee_valid_days] (policy is the ceiling)");

  auto *revoke = app.add_subcommand("revoke", "revoke a certificate");
  std::string rev_target, r_cn, r_reason = "unspecified", r_serial;
  std::vector<std::string> revokable = signable;
  revokable.emplace_back("ca");
  revoke->add_option("target", rev_target, join(revokable))
      ->required()
      ->check(CLI::IsMember(revokable));
  revoke->add_option("--cn", r_cn,
                     "common name; for ca, a generation's CN or signing-ca");
  revoke->add_option("--serial", r_serial,
                     "serial (hex) - selects the exact certificate; by CN "
                     "the newest active one is revoked");
  revoke->add_option("--reason", r_reason,
                     "CRLReason name (default unspecified)");

  auto *add = app.add_subcommand(
      "add", "create a declared but not yet existing CA (root key ceremony)");
  std::string a_target, a_purpose;
  add->add_option("target", a_target, "signing-ca")
      ->required()
      ->check(CLI::IsMember({"signing-ca"}));
  add->add_option("--purpose", a_purpose,
                  "the [ca.<purpose>] section to create")
      ->required();

  auto *renew = app.add_subcommand(
      "renew", "create the next generation of a CA (root key ceremony)");
  std::string n_target, n_purpose, n_new_cn;
  renew->add_option("target", n_target, "signing-ca")
      ->required()
      ->check(CLI::IsMember({"signing-ca"}));
  renew->add_option("--purpose", n_purpose, "which issuing CA to rotate");
  renew->add_option("--new-cn", n_new_cn, "the new generation's common name")
      ->required();

  auto *refresh =
      app.add_subcommand("refresh", "re-sign the CRLs with a fresh nextUpdate");
  std::string f_target, f_scope = "all";
  refresh->add_option("target", f_target, "crl")
      ->required()
      ->check(CLI::IsMember({"crl"}));
  refresh->add_option("scope", f_scope, "root|signing|all (default all)")
      ->check(CLI::IsMember({"root", "signing", "all"}));

  auto *get = app.add_subcommand(
      "get", "export a certificate, CRL, nonce or the config to stdout");
  std::string g_target, g_cn, g_id, g_encoding = "pem";
  // Every profile is exportable, plus the CA-level and store-level targets.
  std::vector<std::string> gettable = signable;
  for (const char *t : {"ca", "crl", "config", "nonce"})
    gettable.emplace_back(t);
  get->add_option("target", g_target, join(gettable))
      ->required()
      ->check(CLI::IsMember(gettable));
  get->add_option(
      "--cn", g_cn,
      "CN ('-' reads it from stdin); root-ca|signing-ca for ca/crl; "
      "unused for config/nonce");
  get->add_option("--id", g_id, "enrolled identity (nonce only)");
  get->add_option("--encoding", g_encoding, "pem|der (default pem)")
      ->check(CLI::IsMember({"pem", "der"}));
  bool g_chain = false;
  get->add_flag("--chain", g_chain,
                "append the issuers, up to but excluding the root (PEM only)");

  auto *list = app.add_subcommand("list", "list certificates by filter");
  // The window cap is the longest life any profile allows, not an
  // arbitrary year: `--expiring` must be able to cover a full EE lifetime,
  // so the issuing CA becomes visible no later than the moment issuance
  // starts refusing.
  const CLI::Range window(1, profile::max_valid_days());
  int l_expiring = app::default_list_window_days;
  int l_expired = app::default_list_window_days;
  int l_revoked = app::default_list_window_days;
  int l_last = app::default_list_window_days;
  std::string l_cn;
  bool l_tsv = false;
  auto *lo_expiring =
      list->add_option("--expiring", l_expiring, "expiring within N days")
          ->expected(0, 1)
          ->check(window);
  auto *lo_expired =
      list->add_option("--expired", l_expired, "expired within last N days")
          ->expected(0, 1)
          ->check(window);
  auto *lo_revoked =
      list->add_option("--revoked", l_revoked, "revoked within last N days")
          ->expected(0, 1)
          ->check(window);
  auto *lo_last =
      list->add_option("--last", l_last, "issued within last N days")
          ->expected(0, 1)
          ->check(window);
  auto *lo_cn =
      list->add_option("--cn", l_cn, "by CN (root-ca|signing-ca or literal)");
  list->add_flag("--tsv", l_tsv, "tab-separated output");
  int l_limit = app::default_list_limit;
  list->add_option("--limit", l_limit,
                   std::format("max rows, 0 for all (default {})",
                               app::default_list_limit))
      ->check(CLI::NonNegativeNumber);

  CLI11_PARSE(app, argc, argv);

  // No subcommand: show help.
  if (!*init && !*create && !*enroll && !*sign && !*revoke && !*add &&
      !*renew && !*refresh && !*get && !*list) {
    std::fputs(app.help().c_str(), stdout);
    return 0;
  }

  // Log file next to the store dir. An empty --log falls back to the
  // default, an unwritable path is fatal.
  if (lo_log->count() && !log_path.empty()) {
    log::set_file(log_path);
  } else {
    const auto p = log::default_path(store_dir);
    log::set_file(p);
    if (lo_log->count()) {
      log::warn("empty --log ignored; logging to {}", p.string());
      // User's flag was overridden, mirror the notice to the console.
      log::to_stderr("empty --log ignored; logging to {}", p.string());
    }
  }

  try {
    auto config = cfg::load(config_path);
    if (!config) {
      for (const auto &e : config.error())
        log::error("config: {}", e);
      log::fatal("invalid configuration");
    }

    // The CA secrets from the environment; which ones an operation needs
    // depends on the key layout (see ca::Secrets).
    auto env = [](const char *k) {
      const char *v = std::getenv(k);
      return std::string_view(v ? v : "");
    };
    ca::Secrets secrets{env(app::passphrase_env), env(app::pin_env),
                        env(app::root_pin_env)};
    if (secrets.root_pin.empty()) // identical-PIN and single-token layouts
      secrets.root_pin = secrets.pin;

    if (*init)
      return ca::init(*config, store_dir, secrets) ? 0 : 1;

    // after init the DB has the effective config.
    if (!ca::is_initialized(store_dir)) {
      log::error("not initialized; run '{} init'", app::name);
      return 1;
    }
    auto eff = ca::load_config(store_dir);
    if (!eff) { // safety net; is_initialized implies ca_config exists
      log::error("not initialized; run '{} init'", app::name);
      return 1;
    }

    if (*create) {
      ca::reconcile(*config, *eff);
      if (c_cn.empty()) {
        log::error("--cn is required for create {}", c_target);
        return 1;
      }
      std::vector<ca::San> sans;
      for (const auto &s : c_sans) {
        if (auto san = parse_san(s)) {
          sans.push_back(*san);
        } else {
          log::error("invalid --san: {}", s);
          return 1;
        }
      }
      std::optional<std::chrono::seconds> valid;
      if (!c_valid.empty()) {
        valid = util::parse_duration(c_valid);
        if (!valid) {
          log::error("invalid --valid '{}' (use <N><s|m|h|d>, e.g. 1h)",
                     c_valid);
          return 1;
        }
      }
      return ca::issue_ee(*eff, store_dir, secrets, c_target, c_cn, sans, valid)
                 ? 0
                 : 1;
    }

    if (*enroll)
      return ca::enroll(store_dir, e_id) ? 0 : 1;

    if (*sign) {
      ca::reconcile(*config, *eff);
      std::optional<std::chrono::seconds> valid;
      if (!s_valid.empty()) {
        valid = util::parse_duration(s_valid);
        if (!valid) {
          log::error("invalid --valid '{}' (use <N><s|m|h|d>, e.g. 1h)",
                     s_valid);
          return 1;
        }
      }
      return ca::sign_csr(*eff, store_dir, secrets, s_target, s_id, s_nonce,
                          s_csr, valid)
                 ? 0
                 : 1;
    }

    if (*revoke) {
      if (rev_target == "ca") {
        if (!r_serial.empty()) {
          log::error("revoke ca selects by --cn, not --serial");
          return 1;
        }
        return ca::revoke_ca(*eff, store_dir, secrets, r_cn, r_reason) ? 0 : 1;
      }
      if (r_cn.empty() == r_serial.empty()) {
        log::error("revoke {} needs exactly one of --cn or --serial",
                   rev_target);
        return 1;
      }
      return ca::revoke(*eff, store_dir, secrets, rev_target, r_cn, r_reason,
                        r_serial)
                 ? 0
                 : 1;
    }

    if (*add) {
      // `add` reads the FILE: the section it creates is by definition not in
      // the store's snapshot yet. Everything else runs off `eff`.
      ca::reconcile(*config, *eff);
      return ca::add_signing_ca(*config, store_dir, secrets, a_purpose) ? 0 : 1;
    }

    if (*renew) {
      // One issuing CA needs no naming; several do.
      if (n_purpose.empty()) {
        if (eff->cas.size() != 1) {
          log::error("--purpose is required with several issuing CAs");
          return 1;
        }
        n_purpose = eff->cas.begin()->first;
      }
      return ca::renew_signing_ca(*eff, store_dir, secrets, n_purpose, n_new_cn)
                 ? 0
                 : 1;
    }

    if (*refresh) {
      const ca::CrlScope scope = f_scope == "root"      ? ca::CrlScope::Root
                                 : f_scope == "signing" ? ca::CrlScope::Signing
                                                        : ca::CrlScope::All;
      return ca::refresh_crl(*eff, store_dir, secrets, scope) ? 0 : 1;
    }

    if (*get) {
      if (g_target == "nonce") {
        if (g_id.empty()) {
          log::error("--id is required for get nonce");
          return 1;
        }
        return ca::get_nonce(store_dir, g_id) ? 0 : 1;
      }
      if (g_target != "config" && g_cn.empty()) {
        log::error("--cn is required for get {}", g_target);
        return 1;
      }
      if (g_cn == "-") { // piped from `sign`: the CN is the first stdin line
        std::getline(std::cin, g_cn);
        if (g_cn.empty()) {
          log::error("--cn - given but stdin provided no CN");
          return 1;
        }
      }
      if (g_chain) {
        // Any leaf profile, plus a CA generation: a chain is meaningful
        // wherever there is an issuer above. Derived from the profile
        // table rather than listed, so a new profile is not left out.
        if (g_target != "ca" && !profile::find(g_target)) {
          log::error("--chain applies to {} or ca", join(signable));
          return 1;
        }
        // A chain is a concatenation, and DER cannot be concatenated.
        if (g_encoding == "der") {
          log::error("--chain requires pem encoding");
          return 1;
        }
      }
      return ca::get_cert(*eff, store_dir, g_target, g_cn, g_encoding, g_chain)
                 ? 0
                 : 1;
    }

    if (*list) {
      const int nf = static_cast<int>(
          lo_expiring->count() + lo_expired->count() + lo_revoked->count() +
          lo_last->count() + lo_cn->count());
      if (nf != 1) {
        log::error("list needs exactly one of "
                   "--expiring/--expired/--revoked/--last/--cn");
        return 1;
      }
      std::string filter = "cn";
      int days = app::default_list_window_days;
      if (lo_expiring->count()) {
        filter = "expiring";
        days = l_expiring;
      } else if (lo_expired->count()) {
        filter = "expired";
        days = l_expired;
      } else if (lo_revoked->count()) {
        filter = "revoked";
        days = l_revoked;
      } else if (lo_last->count()) {
        filter = "last";
        days = l_last;
      }
      if (days < 1) // no value: CLI11 leaves 0, use the default
        days = app::default_list_window_days;
      return ca::list_certs(store_dir, filter, days, l_cn, l_tsv, l_limit) ? 0
                                                                           : 1;
    }

    return 0;
  } catch (const std::exception &e) {
    log::error("unexpected error: {}", e.what());
    return 1;
  }
}

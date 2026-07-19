#include "config.h"

#include <array>
#include <cstdint>
#include <format>
#include <span>
#include <string_view>

#include <botan/botan_all.h>
#include <toml++/toml.hpp>

#include "app.h"

namespace cfg {
namespace {

constexpr std::array<std::string_view, 3> kCurves = {"secp256r1", "secp384r1",
                                                     "secp521r1"};
// Botan's canonical hash names, passed to Botan verbatim.
constexpr std::array<std::string_view, 3> kDigests = {"SHA-256", "SHA-384",
                                                      "SHA-512"};

bool one_of(std::span<const std::string_view> set, std::string_view v) {
  for (auto s : set)
    if (s == v)
      return true;
  return false;
}

bool two_letters(std::string_view s) {
  auto alpha = [](char c) {
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
  };
  return s.size() == 2 && alpha(s[0]) && alpha(s[1]);
}

// Slugs are declared identifiers that end up verbatim in AIA/CDP URIs
// (IA5String, ASCII-only) and in file names: lowercase kebab-case.
// Names (org_name, *_cn) are DN-only (UTF8String), where any script is legal.
bool valid_slug(std::string_view s) {
  for (char c : s) {
    const bool ok = (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') ||
                    c == '.' || c == '-';
    if (!ok)
      return false;
  }
  return true;
}

// DNS host name (RFC 1123): dot-separated labels of [A-Za-z0-9-], 1..63 chars
// each, no leading/trailing hyphen. Notably no underscore - resolvers reject
// it, and base_url goes verbatim into the AIA/CDP URLs.
bool valid_hostname(std::string_view s) {
  if (s.empty() || s.size() > 253)
    return false;
  std::size_t label = 0;
  for (std::size_t i = 0; i < s.size(); ++i) {
    const char c = s[i];
    if (c == '.') {
      if (label == 0 || s[i - 1] == '-')
        return false;
      label = 0;
      continue;
    }
    const bool alnum = (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') ||
                       (c >= '0' && c <= '9');
    if (!alnum && c != '-')
      return false;
    if (c == '-' && label == 0)
      return false;
    if (++label > 63)
      return false;
  }
  return label > 0 && s.back() != '-';
}

// repository_host = host name, optionally with a ":port" (1..65535). No
// scheme, no path: the code prepends "http://" and appends artifact paths.
bool valid_host_port(std::string_view s) {
  if (const auto colon = s.find(':'); colon != std::string_view::npos) {
    const auto port = s.substr(colon + 1);
    if (port.empty() || port.size() > 5)
      return false;
    unsigned v = 0;
    for (char c : port) {
      if (c < '0' || c > '9')
        return false;
      v = v * 10 + static_cast<unsigned>(c - '0');
    }
    if (v < 1 || v > 65535)
      return false;
    s = s.substr(0, colon);
  }
  return valid_hostname(s);
}

} // namespace

std::expected<Config, std::vector<std::string>>
load(const std::filesystem::path &path) {
  std::vector<std::string> errs;

  if (!std::filesystem::exists(path)) {
    errs.push_back(std::format("config not found: {}", path.string()));
    return std::unexpected(std::move(errs));
  }

  toml::table tbl;
  try {
    tbl = toml::parse_file(path.string());
  } catch (const toml::parse_error &e) {
    const auto &b = e.source().begin;
    errs.push_back(std::format("{}:{}:{}: parse error: {}", path.string(),
                               b.line, b.column, e.description()));
    return std::unexpected(std::move(errs));
  }

  Config c;

  auto get_str = [&](std::string_view key, std::string &out) {
    if (auto v = tbl[key].value<std::string>()) {
      out = *v;
      if (out.empty()) {
        errs.push_back(std::format("empty value for key: {}", key));
        return false;
      }
      return true;
    }
    errs.push_back(std::format("missing or non-string key: {}", key));
    return false;
  };
  auto get_int = [&](std::string_view key, int &out) {
    if (auto v = tbl[key].value<int64_t>()) {
      out = static_cast<int>(*v);
      return true;
    }
    errs.push_back(std::format("missing or non-integer key: {}", key));
    return false;
  };
  // Only Botan's SECG curve names are accepted - no aliases (prime256v1 is
  // OpenSSL's name for secp256r1 and is rejected).
  auto check_curve = [&](std::string_view key, const std::string &v) {
    if (!one_of(kCurves, v))
      errs.push_back(
          std::format("{}: invalid curve '{}' (allowed: secp256r1, secp384r1, "
                      "secp521r1)",
                      key, v));
  };
  auto check_digest = [&](std::string_view key, const std::string &v) {
    if (!one_of(kDigests, v))
      errs.push_back(std::format(
          "{}: invalid digest '{}' (allowed: SHA-256, SHA-384, SHA-512)", key,
          v));
  };

  auto check_slug = [&](std::string_view key, const std::string &v) {
    if (!valid_slug(v))
      errs.push_back(std::format(
          "{}: only lowercase ASCII [a-z0-9.-] allowed (used verbatim in "
          "URLs and file names), got '{}'",
          key, v));
  };

  if (get_str("country_code", c.country_code) && !two_letters(c.country_code))
    errs.push_back(std::format("country_code must be 2 letters, got '{}'",
                               c.country_code));
  get_str("org_name", c.org_name); // DN-only: any UTF-8 is fine
  if (get_str("repository_host", c.repository_host) &&
      !valid_host_port(c.repository_host))
    errs.push_back(std::format(
        "repository_host: must be a DNS host name, optionally with :port "
        "(no underscores, scheme, or path), got '{}'",
        c.repository_host));
  get_str("root_ca_cn", c.root_ca_cn);       // DN-only
  get_str("signing_ca_cn", c.signing_ca_cn); // DN-only
  // Slug prefixes: the branded, stable part of the CA slugs. The full
  // slug is <prefix><generation> - generation 1 at init, incremented by
  // CA rotation. Prefixes end up verbatim in file
  // names, AIA/CDP URLs and HSM key labels, so the slug charset applies.
  if (get_str("root_ca_slug_prefix", c.root_ca_slug_prefix))
    check_slug("root_ca_slug_prefix", c.root_ca_slug_prefix);
  if (get_str("signing_ca_slug_prefix", c.signing_ca_slug_prefix))
    check_slug("signing_ca_slug_prefix", c.signing_ca_slug_prefix);
  // Distinct prefixes must stay distinct across generations: one being
  // the other plus digits would collide later (root "ca-e" would meet
  // signing "ca-e1" at generation 11).
  auto digits_apart = [](const std::string &a, const std::string &b) {
    if (a.size() > b.size() || b.compare(0, a.size(), a) != 0)
      return false;
    for (std::size_t i = a.size(); i < b.size(); ++i)
      if (b[i] < '0' || b[i] > '9')
        return false;
    return true;
  };
  if (!c.root_ca_slug_prefix.empty() &&
      (digits_apart(c.root_ca_slug_prefix, c.signing_ca_slug_prefix) ||
       digits_apart(c.signing_ca_slug_prefix, c.root_ca_slug_prefix)))
    errs.push_back("root_ca_slug_prefix and signing_ca_slug_prefix must "
                   "differ (and not only by digits): their generation-"
                   "numbered files share pki/ca/");
  c.root_ca_slug = c.root_ca_slug_prefix + "1";
  c.signing_ca_slug = c.signing_ca_slug_prefix + "1";

  // key_backend is optional (default "internal"). With "pkcs11" both pkcs11_*
  // fields are required; with "internal" they must be absent - a set-but-
  // unused HSM field is a config mistake, not something to ignore.
  if (tbl.contains("key_backend") && get_str("key_backend", c.key_backend) &&
      c.key_backend != "internal" && c.key_backend != "pkcs11")
    errs.push_back(
        std::format("key_backend must be \"internal\" or \"pkcs11\", got '{}'",
                    c.key_backend));
  if (c.key_backend == "pkcs11") {
    get_str("pkcs11_module", c.pkcs11_module);
    if (get_str("pkcs11_token_label", c.pkcs11_token_label) &&
        c.pkcs11_token_label.size() > app::p11_token_label_max)
      errs.push_back(std::format("pkcs11_token_label: at most {} bytes "
                                 "(PKCS#11 limit)",
                                 app::p11_token_label_max));
  } else {
    for (const char *k : {"pkcs11_module", "pkcs11_token_label"})
      if (tbl.contains(k))
        errs.push_back(
            std::format("{} is set but key_backend is not \"pkcs11\"", k));
  }

  // Optional: without it, certificates carry no CertificatePolicies extension.
  if (tbl.contains("root_arc_oid") && get_str("root_arc_oid", c.root_arc_oid)) {
    try {
      static_cast<void>(Botan::OID(c.root_arc_oid));
    } catch (const std::exception &) {
      errs.push_back(
          std::format("root_arc_oid: invalid OID '{}'", c.root_arc_oid));
    }
  }

  if (get_str("root_ca_curve", c.root_ca_curve))
    check_curve("root_ca_curve", c.root_ca_curve);
  if (get_str("root_ca_digest", c.root_ca_digest))
    check_digest("root_ca_digest", c.root_ca_digest);
  if (get_str("signing_ca_curve", c.signing_ca_curve))
    check_curve("signing_ca_curve", c.signing_ca_curve);
  if (get_str("signing_ca_digest", c.signing_ca_digest))
    check_digest("signing_ca_digest", c.signing_ca_digest);
  if (get_str("ee_curve", c.ee_curve))
    check_curve("ee_curve", c.ee_curve);
  if (get_str("ee_digest", c.ee_digest))
    check_digest("ee_digest", c.ee_digest);

  bool rd = get_int("root_ca_valid_days", c.root_ca_valid_days);
  bool sd = get_int("signing_ca_valid_days", c.signing_ca_valid_days);
  bool ed = get_int("ee_valid_days", c.ee_valid_days);

  if (rd && c.root_ca_valid_days <= 0)
    errs.push_back("root_ca_valid_days must be > 0");
  if (sd && c.signing_ca_valid_days <= 0)
    errs.push_back("signing_ca_valid_days must be > 0");
  if (ed && c.ee_valid_days <= 0)
    errs.push_back("ee_valid_days must be > 0");
  if (ed && c.ee_valid_days > app::max_ee_valid_days)
    errs.push_back(
        std::format("ee_valid_days cannot exceed {}", app::max_ee_valid_days));
  // These duration comparisons match the certificate lifetimes only at init
  // (everything is issued at t=0); once the signing CA ages, the runtime
  // guard is detail::outlives_issuer at each issuance.
  if (ed && sd && !(c.ee_valid_days < c.signing_ca_valid_days))
    errs.push_back("ee_valid_days must be < signing_ca_valid_days");
  if (sd && rd && !(c.signing_ca_valid_days < c.root_ca_valid_days))
    errs.push_back("signing_ca_valid_days must be < root_ca_valid_days");

  if (!errs.empty())
    return std::unexpected(std::move(errs));
  return c;
}

} // namespace cfg

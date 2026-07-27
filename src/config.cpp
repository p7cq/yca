#include "config.h"

#include <array>
#include <cstdint>
#include <format>
#include <span>
#include <string_view>

#include <botan/botan_all.h>
#include <toml++/toml.hpp>

#include "app.h"
#include "profile.h"

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

std::string joined(std::span<const std::string_view> set) {
  std::string out;
  for (auto s : set) {
    if (!out.empty())
      out += ", ";
    out += s;
  }
  return out;
}

bool two_letters(std::string_view s) {
  auto alpha = [](char c) {
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
  };
  return s.size() == 2 && alpha(s[0]) && alpha(s[1]);
}

// Slugs are declared identifiers that end up verbatim in AIA/CDP URIs
// (IA5String, ASCII-only) and in file names: lowercase kebab-case.
// Names (org_name, cn) are DN-only (UTF8String), where any script is legal.
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
// it, and repository_host goes verbatim into the AIA/CDP URLs.
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

// Two slug prefixes collide once generations are appended when one is the
// other plus digits: root "ca-e" would meet signing "ca-e1" at generation
// 11, and their files share pki/ca/.
bool digits_apart(const std::string &a, const std::string &b) {
  if (a.size() > b.size() || b.compare(0, a.size(), a) != 0)
    return false;
  for (std::size_t i = a.size(); i < b.size(); ++i)
    if (b[i] < '0' || b[i] > '9')
      return false;
  return true;
}

} // namespace

const SigningCa *Config::ca(const std::string &purpose) const {
  auto it = cas.find(purpose);
  return it == cas.end() ? nullptr : &it->second;
}

const SigningCa *Config::ca_for_profile(const std::string &profile) const {
  for (const auto &[purpose, ca] : cas)
    for (const auto &p : ca.profiles)
      if (p == profile)
        return &ca;
  return nullptr;
}

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

  // Errors name the section and key the way the file spells them, so a
  // message points at a line the operator can find.
  auto get_str = [&](const toml::table &t, std::string_view sec,
                     std::string_view key, std::string &out) {
    if (auto v = t[key].value<std::string>()) {
      out = *v;
      if (out.empty()) {
        errs.push_back(std::format("[{}] {}: empty value", sec, key));
        return false;
      }
      return true;
    }
    errs.push_back(std::format("[{}] {}: missing or not a string", sec, key));
    return false;
  };
  auto get_int = [&](const toml::table &t, std::string_view sec,
                     std::string_view key, int &out) {
    if (auto v = t[key].value<int64_t>()) {
      out = static_cast<int>(*v);
      return true;
    }
    errs.push_back(std::format("[{}] {}: missing or not an integer", sec, key));
    return false;
  };
  auto get_bool = [&](const toml::table &t, std::string_view sec,
                      std::string_view key, bool &out) {
    if (auto v = t[key].value<bool>()) {
      out = *v;
      return true;
    }
    errs.push_back(std::format("[{}] {}: must be a boolean", sec, key));
    return false;
  };
  // Only Botan's SECG curve names are accepted - no aliases (prime256v1 is
  // OpenSSL's name for secp256r1 and is rejected).
  auto check_curve = [&](std::string_view sec, std::string_view key,
                         const std::string &v) {
    if (!one_of(kCurves, v))
      errs.push_back(std::format("[{}] {}: invalid curve '{}' (allowed: {})",
                                 sec, key, v, joined(kCurves)));
  };
  auto check_digest = [&](std::string_view sec, std::string_view key,
                          const std::string &v) {
    if (!one_of(kDigests, v))
      errs.push_back(std::format("[{}] {}: invalid digest '{}' (allowed: {})",
                                 sec, key, v, joined(kDigests)));
  };
  auto check_slug = [&](std::string_view sec, std::string_view key,
                        const std::string &v) {
    if (!valid_slug(v))
      errs.push_back(std::format(
          "[{}] {}: only lowercase ASCII [a-z0-9.-] allowed (used verbatim "
          "in URLs and file names), got '{}'",
          sec, key, v));
  };
  auto check_backend = [&](std::string_view sec, const std::string &v) {
    if (v != "internal" && v != "pkcs11")
      errs.push_back(std::format(
          "[{}] key_backend: must be \"internal\" or \"pkcs11\", got '{}'", sec,
          v));
  };
  auto require_table = [&](std::string_view name) -> const toml::table * {
    const auto node = tbl[name];
    if (!node) {
      errs.push_back(std::format("missing section [{}]", name));
      return nullptr;
    }
    const toml::table *t = node.as_table();
    if (!t)
      errs.push_back(std::format("[{}] must be a section", name));
    return t;
  };

  // The fields shared by [root] and every [ca.*]; the caller adds what is
  // specific to its kind.
  auto read_ca = [&](const toml::table &t, const std::string &sec,
                     CaFields &ca) {
    get_str(t, sec, "cn", ca.cn); // DN-only: any UTF-8 is fine
    if (get_str(t, sec, "curve", ca.curve))
      check_curve(sec, "curve", ca.curve);
    if (get_str(t, sec, "digest", ca.digest))
      check_digest(sec, "digest", ca.digest);
    if (get_int(t, sec, "valid_days", ca.valid_days) && ca.valid_days <= 0)
      errs.push_back(std::format("[{}] valid_days: must be > 0", sec));
    if (get_str(t, sec, "slug_prefix", ca.slug_prefix))
      check_slug(sec, "slug_prefix", ca.slug_prefix);
    ca.slug = ca.slug_prefix + "1";
    if (t.contains("key_backend") &&
        get_str(t, sec, "key_backend", ca.key_backend))
      check_backend(sec, ca.key_backend);
    if (t.contains("token_label"))
      get_str(t, sec, "token_label", ca.token_label);
  };

  if (const toml::table *t = require_table("pki")) {
    get_str(*t, "pki", "org_name", c.pki.org_name); // DN-only
    if (get_str(*t, "pki", "country_code", c.pki.country_code) &&
        !two_letters(c.pki.country_code))
      errs.push_back(std::format("[pki] country_code: must be 2 letters, got "
                                 "'{}'",
                                 c.pki.country_code));
    if (get_str(*t, "pki", "repository_host", c.pki.repository_host) &&
        !valid_host_port(c.pki.repository_host))
      errs.push_back(
          std::format("[pki] repository_host: must be a DNS host name, "
                      "optionally with :port (no underscores, scheme, or "
                      "path), got '{}'",
                      c.pki.repository_host));
    // Optional: without it, certificates carry no CertificatePolicies.
    if (t->contains("arc_oid") && get_str(*t, "pki", "arc_oid", c.pki.arc_oid))
      try {
        static_cast<void>(Botan::OID(c.pki.arc_oid));
      } catch (const std::exception &) {
        errs.push_back(
            std::format("[pki] arc_oid: invalid OID '{}'", c.pki.arc_oid));
      }
  }

  // Optional section: read now, required-ness decided once the backends
  // are known.
  const toml::table *p11 = tbl["pkcs11"].as_table();
  if (tbl.contains("pkcs11") && !p11)
    errs.push_back("[pkcs11] must be a section");
  if (p11) {
    if (p11->contains("module"))
      get_str(*p11, "pkcs11", "module", c.pkcs11.module);
    if (p11->contains("token_label"))
      get_str(*p11, "pkcs11", "token_label", c.pkcs11.token_label);
  }

  if (const toml::table *t = require_table("root"))
    read_ca(*t, "root", c.root);

  // [ca.<purpose>]: the purpose is the section key, so TOML itself rejects
  // a duplicate rather than this loader having to.
  const toml::table *cas = tbl["ca"].as_table();
  if (!cas) {
    errs.push_back(tbl.contains("ca") ? "[ca] must hold [ca.<purpose>] "
                                        "sections"
                                      : "no issuing CA: declare a "
                                        "[ca.<purpose>] section");
  } else {
    for (const auto &[key, node] : *cas) {
      const std::string purpose(key.str());
      const std::string sec = "ca." + purpose;
      const toml::table *t = node.as_table();
      if (!t) {
        errs.push_back(std::format("[{}] must be a section", sec));
        continue;
      }
      SigningCa ca;
      ca.purpose = purpose;
      if (!valid_slug(purpose))
        errs.push_back(std::format(
            "[{}]: the purpose is used verbatim in messages and selectors; "
            "only lowercase ASCII [a-z0-9.-] allowed",
            sec));
      if (purpose == "root")
        errs.push_back("[ca.root]: 'root' names the trust anchor, which is "
                       "declared in [root]; pick another purpose");
      read_ca(*t, sec, ca);
      if (const auto *arr = (*t)["profiles"].as_array()) {
        for (const auto &p : *arr) {
          const auto name = p.value<std::string>();
          if (!name) {
            errs.push_back(std::format("[{}] profiles: entries must be "
                                       "strings",
                                       sec));
            continue;
          }
          if (!profile::find(*name)) {
            std::string known;
            for (const auto &d : profile::all())
              known += std::format("{}{}", known.empty() ? "" : ", ", d.name);
            errs.push_back(std::format("[{}] profiles: unknown profile '{}' "
                                       "(known: {})",
                                       sec, *name, known));
          }
          ca.profiles.push_back(*name);
        }
        if (ca.profiles.empty())
          errs.push_back(std::format("[{}] profiles: must list at least one "
                                     "profile; a CA that issues nothing has "
                                     "no reason to exist",
                                     sec));
      } else {
        errs.push_back(
            std::format("[{}] profiles: missing or not an array", sec));
      }
      // The DN shape is the profile's, the knob is the deployment's, and
      // the knob may only widen: a profile whose subject must carry the
      // organizational attributes refuses it.
      if (t->contains("simple_dn") &&
          get_bool(*t, sec, "simple_dn", ca.simple_dn) && ca.simple_dn)
        for (const auto &name : ca.profiles)
          if (const profile::Def *d = profile::find(name); d && d->full_dn)
            errs.push_back(std::format(
                "[{}] simple_dn: the '{}' profile requires an organizational "
                "subject (C and O beside the CN)",
                sec, name));
      // nameConstraints subtrees. A dNSName is a bare FQDN and covers its
      // subdomains by label suffix. An rfc822Name is a FQDN, optionally
      // preceded by a full stop, and never a mailbox: "example.ca" means
      // every mailbox at that host, ".example.ca" every mailbox in a
      // subdomain of it, and the two do not overlap.
      auto read_subtrees = [&](std::string_view field, bool email,
                               std::vector<std::string> &out) {
        const auto *arr = (*t)[field].as_array();
        if (!arr) {
          if (t->contains(field))
            errs.push_back(
                std::format("[{}] {}: must be an array", sec, field));
          return;
        }
        for (const auto &v : *arr) {
          const auto name = v.value<std::string>();
          if (!name) {
            errs.push_back(
                std::format("[{}] {}: entries must be strings", sec, field));
            continue;
          }
          std::string_view host = *name;
          if (email && host.starts_with('.'))
            host.remove_prefix(1);
          if (email && name->find('@') != std::string::npos)
            errs.push_back(std::format(
                "[{}] {}: '{}' is a mailbox; a constraint is a domain, "
                "optionally preceded by a full stop",
                sec, field, *name));
          else if (!valid_hostname(host))
            errs.push_back(std::format("[{}] {}: '{}' is not a DNS host name",
                                       sec, field, *name));
          else
            out.push_back(*name);
        }
        if (out.empty())
          errs.push_back(std::format(
              "[{}] {}: must list at least one subtree when present", sec,
              field));
      };
      read_subtrees("permitted_dns", false, ca.permitted_dns);
      read_subtrees("permitted_email", true, ca.permitted_email);

      if (get_str(*t, sec, "ee_curve", ca.ee_curve))
        check_curve(sec, "ee_curve", ca.ee_curve);
      if (get_str(*t, sec, "ee_digest", ca.ee_digest))
        check_digest(sec, "ee_digest", ca.ee_digest);
      if (get_int(*t, sec, "ee_valid_days", ca.ee_valid_days)) {
        if (ca.ee_valid_days <= 0)
          errs.push_back(std::format("[{}] ee_valid_days: must be > 0", sec));
        // One ceiling covers every profile the CA carries, so the binding
        // one is the strictest: a CA issuing both server (398) and email
        // (825) may not hand a server certificate 825 days.
        int cap = 0;
        std::string capped_by;
        for (const auto &name : ca.profiles)
          if (const profile::Def *d = profile::find(name))
            if (cap == 0 || d->max_valid_days < cap) {
              cap = d->max_valid_days;
              capped_by = name;
            }
        if (cap > 0 && ca.ee_valid_days > cap)
          errs.push_back(
              std::format("[{}] ee_valid_days: cannot exceed {}, the ceiling "
                          "of the '{}' profile",
                          sec, cap, capped_by));
        // These duration comparisons match the certificate lifetimes only
        // at init (everything is issued at t=0); once the CA ages, the
        // runtime guard is detail::outlives_issuer at each issuance.
        if (ca.valid_days > 0 && ca.ee_valid_days >= ca.valid_days)
          errs.push_back(std::format("[{}] ee_valid_days: must be < "
                                     "valid_days ({})",
                                     sec, ca.valid_days));
      }
      if (c.root.valid_days > 0 && ca.valid_days > 0 &&
          ca.valid_days >= c.root.valid_days)
        errs.push_back(std::format("[{}] valid_days: must be < [root] "
                                   "valid_days ({})",
                                   sec, c.root.valid_days));
      c.cas.emplace(purpose, std::move(ca));
    }
  }

  // Names shared across CAs: a slug collision silently overwrites published
  // artifacts, and a CN collision makes a generation unidentifiable in
  // cert_index and in a CRL's issuer field.
  std::map<std::string, std::string> slugs, cns;
  auto claim = [&](std::map<std::string, std::string> &taken,
                   const std::string &value, std::string_view what,
                   const std::string &sec) {
    if (value.empty())
      return;
    auto [it, fresh] = taken.emplace(value, sec);
    if (!fresh)
      errs.push_back(std::format("[{}] and [{}] share the same {} '{}'",
                                 it->second, sec, what, value));
  };
  claim(slugs, c.root.slug_prefix, "slug_prefix", "root");
  claim(cns, c.root.cn, "cn", "root");
  std::map<std::string, std::string> profile_owner;
  for (const auto &[purpose, ca] : c.cas) {
    const std::string sec = "ca." + purpose;
    claim(slugs, ca.slug_prefix, "slug_prefix", sec);
    claim(cns, ca.cn, "cn", sec);
    for (const auto &p : ca.profiles) {
      auto [it, fresh] = profile_owner.emplace(p, sec);
      if (!fresh)
        errs.push_back(std::format(
            "profile '{}' is claimed by both [{}] and [{}]; each profile "
            "belongs to exactly one CA",
            p, it->second, sec));
    }
  }
  for (const auto &[a, sec_a] : slugs)
    for (const auto &[b, sec_b] : slugs)
      if (a != b && digits_apart(a, b))
        errs.push_back(std::format(
            "[{}] slug_prefix '{}' and [{}] slug_prefix '{}' differ only by "
            "digits: their generation-numbered files share pki/ca/",
            sec_a, a, sec_b, b));

  // Key backends and the tokens they imply.
  const bool root_p11 = c.root.key_backend == "pkcs11";
  bool any_p11 = root_p11;
  for (const auto &[purpose, ca] : c.cas)
    if (ca.key_backend == "pkcs11") {
      any_p11 = true;
      // The one rejected layout: it would protect the replaceable key
      // better than the anchor. The other mixed layout (pkcs11 root,
      // internal issuing) is the hybrid: offline root token, software key.
      if (!root_p11)
        errs.push_back(std::format(
            "[ca.{}] key_backend = \"pkcs11\" with [root] key_backend = "
            "\"internal\" is rejected (misplaced trust)",
            purpose));
    }

  if (any_p11 && c.pkcs11.module.empty())
    errs.push_back("[pkcs11] module: required when any CA key is on a token");
  if (!any_p11) {
    if (!c.pkcs11.module.empty() || !c.pkcs11.token_label.empty())
      errs.push_back("[pkcs11] is set but no CA key uses the pkcs11 backend");
  }

  // Each token-held CA gets its own label, or the shared default. Equal
  // labels mean one token holding several keys, which is the ordinary
  // single-token layout.
  bool default_used = false;
  auto resolve_label = [&](CaFields &ca, const std::string &sec) {
    if (ca.key_backend != "pkcs11") {
      if (!ca.token_label.empty())
        errs.push_back(std::format(
            "[{}] token_label: set but key_backend is not \"pkcs11\"", sec));
      ca.token_label.clear();
      return;
    }
    if (ca.token_label.empty()) {
      ca.token_label = c.pkcs11.token_label;
      default_used = true;
    }
    if (ca.token_label.empty())
      errs.push_back(std::format(
          "[{}] token_label: required, and [pkcs11] token_label sets no "
          "default to fall back on",
          sec));
    else if (ca.token_label.size() > app::p11_token_label_max)
      errs.push_back(std::format("[{}] token_label: at most {} bytes (PKCS#11 "
                                 "limit)",
                                 sec, app::p11_token_label_max));
  };
  resolve_label(c.root, "root");
  for (auto &[purpose, ca] : c.cas)
    resolve_label(ca, "ca." + purpose);
  // A default nobody falls back to is dead config, the same mistake as a
  // pkcs11 section with no token-held CA at all.
  if (!c.pkcs11.token_label.empty() && !default_used)
    errs.push_back("[pkcs11] token_label: set but every token-held CA "
                   "declares a label of its own");

  if (!errs.empty())
    return std::unexpected(std::move(errs));
  return c;
}

} // namespace cfg

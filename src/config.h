#pragma once

#include <expected>
#include <filesystem>
#include <map>
#include <string>
#include <vector>

namespace cfg {

// The structs mirror the sections of yca.toml one for one, which is also
// the granularity at which the config is locked into the store: a section
// is frozen when it is materialized.
//
// `cn` fields are DN Common Names (UTF8String), where any script is legal.
// `slug` fields are the declared ASCII identifiers used for file names,
// AIA/CDP URLs and key labels on the token.

// [pki]
struct Pki {
  // The organization name.
  std::string org_name;
  // The two letter country code.
  std::string country_code;
  // The host serving the published artifacts.
  std::string repository_host;
  // Optional dotted OID arc (an org PEN), parent of the policy OIDs.
  // Empty means certificates carry no CertificatePolicies extension.
  std::string arc_oid;
};

// [pkcs11]. Absent unless some CA holds its key on a token.
struct Pkcs11 {
  // Path to the PKCS#11 provider, e.g. /usr/lib/opensc-pkcs11.so.
  std::string module;
  // Default token label for CAs that declare no label of their own.
  std::string token_label;
};

// The fields every CA section carries, [root] as well as [ca.*].
struct CaFields {
  // The CA certificate's Common Name.
  std::string cn;
  // The CA key's ECDSA curve.
  std::string curve;
  // The CA certificate's signature digest (also signs this CA's CRL).
  std::string digest;
  // The CA certificate's validity in days.
  int valid_days = 0;
  // Configured stable part of the slug; the full slug is
  // <prefix><generation> - generation 1 at init, incremented by rotation.
  std::string slug_prefix;
  // Derived (prefix + generation), never parsed; an initialized store
  // reads it back from the locked snapshot.
  std::string slug;
  // "internal" (key encrypted in the store) or "pkcs11" (key on a token).
  std::string key_backend = "internal";
  // The token holding this CA's key; empty unless key_backend is
  // "pkcs11". Defaults to Pkcs11::token_label.
  std::string token_label;
};

// [ca.<purpose>]: one issuing CA, dedicated to the profiles it lists.
struct SigningCa : CaFields {
  // The section key: a stable identifier for this CA's lineage.
  std::string purpose;
  // The EE profiles this CA issues. Each profile belongs to exactly one
  // CA, which is how issuance routes to an issuer.
  std::vector<std::string> profiles;
  // The end-entity key's ECDSA curve.
  std::string ee_curve;
  // The end-entity certificate's signature digest.
  std::string ee_digest;
  // Default and ceiling for EE validity under this CA. Per CA because the
  // ceiling is profile policy, not a property of the deployment.
  int ee_valid_days = 0;
  // Optional nameConstraints permitted subtrees. Absent means the CA is
  // bounded only by its EKU, which bounds what a certificate may be used
  // for but not who it may be issued to.
  std::vector<std::string> permitted_dns;
  std::vector<std::string> permitted_email;
};

struct Config {
  Pki pki;
  Pkcs11 pkcs11;
  CaFields root;
  // Keyed by purpose; TOML enforces the uniqueness of the keys itself.
  std::map<std::string, SigningCa> cas;

  // The CA named by `purpose`, or nullptr if none carries that name.
  const SigningCa *ca(const std::string &purpose) const;

  // The CA that issues `profile`, or nullptr if no configured CA claims
  // it. This is how issuance picks an issuer without naming one: load
  // guarantees at most one CA claims any given profile.
  const SigningCa *ca_for_profile(const std::string &profile) const;
};

std::expected<Config, std::vector<std::string>>
load(const std::filesystem::path &path);

} // namespace cfg

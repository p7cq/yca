#pragma once

#include <expected>
#include <filesystem>
#include <string>
#include <vector>

namespace cfg {

// *_cn fields are the DN Common Names (UTF8String). *_slug fields
// are the declared ASCII identifiers used for file names, AIA/CDP URLs and
// key labels on the token.
struct Config {
  // The organization name.
  std::string org_name;
  // The two letter country code.
  std::string country_code;
  // The host serving the published artifacts.
  std::string repository_host;
  // The Root certificate Common Name.
  std::string root_ca_cn;
  // The root CA key's ECDSA curve.
  std::string root_ca_curve;
  // The root certificate signature digest.
  std::string root_ca_digest;
  // The root certificate validity in days.
  int root_ca_valid_days;
  // The root certificate ASCII identifier used for file names, AIA/CDP URLs,
  // key labels on the token.
  std::string root_ca_slug;
  // The signing CA certificate Common Name.
  std::string signing_ca_cn;
  // The signing CA key's ECDSA curve.
  std::string signing_ca_curve;
  // The signing certificate signature digest.
  std::string signing_ca_digest;
  // The signing certificate's validity in days.
  int signing_ca_valid_days;
  // The signing certificate ASCII identifier used for file names, AIA/CDP URLs,
  // key labels on the token.
  std::string signing_ca_slug;
  // The end-entity keys' ECDSA curve.
  std::string ee_curve;
  // The end-entity certificate signature digest.
  std::string ee_digest;
  // The end entity certificate's validity in days.
  int ee_valid_days;
  // The OCSP certificate Common Name.
  std::string ocsp_cn;
  // The OCSP certificate's validity in days.
  int ocsp_valid_days;
  // The OCSP certificate ASCII identifier used for file names, AIA/CDP URLs,
  // key labels on the token.
  std::string ocsp_slug;
  // The OID of the root arc, e.g. 1.3.6.1.4.1.00000
  std::string root_arc_oid;
  // Key backend: "internal" for CA keys encrypted in the SQLite store
  // or "pkcs11" for CA keys encrypted on an HSM token (module path + token
  // label below, PIN from the environment).
  std::string key_backend = "internal";
  // The PKCS11 module path, e.g. /usr/lib/opensc-pkcs11.so.
  std::string pkcs11_module;
  // The token label.
  std::string pkcs11_token_label;
};

std::expected<Config, std::vector<std::string>>
load(const std::filesystem::path &path);

} // namespace cfg

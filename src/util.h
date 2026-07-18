#pragma once

#include <chrono>
#include <cstdint>
#include <optional>
#include <string>

namespace util {

// "ETS Root E1" -> "ets-root-e1": lowercase, spaces turned into hyphens.
std::string slug(const std::string &name);

// Parses a dotted IPv4 string into a host-order uint32; nullopt if malformed.
std::optional<uint32_t> parse_ipv4(const std::string &s);

// Parses "<N><s|m|h|d>" (positive integer, single unit, e.g. "30m", "1h") into
// seconds; nullopt if malformed.
std::optional<std::chrono::seconds> parse_duration(const std::string &s);

// True if `s` is non-empty and contains only ASCII hostname characters
// [A-Za-z0-9.*-] ('*' for wildcards). dNSName SANs are IA5String (ASCII);
// IDN hosts must be given in punycode.
bool dns_safe(const std::string &s);

// True if `s` is non-empty, all printable ASCII (0x21..0x7E) - IA5-safe.
bool ascii_graphic(const std::string &s);

// Whole days in seconds, for the seconds-based X.509 validity APIs.
constexpr uint32_t days_to_seconds(int days) {
  return static_cast<uint32_t>(days) * 24u * 60u * 60u;
}

} // namespace util

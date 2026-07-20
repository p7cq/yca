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

// True if `s` is a printable-ASCII absolute URI: an RFC 3986 scheme
// (ALPHA *( ALPHA / DIGIT / "+" / "-" / "." )) followed by ':' and a
// non-empty remainder. uniformResourceIdentifier SANs are IA5String and
// RFC 5280 requires them absolute; reserved characters and non-ASCII
// must arrive percent-encoded.
bool uri_safe(const std::string &s);

// True if `s` is a valid SPIFFE ID (spiffe://<trust-domain>[/<path>]),
// per the SPIFFE-ID standard: lowercase trust domain of [a-z0-9.-_] with
// no userinfo or port, path segments of [a-zA-Z0-9.-_] that are neither
// empty nor relative modifiers ('.', '..'), no trailing '/', no query or
// fragment, and the length caps in app.h.
bool spiffe_id_safe(const std::string &s);

// Whole days in seconds, for the seconds-based X.509 validity APIs.
constexpr uint32_t days_to_seconds(int days) {
  return static_cast<uint32_t>(days) * 24u * 60u * 60u;
}

} // namespace util

#include "util.h"

#include <cctype>
#include <string_view>

#include "app.h"

namespace util {

std::string slug(const std::string &name) {
  std::string s;
  s.reserve(name.size());
  for (char c : name)
    s.push_back(c == ' ' ? '-'
                         : static_cast<char>(
                               std::tolower(static_cast<unsigned char>(c))));
  return s;
}

std::optional<uint32_t> parse_ipv4(const std::string &s) {
  uint32_t result = 0;
  size_t pos = 0;
  for (int i = 0; i < 4; ++i) {
    const bool last = (i == 3);
    const size_t dot = s.find('.', pos);
    if (last != (dot == std::string::npos))
      return std::nullopt;
    const std::string part = last ? s.substr(pos) : s.substr(pos, dot - pos);
    if (part.empty() || part.size() > 3)
      return std::nullopt;
    if (part.size() > 1 && part[0] == '0') // no leading zeros
      return std::nullopt;
    uint32_t v = 0;
    for (char c : part) {
      if (c < '0' || c > '9')
        return std::nullopt;
      v = v * 10 + static_cast<uint32_t>(c - '0');
    }
    if (v > 255)
      return std::nullopt;
    result = (result << 8) | v;
    pos = last ? pos : dot + 1;
  }
  return result;
}

std::optional<std::chrono::seconds> parse_duration(const std::string &s) {
  if (s.size() < 2)
    return std::nullopt;
  long long mult = 0;
  switch (s.back()) {
  case 's':
    mult = 1;
    break;
  case 'm':
    mult = 60;
    break;
  case 'h':
    mult = 60 * 60;
    break;
  case 'd':
    mult = 24 * 60 * 60;
    break;
  default:
    return std::nullopt;
  }
  long long v = 0;
  for (std::size_t i = 0; i + 1 < s.size(); ++i) {
    const char c = s[i];
    if (c < '0' || c > '9')
      return std::nullopt;
    v = v * 10 + (c - '0');
    if (v > 1'000'000'000) // keep v*mult far from overflow
      return std::nullopt;
  }
  if (v == 0)
    return std::nullopt;
  return std::chrono::seconds(v * mult);
}

bool dns_safe(const std::string &s) {
  if (s.empty())
    return false;
  for (char c : s) {
    const bool ok = (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') ||
                    (c >= '0' && c <= '9') || c == '.' || c == '-' || c == '*';
    if (!ok)
      return false;
  }
  return true;
}

bool ascii_graphic(const std::string &s) {
  if (s.empty())
    return false;
  for (char c : s) {
    const auto u = static_cast<unsigned char>(c);
    if (u < 0x21 || u > 0x7E)
      return false;
  }
  return true;
}

bool uri_safe(const std::string &s) {
  if (!ascii_graphic(s))
    return false;
  const auto colon = s.find(':');
  if (colon == std::string::npos || colon == 0 || colon + 1 == s.size())
    return false;
  const char first = s[0];
  if (!((first >= 'A' && first <= 'Z') || (first >= 'a' && first <= 'z')))
    return false;
  for (std::size_t i = 1; i < colon; ++i) {
    const char c = s[i];
    const bool ok = (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') ||
                    (c >= '0' && c <= '9') || c == '+' || c == '-' || c == '.';
    if (!ok)
      return false;
  }
  return true;
}

bool spiffe_id_safe(const std::string &s) {
  static constexpr std::string_view kPrefix = "spiffe://";
  // The scheme must be exactly "spiffe" (lowercase), so a plain prefix
  // match doubles as the case check.
  if (s.size() > app::spiffe_id_max || !s.starts_with(kPrefix))
    return false;
  const std::string rest = s.substr(kPrefix.size());

  // Trust domain: everything up to the first '/'. The charset excludes
  // '@' and ':', so userinfo and port cannot appear, and '?'/'#' are
  // rejected too - a query or fragment can never parse as one.
  const auto slash = rest.find('/');
  const std::string td = rest.substr(0, slash);
  if (td.empty() || td.size() > app::spiffe_trust_domain_max)
    return false;
  for (char c : td) {
    const bool ok = (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') ||
                    c == '.' || c == '-' || c == '_';
    if (!ok)
      return false;
  }
  if (slash == std::string::npos)
    return true; // trust domain ID, no path component

  // Path: segments are non-empty, never relative modifiers, and their
  // charset again excludes '?' and '#'. A trailing '/' leaves an empty
  // last segment, which the same rule rejects.
  std::size_t pos = slash;
  while (pos < rest.size()) {
    const auto next = rest.find('/', pos + 1);
    const std::string seg = rest.substr(
        pos + 1, next == std::string::npos ? std::string::npos : next - pos - 1);
    if (seg.empty() || seg == "." || seg == "..")
      return false;
    for (char c : seg) {
      const bool ok = (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') ||
                      (c >= '0' && c <= '9') || c == '.' || c == '-' ||
                      c == '_';
      if (!ok)
        return false;
    }
    if (next == std::string::npos)
      break;
    pos = next;
  }
  return true;
}

} // namespace util

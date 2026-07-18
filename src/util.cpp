#include "util.h"

#include <cctype>

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

} // namespace util

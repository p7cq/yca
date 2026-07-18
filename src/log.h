#pragma once

#include <array>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <format>
#include <print>
#include <string>
#include <string_view>
#include <utility>

#include "app.h"

namespace log {
enum class Level : int { OFF, FATAL, ERROR, WARN, INFO, DEBUG, Count };

struct Cfg {
  Level default_level = Level::INFO;
  std::filesystem::path file_path; // empty: no file log (unit tests)
};

inline Cfg &config() {
  static Cfg cfg{};
  return cfg;
}

static inline constexpr std::string as_string(const Level &level) {
  static constexpr std::array<std::string,
                              static_cast<std::size_t>(Level::Count)>
      name = {"OFF", "FATAL", "ERROR", "WARN", "INFO", "DEBUG"};
  return name[static_cast<std::size_t>(level)];
}

// Default log path for a store: yca.log next to the store dir.
inline std::filesystem::path default_path(std::filesystem::path store_dir) {
  if (!store_dir.has_filename()) { // tolerate a trailing slash
    store_dir = store_dir.parent_path();
  }
  return store_dir.parent_path() / (std::string(app::name) + ".log");
}

// The log file is the single diagnostics destination - the audit trail:
// failing to open or write it is fatal (bare message on stderr, exit 1).
inline std::FILE *stream() {
  auto &cfg = config();
  if (cfg.file_path.empty())
    return nullptr; // not configured (unit tests): console mirroring only

  static std::FILE *file = std::fopen(cfg.file_path.string().c_str(), "ab");
  if (!file) {
    std::println(stderr, "log: cannot open {} for writing",
                 cfg.file_path.string());
    std::exit(EXIT_FAILURE);
  }
  return file;
}

// Sets the log file and opens it eagerly, so an unwritable path stops the
// program before it does any work.
inline void set_file(std::filesystem::path path) {
  config().file_path = std::move(path);
  stream();
}

inline void write_line(std::string_view line) {
  std::FILE *file = stream();
  if (!file)
    return;

  std::string out;
  out.reserve(line.size() + 1);
  out.append(line);
  out.push_back('\n');

  if (std::fwrite(out.data(), 1, out.size(), file) < out.size()) {
    std::println(stderr, "log: write to {} failed",
                 config().file_path.string());
    std::exit(EXIT_FAILURE);
  }
  std::fflush(file);
}

inline std::string timestamp() {
  using namespace std::chrono;
  auto now = system_clock::now();
  std::time_t tt = system_clock::to_time_t(now);
  std::tm tm{};
  localtime_r(&tt, &tm);
  char buf[32];
  std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tm);
  auto ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;
  return std::format("{}.{:03}", buf, ms.count());
}

inline bool should_log(Level L) {
  return static_cast<int>(L) <= static_cast<int>(config().default_level) &&
         L != Level::OFF;
}

// Bare-message writers. Artifacts and the one-time passphrase banner
// to stdout, user-facing errors to stderr.
template <class... Args>
inline void to_stdout(std::string_view fmt, Args &&...args) {
  std::println(stdout, "{}", std::vformat(fmt, std::make_format_args(args...)));
}
template <class... Args>
inline void to_stderr(std::string_view fmt, Args &&...args) {
  std::println(stderr, "{}", std::vformat(fmt, std::make_format_args(args...)));
}

template <class... Args>
inline void log(Level L, std::string_view fmt, Args &&...args) {
  if (!should_log(L))
    return;

  auto msg = std::vformat(fmt, std::make_format_args(args...));
  // Errors are also printed to stderr.
  if (L <= Level::ERROR)
    to_stderr("{}", msg);
  write_line(std::format("{} {:5} {}", timestamp(), as_string(L), msg));
}

// Fatal terminates the process.
template <class... Args>
[[noreturn]] inline void fatal(std::string_view f, Args &&...a) {
  log(Level::FATAL, f, std::forward<Args>(a)...);
  std::exit(EXIT_FAILURE);
}
template <class... Args> inline void error(std::string_view f, Args &&...a) {
  log(Level::ERROR, f, std::forward<Args>(a)...);
}
template <class... Args> inline void warn(std::string_view f, Args &&...a) {
  log(Level::WARN, f, std::forward<Args>(a)...);
}
template <class... Args> inline void info(std::string_view f, Args &&...a) {
  log(Level::INFO, f, std::forward<Args>(a)...);
}
template <class... Args> inline void debug(std::string_view f, Args &&...a) {
  log(Level::DEBUG, f, std::forward<Args>(a)...);
}

class timer {
  std::string name;
  std::chrono::steady_clock::time_point start;

public:
  explicit timer(std::string name)
      : name(std::move(name)), start(std::chrono::steady_clock::now()) {}
  ~timer() {
    using namespace std::chrono;
    auto end = steady_clock::now();
    auto ms = duration_cast<milliseconds>(end - start).count();
    log::info("{} took: {}", name, ms);
  }
};
} // namespace log

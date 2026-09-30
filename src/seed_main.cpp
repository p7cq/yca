// Copyright 2026 p7cq <707c71@gmail.com>
// SPDX-License-Identifier: Apache-2.0

#include <cstdlib>
#include <exception>
#include <string>
#include <string_view>

#include <CLI/CLI11.hpp>

#include "app.h"
#include "ca.h"
#include "log.h"
#include "seed.h"

int main(int argc, char **argv) {
  CLI::App app{std::string(app::name) +
               "-seed - load-test store seeder (not for production use)"};

  std::string store_dir{app::store_dir};
  int count = 0, same_cn = 0;
  app.add_option("--store", store_dir,
                 std::string("store/artifacts directory (default ") +
                     std::string(app::store_dir) + ")");
  app.add_option("--count", count, "number of EE certs to seed")->required();
  app.add_option("--same-cn", same_cn,
                 "extra certs piled on one CN (adversarial)");

  CLI11_PARSE(app, argc, argv);

  // Same audit log as the CLI: seeding pollutes the store, it deserves a
  // trace.
  log::set_file(log::default_path(store_dir));

  try {
    if (!ca::is_initialized(store_dir)) {
      log::error("not initialized; run '{} init'", app::name);
      return 1;
    }
    auto eff = ca::load_config(store_dir);
    if (!eff) { // safety net; is_initialized implies ca_config exists
      log::error("not initialized; run '{} init'", app::name);
      return 1;
    }

    const char *pass_env = std::getenv(app::passphrase_env);
    std::string_view passphrase = pass_env ? pass_env : "";
    return ca::seed(*eff, store_dir, passphrase, count, same_cn) ? 0 : 1;
  } catch (const std::exception &e) {
    log::error("unexpected error: {}", e.what());
    return 1;
  }
}

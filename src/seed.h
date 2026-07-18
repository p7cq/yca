#pragma once

#include <filesystem>
#include <string_view>

#include "config.h"

namespace ca {

// Load-test seeder: inserts `count` EE certs (server ~60% / client ~40%, distinct
// CNs, mixed active/expired/revoked) plus `same_cn` certs piled on one CN. Uses a
// reused EE keypair and a batched transaction; revoked certs go to the store's
// revoked table and the signing CA's file CRL. Not crypto-realistic - for
// store/query load testing. Built only into the `yca-seed` binary, not the
// production CLI.
bool seed(const cfg::Config &config, const std::filesystem::path &store_dir,
          std::string_view passphrase, int count, int same_cn);

} // namespace ca

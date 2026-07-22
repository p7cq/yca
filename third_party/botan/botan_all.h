#pragma once

// Botan amalgamations are generated per (os, arch) by tools/update-botan.sh;
// dispatch to the vendored one matching this platform. The CMakeLists here
// picks the matching botan_all.cpp the same way.
#if defined(__APPLE__) && defined(__aarch64__)
#include "macos-arm64/botan_all.h"
#elif defined(__linux__) && defined(__x86_64__)
#include "linux-x86_64/botan_all.h"
#elif defined(__linux__) && defined(__aarch64__)
#include "linux-aarch64/botan_all.h"
#elif defined(__FreeBSD__) && defined(__x86_64__)
#include "freebsd-x86_64/botan_all.h"
#else
#error                                                                         \
    "No vendored Botan amalgamation for this platform; add a target to tools/update-botan.sh"
#endif

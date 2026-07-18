#pragma once

#include <memory>
#include <optional>
#include <string>
#include <string_view>

#include <botan/botan_all.h>

#include "config.h"

namespace p11 {

// A logged-in session on the configured PKCS#11 token. Owns the module, slot
// and session; keys returned by find/generate reference the session, so the
// Token must outlive them.
class Token {
public:
  // Loads config.pkcs11_module, locates the slot whose token label equals
  // config.pkcs11_token_label, opens a session (read-write only for key
  // generation) and logs in as the user. A single login attempt, no retry:
  // NK HSM PIN has a hardware retry counter. Throws std::runtime_error or a
  // Botan exception on any failure.
  Token(const cfg::Config &config, std::string_view pin, bool read_write);

  // Finds the ECDSA keypair labeled `label` and returns the private key with
  // its public point filled in from the paired public object (a PKCS#11 EC
  // private object carries no public point). Returns nullptr if neither
  // object exists; throws if only half the pair is present or the key is not
  // on `curve` (the curve declared in config).
  std::shared_ptr<Botan::PKCS11::PKCS11_ECDSA_PrivateKey>
  find_keypair(const std::string &label, const std::string &curve);

  // Generates a token-resident ECDSA keypair on `curve`, labeled `label`;
  // the private half is sensitive and non-extractable. Needs a read-write
  // session.
  std::shared_ptr<Botan::PKCS11::PKCS11_ECDSA_PrivateKey>
  generate_keypair(const std::string &label, const std::string &curve);

private:
  Botan::PKCS11::Module m_module;
  std::optional<Botan::PKCS11::Slot> m_slot;
  std::optional<Botan::PKCS11::Session> m_session;
};

} // namespace p11

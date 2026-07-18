#include "p11.h"

#include <format>
#include <stdexcept>
#include <utility>
#include <vector>

namespace p11 {
namespace {

namespace P = Botan::PKCS11;

// CK_TOKEN_INFO.label is a fixed 32-byte field, space-padded, not
// NULL-terminated.
std::string token_label(const P::TokenInfo &info) {
  std::string s(reinterpret_cast<const char *>(info.label), sizeof(info.label));
  while (!s.empty() && s.back() == ' ')
    s.pop_back();
  return s;
}

// The public point comes from the paired public object; go through the SEC1
// point encoding (public_key_bits, the SubjectPublicKey contents for EC keys)
// to stay on stable Botan API.
Botan::EC_AffinePoint public_point_of(const P::PKCS11_ECDSA_PublicKey &pub,
                                      const Botan::EC_Group &group,
                                      const std::string &label) {
  auto pt = Botan::EC_AffinePoint::deserialize(group, pub.public_key_bits());
  if (!pt)
    throw std::runtime_error(std::format(
        "public key object '{}' does not decode on the expected curve", label));
  return *pt;
}

} // namespace

Token::Token(const cfg::Config &config, std::string_view pin, bool read_write)
    : m_module(config.pkcs11_module) {
  const auto slots = P::Slot::get_available_slots(m_module, true);
  for (const auto id : slots) {
    P::Slot candidate(m_module, id);
    if (token_label(candidate.get_token_info()) == config.pkcs11_token_label) {
      m_slot.emplace(m_module, id);
      break;
    }
  }
  if (!m_slot)
    throw std::runtime_error(std::format(
        "no token labeled '{}' found via {} ({} token(s) present)",
        config.pkcs11_token_label, config.pkcs11_module, slots.size()));
  m_session.emplace(*m_slot, /*read_only=*/!read_write);
  try {
    m_session->login(P::UserType::User,
                     P::secure_string(pin.begin(), pin.end()));
  } catch (const std::exception &e) {
    throw std::runtime_error(std::format("PKCS#11 login failed: {}", e.what()));
  }
}

std::shared_ptr<Botan::PKCS11::PKCS11_ECDSA_PrivateKey>
Token::find_keypair(const std::string &label, const std::string &curve) {
  auto privs = P::Object::search<P::PKCS11_ECDSA_PrivateKey>(*m_session, label);
  auto pubs = P::Object::search<P::PKCS11_ECDSA_PublicKey>(*m_session, label);
  if (privs.empty() && pubs.empty())
    return nullptr;
  if (privs.size() != 1 || pubs.size() != 1)
    throw std::runtime_error(std::format(
        "token objects for label '{}': {} private / {} public (expected "
        "exactly one of each)",
        label, privs.size(), pubs.size()));

  const Botan::EC_Group expected = Botan::EC_Group::from_name(curve);
  if (privs[0].domain() != expected)
    throw std::runtime_error(
        std::format("key '{}' on the token is not on the configured curve {}",
                    label, curve));

  auto key = std::make_shared<P::PKCS11_ECDSA_PrivateKey>(std::move(privs[0]));
  key->set_public_point(public_point_of(pubs[0], expected, label));
  return key;
}

std::shared_ptr<Botan::PKCS11::PKCS11_ECDSA_PrivateKey>
Token::generate_keypair(const std::string &label, const std::string &curve) {
  const Botan::EC_Group group = Botan::EC_Group::from_name(curve);
  const auto ec_params = group.DER_encode(Botan::EC_Group_Encoding::NamedCurve);

  P::EC_PublicKeyGenerationProperties pub_props(ec_params);
  pub_props.set_token(true);
  pub_props.set_verify(true);
  pub_props.set_label(label);

  P::EC_PrivateKeyGenerationProperties priv_props;
  priv_props.set_token(true);
  priv_props.set_private(true);
  priv_props.set_sensitive(true);
  priv_props.set_extractable(false);
  priv_props.set_sign(true);
  priv_props.set_label(label);

  auto [pub, priv] =
      P::generate_ecdsa_keypair(*m_session, pub_props, priv_props);
  auto key = std::make_shared<P::PKCS11_ECDSA_PrivateKey>(std::move(priv));
  key->set_public_point(public_point_of(pub, group, label));
  return key;
}

} // namespace p11

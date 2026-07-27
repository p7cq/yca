#include "x509ext.h"

namespace x509ext {
namespace {

// GeneralName CHOICE tags used here (RFC 5280 4.2.1.6).
constexpr uint32_t kRfc822Name = 1;
constexpr uint32_t kDnsName = 2;

// One GeneralSubtree holding an IA5String GeneralName under `tag`. The
// name is implicitly tagged, so it is written with the context tag in
// place of the universal IA5String tag rather than wrapped in it.
void encode_subtree(Botan::DER_Encoder &der, uint32_t tag,
                    const std::string &name) {
  der.start_sequence()
      .add_object(static_cast<Botan::ASN1_Type>(tag),
                  Botan::ASN1_Class::ContextSpecific, name)
      .end_cons();
}

} // namespace

Botan::OID Name_Constraints::oid_of() const {
  return Botan::OID::from_string("X509v3.NameConstraints");
}

std::vector<uint8_t> Name_Constraints::encode_inner() const {
  std::vector<uint8_t> out;
  Botan::DER_Encoder der(out);
  der.start_sequence();
  // permittedSubtrees [0] IMPLICIT: a constructed context tag replacing
  // the SEQUENCE OF tag.
  der.start_cons(static_cast<Botan::ASN1_Type>(0),
                 Botan::ASN1_Class::ContextSpecific |
                     Botan::ASN1_Class::Constructed);
  // dNSName before rfc822Name, so the encoding of a given set of
  // constraints is deterministic and comparable byte for byte.
  for (const auto &d : m_nc.permitted_dns)
    encode_subtree(der, kDnsName, d);
  for (const auto &e : m_nc.permitted_email)
    encode_subtree(der, kRfc822Name, e);
  der.end_cons();
  der.end_cons();
  return out;
}

void Name_Constraints::decode_inner(const std::vector<uint8_t> &) {
  // Nothing decodes through this type: Botan's own Name_Constraints is
  // registered for this OID and its decoder works. Reaching here means the
  // extension was registered for parsing by mistake.
  throw Botan::Not_Implemented("x509ext::Name_Constraints is write-only");
}

} // namespace x509ext

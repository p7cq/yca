#pragma once

#include <string>
#include <vector>

#include <botan/botan_all.h>

// X.509 extensions Botan can parse and enforce but cannot emit, so their
// DER is written here by hand.

namespace x509ext {

// The permitted subtrees of a nameConstraints extension. Only the two
// GeneralName forms this PKI constrains are supported: a CA issuing TLS
// certificates is bounded by dNSName, one issuing S/MIME by rfc822Name.
//
// There are deliberately no excluded subtrees and no directoryName. A
// directoryName constraint would reject every leaf this PKI issues, whose
// DN carries a CN and nothing else, so no non-empty subtree can contain
// it; the S/MIME BR ask for one, and meeting that would mean putting O and
// C into leaf DNs first.
struct NameConstraints {
  // Bare FQDNs. RFC 5280 dNSName matching is by label suffix, so
  // "example.ca" also covers "www.example.ca".
  std::vector<std::string> permitted_dns;
  // Bare FQDNs, or a FQDN preceded by a full stop. RFC 5280 rfc822Name
  // matching treats "example.ca" as every mailbox at that host and
  // ".example.ca" as every mailbox in a subdomain of it - the two do not
  // overlap, so a constraint for mailboxes at the domain itself must be
  // written without the leading stop.
  std::vector<std::string> permitted_email;

  bool empty() const {
    return permitted_dns.empty() && permitted_email.empty();
  }
};

// A nameConstraints extension (RFC 5280 4.2.1.10) that can be attached to
// a certificate being issued. Botan's own GeneralName, GeneralSubtree and
// Name_Constraints encoders all throw Not_Implemented, so the structure is
// encoded directly:
//
//   NameConstraints ::= SEQUENCE {
//        permittedSubtrees  [0] GeneralSubtrees OPTIONAL,
//        excludedSubtrees   [1] GeneralSubtrees OPTIONAL }
//   GeneralSubtrees ::= SEQUENCE SIZE (1..MAX) OF GeneralSubtree
//   GeneralSubtree  ::= SEQUENCE { base GeneralName, ... }
//   GeneralName     ::= CHOICE { rfc822Name [1] IA5String,
//                                dNSName    [2] IA5String, ... }
//
// The context tags are implicit, and `minimum`/`maximum` are omitted: PKIX
// requires minimum to be 0 and maximum to be absent.
//
// This type is write-only. Reading a certificate's constraints goes
// through Botan, whose decoder works; decode_inner throws.
class Name_Constraints final : public Botan::Certificate_Extension {
public:
  Name_Constraints() = default;
  explicit Name_Constraints(NameConstraints nc) : m_nc(std::move(nc)) {}

  Botan::OID oid_of() const override;
  std::string oid_name() const override { return "X509v3.NameConstraints"; }
  std::unique_ptr<Botan::Certificate_Extension> copy() const override {
    return std::make_unique<Name_Constraints>(m_nc);
  }

private:
  // An extension with no subtrees at all would be a SEQUENCE {} that
  // permits everything, which is worse than saying nothing.
  bool should_encode() const override { return !m_nc.empty(); }
  std::vector<uint8_t> encode_inner() const override;
  void decode_inner(const std::vector<uint8_t> &) override;

  NameConstraints m_nc;
};

} // namespace x509ext

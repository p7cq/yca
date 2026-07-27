#pragma once

#include <algorithm>
#include <span>
#include <string_view>

namespace profile {

// The subject rules a profile imposes on the certificate it shapes.
enum class Subject {
  // The CN is a hostname and is always added as DNS:CN.
  DnsCn,
  // At least one SAN of any supported type; the CN stays free-form.
  RequireSan,
  // At least one rfc822Name, one of which must equal the CN.
  EmailMatchingCn,
};

// One end-entity certificate shape. This table is the only place that says
// what a profile means: which EKU it carries, which key usage bits, what it
// demands of the subject, its policy OID under the org arc, and how long it
// may live. Adding a profile is an entry here plus tests; nothing about a
// profile is a per-deployment decision, so none of it is configurable.
//
// Before adding one, check that a CA carrying it can still be a CA to
// Botan, which treats a certificate whose EKU names none of serverAuth,
// clientAuth, OCSPSigning or anyExtendedKeyUsage as not being one at all
// (see ca_companion_eku below).
struct Def {
  std::string_view name;
  // extendedKeyUsage. An issuing CA carries the union of the EKUs of the
  // profiles it lists, so it can never be broader than what it may sign.
  std::string_view eku;
  // keyUsage is digitalSignature, plus keyAgreement when this is set: an
  // ECDH recipient key needs it, an ECDSA signing key must not have it.
  bool key_agreement;
  Subject subject;
  // Whether the subject must carry the organizational attributes (C and O)
  // beside the CN. Set for any profile whose standards put organizational
  // identity in the subject, and for any profile that has to fit inside a
  // directoryName name constraint, which is compared position by position
  // against a C, O, CN encoding.
  bool full_dn;
  // Appended to <arc_oid>; the registry of assignments is in cps-tpl.md.
  std::string_view policy_suffix;
  // Ceiling on this profile's certificates. 398 days is the TLS Baseline
  // Requirements number; the S/MIME BR cap the Strict and Multipurpose
  // profiles at 825.
  int max_valid_days;
  // Not available through `create`: the CA must never hold the private key.
  bool csr_only;
  // An EKU the issuing CA carries in addition to this profile's own.
  //
  // Botan treats a certificate whose EKU extension is present but names
  // none of serverAuth, clientAuth, OCSPSigning or anyExtendedKeyUsage as
  // not being a CA at all, so an emailProtection-only CA cannot sign, or
  // even put out its own CRL. Adding clientAuth is not a workaround for
  // that: S/MIME BR 7.1.2.2 requires id-kp-emailProtection on a subordinate
  // CA, forbids serverAuth, codeSigning, timeStamping and anyEKU, and
  // permits any other value, and the publicly trusted S/MIME intermediates
  // carry exactly this pair - Sectigo's are even named "Client
  // Authentication and Secure Email CA". The subscriber certificate keeps
  // emailProtection alone, which is what the Strict generation requires of
  // a leaf (7.1.2.3); the generation split does not reach the CA.
  //
  // The cost is that this CA could also mint client-auth leaves. What
  // bounds that is nameConstraints, which 7.1.5 makes the definition of a
  // technically constrained S/MIME subordinate CA.
  std::string_view ca_companion_eku;
};

inline constexpr Def kDefs[] = {
    // The TLS profiles are the only ones with no use for an organizational
    // subject: a server is named by its dNSName SANs and a client by the
    // SAN its relying party matches, so [ca.*] simple_dn may reduce them.
    {"server", "1.3.6.1.5.5.7.3.1", false, Subject::DnsCn, false, ".1.1", 398,
     false, ""},
    {"client", "1.3.6.1.5.5.7.3.2", false, Subject::RequireSan, false, ".1.2",
     398, false, ""},
    // S/MIME, as one dual-use certificate carrying both signing and ECDH key
    // agreement rather than a separate signing and encryption pair. There is
    // no key escrow and nonRepudiation is deliberately not asserted.
    // full_dn: an S/MIME subordinate CA counts as technically constrained
    // only with a directoryName subtree beside the rfc822Name one (S/MIME BR
    // 7.1.5), and a bare CN cannot sit inside any non-empty such subtree.
    {"email", "1.3.6.1.5.5.7.3.4", true, Subject::EmailMatchingCn, true, ".1.3",
     825, true,
     "1.3.6.1.5.5.7.3.2"}, // + clientAuth on the CA, see ca_companion_eku
};

inline std::span<const Def> all() { return kDefs; }

inline const Def *find(std::string_view name) {
  const auto it = std::ranges::find(kDefs, name, &Def::name);
  return it == std::end(kDefs) ? nullptr : it;
}

// The longest life any profile allows, which is what a `list` day window
// must be able to span for an expiring certificate to show up in time.
inline constexpr int max_valid_days() {
  int m = 0;
  for (const Def &d : kDefs)
    m = d.max_valid_days > m ? d.max_valid_days : m;
  return m;
}

} // namespace profile

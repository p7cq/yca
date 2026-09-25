# Day-to-day operation - usage examples

Examples for every issuance and lookup flow the CLI offers. Commands
assume an initialized store and run as the admin through the packaged
`yca` wrapper: it runs the CLI as the `yca` account with
`/etc/yca/yca.toml` and `/var/lib/yca/store`, and the unattended secrets
from `/etc/yca/yca.env`. A secret kept off disk is exported for the
ceremony only, and the wrapper passes it on:

```bash
read -rs CA_HSM_ROOT_PIN && export CA_HSM_ROOT_PIN  # root token (split/hybrid)
# ... ceremony commands ...
unset CA_HSM_ROOT_PIN
```

Without the wrapper (a development build) the secrets come from the
environment the same way: `CA_STORE_PASSPHRASE` (keys on the internal
backend), `CA_HSM_PIN` (signing token), `CA_HSM_ROOT_PIN` (root token,
falling back to `CA_HSM_PIN`). Through the wrapper the CLI runs in
`/var/lib/yca` as `yca` and cannot open files in your home, which is why
the examples pass CSRs on stdin.

## Issue a certificate (CA-generated key)

`create` generates the key on the CA side and delivers cert + unencrypted
PKCS#8 key under `<store>/ee/`:

```bash
yca create server --cn server.example.ca --san dns:example.ca
sudo ls /var/lib/yca/store/ee/
# server.example.ca.crt  server.example.ca.key

yca create client --cn "Client Name" --san email:client@example.ca

# workload identity (SPIFFE): the uri SAN carries the SPIFFE ID
yca create client --cn web --san uri:spiffe://example.ca/ns/prod/sa/web
```

A `uri` SAN must be an absolute URI; when it uses the `spiffe` scheme it
is validated as a SPIFFE ID (lowercase trust domain, no port, no query or
fragment, no trailing slash). At most one `uri` per certificate, because
an X509-SVID carries exactly one.

Server certs always get `DNS:<CN>` added to the SAN; clients need at least
one `--san` (the CN may be a person name). One active certificate per
(CN, profile) - a second `create server --cn server.example.ca` is rejected
until the first is revoked, expires, or enters the renewal window (less
than 33% of its lifetime left): then an overlapping successor is issued -
what automated rotation needs - and the old cert just expires. With two
active certs, `get`/`revoke` operate on the newest (revoke again to clear
the older).

A certificate may never outlive its issuer: once the requested validity
(`ee_valid_days` or `--valid`) would end after the signing
CA's own `notAfter`, issuance (`create` and `sign` alike) is refused - lower
the configured validity, or renew the signing CA.

## Issue a certificate from a CSR (key stays with the requester)

The CA never sees the private key: the requester generates the keypair and a
PKCS#10 request, an operator-enrolled identity plus a short-lived nonce gate
the signing, and the certificate is delivered via `get`. The policy: only
the public key, the CN and the supported SANs (dns, email, IPv4, URI) are taken
from the CSR - every other requested extension (basicConstraints, keyUsage,
EKU, ...) is ignored - the CA dictates the profile.

**Requester side** - generate the key and CSR with openssl (the key must be
ECDSA on the issuing CA's configured `ee_curve`; `prime256v1` is the OpenSSL name
for `secp256r1`):

```bash
openssl ecparam -genkey -name prime256v1 -noout -out server.key
openssl req -new -key server.key -out server.csr \
  -subj "/CN=server.example.ca" \
  -addext "subjectAltName=DNS:example.ca"
```

Or with a reusable request config instead of the inline flags - see
[`share/csr/server.cnf`](../share/csr/server.cnf),
[`share/csr/client.cnf`](../share/csr/client.cnf) and
[`share/csr/email.cnf`](../share/csr/email.cnf):

```bash
openssl req -new -key server.key -config share/csr/server.cnf -out server.csr
```

```ini
# server.cnf - only the fields yca honors: CN-only subject + supported SANs
[req]
prompt             = no
distinguished_name = dn
req_extensions     = ext

[dn]
CN = server.example.ca

[ext]
subjectAltName = DNS:example.ca
```

Ready-made example CSRs live in [`share/csr/`](../share/csr/) - one per
profile, generated from those configs. Both configs and CSRs deliberately
carry only the fields yca honors: a CN-only subject and one supported SAN.
The CA rebuilds the subject DN from its own configuration, so an
`organizationName` or `countryName` in the CSR is replaced rather than
carried through.

For the `email` profile this is the only issuance path - `yca create email`
is refused - and it has rules of its own; see
[S/MIME certificates](#smime-certificates-the-email-profile) below.

**CA side** - enroll the requester once, then a nonce per issuance:

```bash
yca enroll --id user@example.ca
yca get nonce --id user@example.ca
2ebcb359db892a7ef0c3d7964cca5930857a88f660e2712c1905ccc625113931

yca sign server --id user@example.ca \
    --nonce 2ebcb359db892a7ef0c3d7964cca5930857a88f660e2712c1905ccc625113931 \
    --csr - < server.csr
server.example.ca
```

`sign` prints just the CN on stdout and writes nothing under `ee/`; the
certificate is fetched with `get` (which is how the requester receives it,
too - and can re-fetch it any time without a re-issue):

```bash
yca get server --cn server.example.ca > server.crt
```

Or pipe the two directly - `get --cn -` reads the CN from stdin:

```bash
yca sign server --id user@example.ca --nonce "$(yca get nonce --id user@example.ca)" \
    --csr - < server.csr | yca get server --cn - > server.crt
```

The nonce is single-use (consumed in the same transaction as the insert) and
lives 5 minutes; `get nonce` returns the pending one while it still has at
least max(1 minute, 20% of its validity) left, otherwise it rotates. After
a successful `sign`, the next issuance for the same identity just requests
a fresh nonce.

`--csr` takes three forms:

```bash
yca sign server --id ... --nonce ... --csr server.csr          # file (PEM or DER), readable by the CLI
yca sign server --id ... --nonce ... --csr - < server.csr      # stdin (PEM or DER)
yca sign server --id ... --nonce ... --csr "$(cat server.csr)" # inline PEM
```

The inline form survives a single-line paste (newlines replaced by spaces) -
the PEM decoder does not care about line structure:

```bash
yca sign server --id ... --nonce ... --csr "$(tr '\n' ' ' < server.csr)"
```

Client profile from a CSR - same flow, and the CSR must carry at least one
supported SAN (example: [`share/csr/client.csr`](../share/csr/client.csr),
generated from [`share/csr/client.cnf`](../share/csr/client.cnf)):

```bash
openssl ecparam -genkey -name prime256v1 -noout -out client.key
openssl req -new -key client.key -config share/csr/client.cnf -out client.csr
# equivalent inline: -subj "/CN=Client Name" -addext "subjectAltName=DNS:server.example.ca"
yca sign client --id user@example.ca --nonce "$(yca get nonce --id user@example.ca)" \
    --csr - < client.csr | yca get client --cn -
```

Verify what was issued - the hostile parts of a CSR never survive:

```bash
yca get server --cn server.example.ca | openssl x509 -noout -subject \
    -ext basicConstraints,keyUsage,extendedKeyUsage,subjectAltName
# subject=CN=server.example.ca          <- DN rebuilt by the CA; bare CN
#                                          because [ca.tls] sets simple_dn
# CA:FALSE, Digital Signature, TLS Web Server Authentication
# DNS:example.ca, DNS:server.example.ca <- DNS:CN always added
```

## S/MIME certificates (the `email` profile)

Everything above applies; what follows is what is specific to S/MIME. It
assumes an `[ca.email]` issuing CA exists - see
[Add an issuing CA](#add-an-issuing-ca) if it does not.

**The CA never holds the key.** `yca create email` is refused. For a TLS
server key, `create` writing the key under `<store>/ee/` is a delivery
mechanism; for an S/MIME encryption key it would be an uncontrolled
escrow, holding the long-term confidentiality of a mailbox in plaintext
on the CA host. So the profile supports only the CSR path, where the key
is generated by the subscriber and never leaves them.

**The mailbox is the identity.** The CN is the mailbox address, and one
`rfc822Name` SAN must equal it - a certificate whose CN names a mailbox it
does not carry is refused. The enrollment `--id` is the same address.

```bash
# Subscriber side: key + request. secp256r1 or secp384r1 (Gmail accepts
# both); prime256v1 is the OpenSSL name for secp256r1.
openssl ecparam -genkey -name prime256v1 -noout -out user.key
openssl req -new -key user.key -out user.csr \
  -subj "/CN=user@example.ca" \
  -addext "subjectAltName=email:user@example.ca"
# or with the reusable config: share/csr/email.cnf

# CA side: enroll once, then a nonce per issuance
yca enroll --id user@example.ca
yca sign email --id user@example.ca \
    --nonce "$(yca get nonce --id user@example.ca)" \
    --csr - < user.csr | yca get email --cn - > user.crt
```

**One dual-use certificate**, carrying `digitalSignature` for signing and
`keyAgreement` for ECDH encryption, rather than a separate signing and
encryption pair. The pair exists to serve key escrow - escrow the
encryption key, never the signature key - and without an escrow facility
it buys nothing. `nonRepudiation` is deliberately not asserted: this PKI
makes no such claim.

```bash
openssl x509 -in user.crt -noout -subject -nameopt oneline \
    -ext keyUsage,extendedKeyUsage,subjectAltName,certificatePolicies
# subject=C = CA, O = Example, CN = user@example.ca  <- organizational DN
# Digital Signature, Key Agreement                   <- dual use, no nonRepudiation
# E-mail Protection
# email:user@example.ca
# Policy: 1.3.6.1.4.1.32473.1.3
```

The subject DN is organizational here even when the TLS CA uses
`simple_dn`: an S/MIME subject must carry `C` and `O`, so the `email`
profile refuses the knob (see [Installation](install.md)).

**Delivery to a mail client** is a PKCS#12 bundle, and the subscriber
assembles it: the CA can hand out the certificate and the chain, but never
had the private key that has to go in beside them.

```bash
yca get ca --cn root-ca > root.pem
yca get email --cn user@example.ca --chain > user-chain.pem
openssl pkcs12 -export -inkey user.key -in user-chain.pem -certfile root.pem \
    -name "user@example.ca" -out user.p12
```

Import `user.p12` into Thunderbird, Outlook or Apple Mail, and the root
into the client's trust store. Verify before shipping it:

```bash
openssl verify -purpose smimesign -CAfile root.pem \
    -untrusted user-chain.pem user.crt
# user.crt: OK
```

**No key recovery, ever.** There is no escrow, no archival and no recovery
ceremony, and none is planned. Losing the private key permanently destroys
the ability to read every message encrypted to it; no party, the operator
included, can recover that plaintext. The subscribers MUST back up the `.p12`
and keep superseded key pairs for as long as they need to read archived mail.

Retrieval, listing and revocation use the profile name like any other:

```bash
yca get email --cn user@example.ca
yca list --cn user@example.ca
yca revoke email --cn user@example.ca --reason keyCompromise
yca get ca --cn email-ca            # the issuing CA, by purpose alias
yca get crl --cn email-ca > email.crl
```

Note that revoking an S/MIME certificate stops future signature
verification, but does nothing about messages already encrypted to that
key.

## Retrieve artifacts

```bash
yca get server --cn server.example.ca                 # active cert, PEM
yca get server --cn server.example.ca --chain         # cert + issuers, no root
yca get client --cn "Client Name"
yca get email --cn user@example.ca                 # S/MIME, same shape
yca get ca --cn root-ca --encoding der > root.crt  # CA certs by alias or CN
yca get ca --cn signing-ca
yca get crl --cn signing-ca > signing.crl          # PEM by default
yca get config                                     # effective DB config (TOML)
yca get nonce --id user@example.ca                 # enrollment nonce
```

Verify a delivered chain with openssl:

```bash
yca get ca --cn root-ca > root.pem
yca get ca --cn signing-ca > sub.pem
yca get server --cn server.example.ca | openssl verify -CAfile root.pem -untrusted sub.pem
# stdin: OK
```

Or let the CA assemble the intermediates, which is what `--chain` is for:
how deep the chain goes is a property of the hierarchy; it stops below
the root, the one certificate the relying party is expected to hold already,
and works only with PEM.

```bash
yca get ca --cn root-ca > root.pem
yca get server --cn server.example.ca --chain > chain.pem
openssl verify -CAfile root.pem -untrusted chain.pem chain.pem
# chain.pem: OK
```

## List and inspect

Exactly one filter per call; time filters take an optional window in days
(default 30):

```bash
yca list --expiring 7        # active, expiring within a week
yca list --expired           # recently expired
yca list --revoked           # recently revoked
yca list --last 1            # issued in the last day
yca list --cn server.example.ca # everything ever issued for a CN
yca list --last --tsv        # tab-separated
```

The CA certificates sit in the same index, so `list --expiring N` (N up to
825, the longest life any profile allows) and `list --cn <purpose>-ca`
cover them too. Monitor each issuing CA's expiry externally and well
ahead: issuance stops as soon as a certificate would outlive its issuer -
that is, that CA's `ee_valid_days` *before* its own `notAfter` - and
`--expiring <ee_valid_days>` flags the CA no later than that moment.

## Revoke

```bash
yca revoke server --cn server.example.ca --reason keyCompromise
yca revoke client --cn "Client Name"        # reason defaults to unspecified
```

Revocation rewrites the signed CRL (`<store>/ca/<signing-slug>.crl`, DER) -
the only revocation channel: this PKI is CRL-only, there is no OCSP.
CSR-signed certificates revoke
exactly like `create`d ones. After a revocation the CN is free for re-issue:

```bash
yca create server --cn server.example.ca          # or another sign round
```

The two CRLs run on separate cadences. The signing CRL carries a 7-day
`nextUpdate`; the root CRL carries a 183-day one (industry-standard
offline-root practice sits in the 6-12 month band; yca borrows the
cadence; the isolation depends on the layout - with a single token or
store the root key is reachable whenever the signing key is, while the
split and hybrid layouts keep the root on its own token, see the README
limitations). Both are clamped so the promise never extends past the
issuing CA's own `notAfter`. Keep them with:

```bash
yca refresh crl signing   # daily: re-signs the signing CRL, fresh nextUpdate
yca refresh crl root      # quarterly: same for the root CRL, root key needed
yca refresh crl           # both (scope defaults to all)
```

Each scope wants only its own secret: `signing` the signing secret,
`root` the root secret (`CA_HSM_ROOT_PIN` in the split and hybrid
layouts - plug the root token in for it). `all` wants both.

Run each scope on a schedule shorter than its horizon
(`share/systemd/yca-crl-refresh.{service,timer}` does signing daily,
`share/systemd/yca-root-crl-refresh.{service,timer}` does root quarterly;
the hourly publish timer picks the re-signed files up). The `signing` scope
never loads the root key, so the routine daily job does not touch it.
Each refresh (and each revoke) also prunes entries whose certificate has
expired, per RFC 5280 3.3: an entry stays until one scheduled CRL issued
beyond the certificate's validity has carried it, then drops off. The CRL
therefore holds at most `ee_valid_days` worth of revocations regardless of
the CA's age; the store's `cert_index` keeps the full revocation history.
Without a refresh, strict CRL checkers consider a CRL stale once its
`nextUpdate` passes - and the CRLs are the only revocation channel
(CRL-only PKI), so the timers matter.

Trade-off of the 6-month root horizon: relying parties may keep serving a
cached root CRL until its `nextUpdate`, so revoking the signing CA becomes
visible to them only after a fresh root CRL is published. After any signing
CA revocation, run `yca refresh crl root` immediately instead of waiting
for the quarterly timer (and let the publish timer push it out).

See [Installation](install.md) for the timers and the publishing pipeline.

## Add an issuing CA

An issuing CA exists per purpose - `[ca.tls]`, `[ca.email]` - and each
carries the EE profiles it is allowed to issue. `init` creates every
purpose the config declares at that moment; one declared later is created
with its own ceremony, without re-initializing the store.

This is a **root key ceremony**: in the split or hybrid HSM layouts the
root token has to come out of the safe, exactly as for a rotation.

```bash
# 1. Declare it.
sudoedit /etc/yca/yca.toml
# [ca.email]
# profiles = ["email"]
# cn = "Email CA E1"
# curve = "secp384r1"
# digest = "SHA-384"
# valid_days = 1194
# slug_prefix = "email-e"
# ee_curve = "secp256r1"
# ee_digest = "SHA-256"
# ee_valid_days = 825
# permitted_email = ["example.ca"]

# 2. Create it (root key online)
yca add signing-ca --purpose email
Email CA E1                              # the new CN, on stdout

# 3. Publish (or wait for the timer)
sudo systemctl start yca-publish.service
curl -I http://pki.example.ca/email-e1.crt
curl -I http://pki.example.ca/email-e1.crl
```

The section is locked into the store as the CA is created, so from then
on it behaves like every other: edits to it are warned and ignored.
Until step 2 runs, the purpose is declared but does not exist, and
issuance for its profiles is refused rather than silently routed
somewhere else.

Which CA signs a certificate follows from the profile: `create server`
goes to whichever CA lists `server`. A profile no CA claims is not a
configuration error - that PKI simply does not issue it, and issuance
says so.

## CA rotation - the renewal ceremony

Rotating the signing CA never touches a relying party's trust store: old
leaves keep validating through the old generation, new leaves through the
new one, and both chains end at the same root. That is what the 2-tier
hierarchy buys. Rotating the **root** is the expensive case and has its
own scenario in [CA rotation](ca-rotation.md).

### When to rotate

| Trigger | Urgency | What it looks like |
|---|---|---|
| aging out | calendar | issuance stops `ee_valid_days` before the issuing CA's `notAfter`; renew well before that |
| key compromise | hours | renew, re-issue, then revoke the old generation |
| policy migration | calendar | new curve or digest; behaves like an aging-out renewal |

The aging-out horizon is visible without any new tooling:

```bash
yca list --expiring 397     # an issuing CA appears once it is within
                            # its own ee_valid_days of its notAfter
```

Do not wait for that: at that point issuance is about to start refusing.

### Preconditions

- The **root key online**: a renewal is signed by the root, exactly like a
  root CRL refresh. With a `pkcs11` root that means its token present and
  `CA_HSM_PIN` (or `CA_HSM_ROOT_PIN` in the split and hybrid layouts) in
  the environment. The renewal also needs the signing side's secret: the
  new generation's key is created on its backend.
- A **new display name** for the generation. Slugs derive themselves
  (`ca-e1` -> `ca-e2`, from that CA's `slug_prefix`), but the CN is
  yours to pick, and it must differ from every existing generation's.
- With a `pkcs11` backend on that CA, **room on its token** for one more
  keypair (the new generation gets its own, labeled with its slug).

### Planned rotation, step by step

Rotation is per issuing CA: it moves one purpose's lineage and leaves
every other CA on its current generation. With a single issuing CA -
the ordinary case, and what this runbook assumes - `--purpose` may be
omitted and is inferred; with several it is required, and the bare
`signing-ca` alias below becomes `<purpose>-ca`.

```bash
# 1. Create the next generation (root key online)
yca renew signing-ca --new-cn "CA E2"
CA E2                                    # the new CN, on stdout

# 2. Confirm the switch: E2 active, E1 retiring
yca list --cn signing-ca
# CN     KIND     SERIAL    EXPIRES     STATUS
# CA E2  signing  1A2B...   2035-01-01  active
# CA E1  signing  0F9E...   2033-06-01  active     <- still an issuer of record

# 3. Publish the new artifacts (or wait for the hourly timer)
sudo systemctl start yca-publish.service
curl -I http://pki.example.ca/ca-e2.crt
curl -I http://pki.example.ca/ca-e2.crl
```

From step 1 on, every new certificate chains to E2: `create`, `sign` and
the ACME frontend all follow the active generation, with **nothing to
reconfigure** - `yca-acme` fetches the chain through `get ca --cn
signing-ca`, which is an alias for "whichever generation is active".

### The overlap

E1 stays an **issuer of record** until the last certificate it signed has
expired. Concretely, during that window:

- its certificate stays published, because leaves issued under it carry
  `caIssuers` URLs pointing at `ca-e1.crt`;
- its CRL keeps being re-signed: `yca refresh crl signing` covers every
  live generation, so the daily timer already does this with no change;
- revoking one of its leaves rewrites **its** CRL, not E2's: `revoke`
  follows the certificate's issuer, not whichever generation signs today;
- `yca list --cn signing-ca` shows both, so the overlap is auditable.

Nothing has to be scheduled for the retirement itself: once E1 expires,
the refresh skips it (logging `skipping ca-e1: expired ...`) and it can be
unpublished at leisure.

### Verify a rotation

```bash
yca get ca --cn root-ca > root.pem
yca get ca --cn "CA E1" > ca-e1.pem
yca get ca --cn "CA E2" > ca-e2.pem
# A leaf issued before the rotation still verifies through the old chain
yca get server --cn old.example.ca |
  openssl verify -CAfile root.pem -untrusted ca-e1.pem
# A leaf issued after it verifies through the new one
yca create server --cn new.example.ca --valid 5m
yca get server --cn new.example.ca |
  openssl verify -CAfile root.pem -untrusted ca-e2.pem
# Both CRLs are fresh
for cn in "CA E1" "CA E2"; do
  yca get crl --cn "$cn" | openssl crl -noout -lastupdate -nextupdate
done
```

### Emergency rotation (key compromise)

Order matters: **replace first, revoke second**, so issuance never has a
gap and the fleet has somewhere to renew to.

```bash
# 1. New generation
yca renew signing-ca --new-cn "CA E2"
# 2. Re-issue what matters under E2 (ACME clients renew on their own;
#    ARI can pull that forward)
yca create server --cn critical.example.ca
# 3. Revoke the compromised generation on the ROOT CRL
yca revoke ca --cn "CA E1" --reason cACompromise
# 4. Publish it immediately - do not wait for the quarterly timer
sudo systemctl start yca-publish.service

# 5. Tell the ACME fleet to replace what E1 signed, at once
sudo -u yca yca-acme ari accelerate --state /var/lib/yca/acme.db \
    --issuer "CA E1" --window 2h
```

Step 5 only matters if the ACME frontend is in use: it moves the renewal
window `renewalInfo` advertises to "now", so clients re-issue on their
next poll instead of at 67% of their lifetime. See
[acme-operation.md](acme-operation.md#accelerating-renewal-after-a-ca-compromise).

Step 3 rewrites the root CRL with the new entry *and* a fresh
`nextUpdate`, so there is no `refresh crl root` to run afterwards; what
is left is publication. That is the price of the 6-month root horizon:
relying parties may keep serving a cached root CRL until its own
`nextUpdate`, so clients that fetched it recently keep trusting E1 until
their copy expires. That window is the deliberate trade-off for a root
key that stays quiet the rest of the year.

The order is enforced, not merely advised: `revoke ca` refuses to revoke
the active issuer, so step 1 cannot be skipped. Revoking the root is
refused outright - a trust anchor is dropped by the relying parties, not
revoked by itself.

Note that revoking E1 kills **every** chain through it at once - there is
no need to revoke its leaves individually.

### If a ceremony is interrupted

A renewal writes its artifacts first and records the generation second,
so the failure modes are ordered: until `ca_cert_index` says a generation
is `active`, issuance keeps using the previous one. An interrupted run
therefore leaves a working CA, possibly with orphan `ca-e2.*` files - the
next run warns (`exists from an unfinished renewal; replacing it`) and
replaces them. With `pkcs11` a keypair already generated under the new
slug is adopted (matching label, curve-checked), not duplicated.

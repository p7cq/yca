# Nitrokey HSM 2

## User PIN retry counter

The token enforces a hardware retry counter on the user PIN (3 tries).
When it reaches 0 the user PIN locks and only the SO-PIN can reset it.
Check it any time with:

```console
$ sc-hsm-tool | grep "PIN tries"
User PIN tries left  : 3
```

`yca` makes exactly one login attempt per token per command - never a
retry loop - so a mistyped PIN costs at most one try. In the split
layout each Nitrokey is its own token with its own counter; the root
token's PIN comes from `CA_HSM_ROOT_PIN` (falling back to
`CA_HSM_PIN`).

Not every login failure consumes a try, though. OpenSC validates the PIN
*length* in software (the HSM 2 user PIN is 6-16 characters) and rejects
an out-of-range PIN before anything is sent to the card:

- **wrong PIN, valid length** (e.g. one character *replaced*) ->
  `CKR_PIN_INCORRECT` (PKCS11 error 160): the attempt reached the card and
  **the counter decrements**;
- **out-of-range length** (e.g. one character *appended* to an already
  maximum-length PIN) -> rejected client-side as `CKR_DATA_LEN_RANGE`
  (PKCS11 error 33, rendered by Botan as "plaintext data length out of
  range"; the PKCS#11 spec name for this case would be `CKR_PIN_LEN_RANGE`):
  nothing reached the card and **the counter is untouched**;
- `CKR_PIN_LOCKED` (PKCS11 error 164): the counter already hit 0 - unlock
  with the SO-PIN.

yca wraps every login failure in the same "PKCS#11 login failed: ..."
message: look at the trailing PKCS11 error code to tell whether the attempt
actually consumed a try.

SoftHSM has no retry counter at all - see
[softhsm.md](softhsm.md#differences-vs-the-nitrokey).

## Key backup (DKEK)

A SmartCard-HSM can export a private key, but only wrapped under a
Device Key Encryption Key, and only if it was initialized with one.

- The DKEK is fixed at `sc-hsm-tool --initialize --dkek-shares n`, and
  initialization erases the device. There is no retrofit.
- Shares are XOR-combined, so all n must be imported: n-of-n split
  custody, not an m-of-n threshold.
- A wrapped key restores only into a device holding the same DKEK.

The state is readable without logging in, so it costs no PIN try:

```console
$ sc-hsm-tool --reader 0
DKEK shares          : 1
DKEK key check value : A1B2C3D4E5F60789
```

A key check value and no "import pending" line means the DKEK is
complete and `--wrap-key` is available. No DKEK line at all means the
device was initialized with zero shares: nothing leaves it and nothing
can be restored into it, ever.

```console
$ sc-hsm-tool --reader 0 --wrap-key root-e1.wrap --key-reference 1 --pin ...
$ sc-hsm-tool --reader 1 --unwrap-key root-e1.wrap --key-reference 1 --pin ...
```

What it costs: the key stops being unextractable, which is the property
the token is bought for. The risk moves to the custody of the share
files, and a restored key means two live copies of one private key. That
is a defensible trade for a root and rarely one for an online signing CA.

This is the alternative to the standby root in
[ca-rotation.md](ca-rotation.md#scenario-d-standby-root-the-token-death-case),
and it has nothing to do with cross-signing: a shared DKEK puts the
*same* root key on a second token, with no cross-certificate, no chain
change and no code. The standby root instead accepts that the key is
lost and buys continuity with a bridge certificate. Same problem from
opposite ends; picking one is a custody decision, not a PKI one.

# SoftHSM2 - dev/test token for the `pkcs11` key backend

SoftHSM2 is a software PKCS#11 token: same API and workflow as a hardware
HSM (Nitrokey), but keys live in files on disk. Use it to develop and test
the `key_backend = "pkcs11"` flow without touching real hardware. The
examples below mirror the production setup: token label `ets`, CA key
labels `root-e1` / `ca-e1` (= the CA slugs from config).

## Install

| OS | Package | Module path |
|----|---------|-------------|
| Arch | `pacman -S softhsm` | `/usr/lib/softhsm/libsofthsm2.so` (also `/usr/lib/pkcs11/libsofthsm2.so`) |
| Debian/Ubuntu | `apt install softhsm2` | `/usr/lib/x86_64-linux-gnu/softhsm/libsofthsm2.so` |
| macOS | `brew install softhsm` | `/opt/homebrew/lib/softhsm/libsofthsm2.so` |

`pkcs11-tool` (package `opensc`) is useful for inspection but not required.

**Version requirement: SoftHSM >= 2.7.0.** Botan signs X.509 objects through
PKCS#11 with the hash-on-token mechanisms (`CKM_ECDSA_SHA384` for a SHA-384
digest, etc.); SoftHSM 2.6.1 only offers raw `CKM_ECDSA`, so `yca init`
fails at the very first signature with `PKCS11 error 112`
(`CKR_MECHANISM_INVALID`) - key generation still succeeds, which makes the
failure look confusing. 2.7.0 added the `ECDSA-SHA1..512` mechanisms
(check with `pkcs11-tool --module <path> -M | grep ECDSA`). Debian trixie
still ships 2.6.1; Arch ships 2.7.0. Real tokens driven by OpenSC (the
Nitrokey) are unaffected: OpenSC advertises the hash mechanisms and hashes
in software before handing the card a raw ECDSA operation.

## Where tokens live

SoftHSM reads `SOFTHSM2_CONF` (env var), falling back to
`~/.config/softhsm2/softhsm2.conf`, then `/etc/softhsm2.conf`. The config
names the token directory:

```ini
directories.tokendir = /var/lib/softhsm/tokens
objectstore.backend = file
```

For an isolated playground (like `tests/e2e-hsm.sh` does), point
`SOFTHSM2_CONF` at a private copy with its own `tokendir` - nothing global
is touched:

```bash
mkdir -p ~/softhsm-dev/tokens
cat > ~/softhsm-dev/softhsm2.conf <<EOF
directories.tokendir = $HOME/softhsm-dev/tokens
objectstore.backend = file
EOF
export SOFTHSM2_CONF=~/softhsm-dev/softhsm2.conf
```

## Initialize a token

A fresh SoftHSM slot shows up as *present but uninitialized* (no label, no
PIN) - `yca init` will fail with `no token labeled 'ets' found` until you
initialize it:

```bash
softhsm2-util --init-token --free --label ets --pin 1234 --so-pin 87654321
```

> **Note:** SoftHSM must be fully set up *before* any `yca` command that
> touches the token: a readable config (`SOFTHSM2_CONF` or a system
> default), an existing `tokendir`, and an initialized token. If the config
> or `tokendir` is missing, SoftHSM's `C_Initialize` itself fails and
> `yca` can only report the raw PKCS#11 code:
>
> ```
> ERROR CA init failed: PKCS11 error 5 (unrecoverable error occurred)
> ```
>
> PKCS#11 has no more specific return code for this case (SoftHSM maps any
> config/store problem to `CKR_GENERAL_ERROR`), so if you see error 5 at
> startup, check the SoftHSM setup first - see Troubleshooting.

- `--free` picks the first uninitialized slot.
- `--pin` is the **user PIN** - the one `yca` uses (`CA_HSM_PIN`).
- `--so-pin` is the security-officer PIN (only for re-initializing/unblocking
  the token; `yca` never uses it).
- Pick real PINs outside of examples; SoftHSM accepts 4-255 characters.

## Second token (split and hybrid layouts)

The split layout keeps the root key on its own token (a `token_label`
under `[root]`), and the hybrid layout uses only that root token (the
issuing keys stay internal). Initialize it like the first, with its own
label and PINs:

```bash
softhsm2-util --init-token --free --label ets-root --pin 5678 --so-pin 87654321
```

`yca` logs into the root token with `CA_HSM_ROOT_PIN`, falling back to
`CA_HSM_PIN` when unset (the identical-PIN case). Both tokens are slots
of the same `[pkcs11] module`; nothing else changes.

## List / inspect

```bash
# slots + token labels, SoftHSM's own view
softhsm2-util --show-slots

# the PKCS#11 view yca sees (label, flags, serial)
pkcs11-tool --module /usr/lib/softhsm/libsofthsm2.so -L

# objects on the token; private objects are visible only after --login
pkcs11-tool --module /usr/lib/softhsm/libsofthsm2.so --token-label ets \
  --login --pin 1234 --list-objects
```

After a `yca init` the object listing shows the two CA keypairs, labeled
with the CA slugs:

```
Private Key Object; EC
  label:      root-e1
  ...
Private Key Object; EC
  label:      ca-e1
```

## Use with yca

Config (e.g. `config-sw.toml`) - identical to the Nitrokey one except
for the module path (and the label, if you chose another):

```toml
[pkcs11]
module = "/usr/lib/softhsm/libsofthsm2.so"
token_label = "ets"

# ...then, in each CA's own section:
#   [root]    key_backend = "pkcs11"
#   [ca.tls]  key_backend = "pkcs11"
```

```bash
export CA_HSM_PIN=1234
bin/yca --config config-sw.toml --store ./store-sw init
```

On a fresh token the log (`yca.log`, next to the store) shows the
**generate** path:

```
INFO  generating keypair 'root-e1' on the token
INFO  generating keypair 'ca-e1' on the token
```

A later init over the *same* token (different `--store`) shows the
**adopt** path, and the CA certificates carry the same public keys:

```
INFO  using existing token keypair 'root-e1'
INFO  using existing token keypair 'ca-e1'
```

Sanity check - the SPKI point must be uncompressed (`04:...`, 97 bytes for
P-384):

```bash
openssl x509 -in store-sw/ca/root-e1.pem -noout -text | grep -A3 'pub:'
```

Keep **one store directory per token**: each store locks its config at init
and its CA certificates match that token's keys - mixing stores across
tokens fails closed (`CA key '<slug>' not found on the token`).

## Key management / reset

```bash
# delete one CA keypair (both halves - yca requires the pair complete)
pkcs11-tool --module /usr/lib/softhsm/libsofthsm2.so --token-label ets \
  --login --pin 1234 --delete-object --type privkey --label root-e1
pkcs11-tool --module /usr/lib/softhsm/libsofthsm2.so --token-label ets \
  --login --pin 1234 --delete-object --type pubkey --label root-e1

# nuke the whole token (then --init-token again)
softhsm2-util --delete-token --token ets
```

Private keys are generated `CKA_SENSITIVE` and non-extractable - there is no
key export, same contract as real hardware. "Backup" of a SoftHSM token =
copying its `tokendir` (treat it as secret material).

## Differences vs the Nitrokey

- **No PIN lockout**: SoftHSM tolerates wrong PINs forever; the Nitrokey
  decrements a hardware retry counter (yca makes exactly one login attempt
  per run for this reason).
- **Keys are files**: no physical protection; dev/test only.
- **Speed**: on-token operations are local calls - fast enough that the
  `e2e-hsm` suite runs in well under a second.

## Troubleshooting

- `PKCS11 error 5 (unrecoverable error occurred)` at init - `C_Initialize`
  failed: SoftHSM could not read its configuration or token directory
  (`SOFTHSM2_CONF` pointing at a missing file, or `directories.tokendir`
  nonexistent). `softhsm2-util --show-slots` fails the same way (`Could not
  load the SoftHSM configuration`). Fix the config path, then re-run.
- `no token labeled 'ets' found via ... (1 token(s) present)` - the slot
  exists but the token is uninitialized (or labeled differently): run
  `softhsm2-util --show-slots` and initialize/match the label.
- `PKCS#11 login failed` - wrong `CA_HSM_PIN` for that token (each token has
  its own user PIN).
- Token missing entirely - check which `SOFTHSM2_CONF` is in effect; a shell
  with a stale export sees a different `tokendir`.
- `tests/e2e-hsm.sh` never touches your tokens: it builds an ephemeral
  `SOFTHSM2_CONF` + `tokendir` under `mktemp -d` and deletes them on exit.

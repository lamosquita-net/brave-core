# FlyWeb: ML-KEM and X25519MLKEM768 in BoringSSL (FS.17, base 128)

Chrome 131 offers `X25519MLKEM768` (0x11ec) in TLS 1.3 and QUIC instead of `X25519Kyber768Draft00` (0x6399). The
BoringSSL of Chromium 128 (f01108e4) already has the Keccak absorb API but no ML-KEM. These patches are upstream
BoringSSL commits, unchanged except for the build lists (no FlyWeb cryptography). Same port as FlyWeb 1.15
(`seg/ml-kem` fc0f45c433, on the 116), redone on the 128 for FlyWeb 2.0.0. MOTOR-16, 21-10-2026.

## Upstream commits (cherry-picked onto f01108e4, in this order)

| Commit | What |
|---|---|
| 500fa1f9d | Add ML-KEM (`crypto/mlkem/mlkem.cc`, `include/openssl/mlkem.h`, test vectors) |
| f1b043c28 | Silence clang-tidy warnings in ML-KEM |
| 06aff989b | Remove the unused `MLKEM1024_SHARED_SECRET_BYTES` |
| 7fb4d3da5 | Implement X25519MLKEM768 for TLS (`ssl_key_share.cc`, group and NID) |
| 72a60506d | `illegal_parameter` instead of `decode_error` for invalid key shares |

The Keccak commits of the 116 port (4df6f97cf, 261fe436f, 1b7fdbd91) are already in f01108e4.

Adapted by hand, none of it used by Chromium's build: ML-DSA entries dropped from the conflicting lists of 500fa1f9d
(the 128 has no ML-DSA), `gen/` regenerated with `util/pregenerate`, `tool/speed.cc` left as in the 128, `go.mod` from
upstream (the runner needs `filippo.io/mlkem768`), and the one-line runner change of 72a60506d left out (the 128's
curve tests have another layout). None of those files is in these patches: only what Chromium compiles, plus the
tests. Chromium's lists are in `patches/third_party-boringssl-BUILD.generated{,_tests}.gni.patch`, written by
BoringSSL's `util/generate_build_files.py gn` (as `roll_boringssl.py` does; on f01108e4 it gives exactly the 128's
files). Kyber stays (the kill switch needs it). The upstream test vectors have trailing spaces: `git apply` only warns.

## Chromium side

`net/socket/ssl_client_socket_impl.cc` and `net/quic/quic_session_pool.cc` as Chromium 1fef3fb10d7; `kUseMLKEM`
declared and enabled in `net/base/features.{h,cc}` as 4beeb73c814 (patches, as upstream; the about:flags entry and the
policy descriptions are not ported). `PostQuantumKyber` is on by default in the 128 on desktop and still switches all
post-quantum key agreement.

**Kill switch:** `--disable-features=UseMLKEM` goes back to Kyber (0x6399) without rebuilding.

## Expected ClientHello (for LOCAL's JA4 against Chrome for Testing 131)

- `supported_groups`: GREASE, **0x11ec** (X25519MLKEM768), 0x001d (X25519), 0x0017 (P-256), 0x0018 (P-384).
- `key_share`: GREASE, **0x11ec** (1216 bytes: the ML-KEM-768 encapsulation key, 1184 bytes, then the X25519 share, 32
  bytes) and **0x001d** (32 bytes: BoringSSL adds a classical share when the first group is post-quantum).
- QUIC: the same groups and order.

## Tests (cloud, BoringSSL built alone with CMake and clang, Linux x64)

- `crypto_test` ML-KEM, Keccak and Kyber: **18/18** (BoringSSL's and NIST's vectors, ML-KEM-768 and ML-KEM-1024).
- `crypto_test` complete: **1467/1467**. `ssl_test`: **365/365**.
- BoringSSL's TLS runner, complete (with the handshaker): **9285/9285**; `CurveTest-*`, `*KeyShare*`, `*Kyber*`,
  `*MLKEM*`: 213/213.
- The 23 patches apply on a clean f01108e4 and give exactly the tested tree for every file they touch.

**Not compiled in FlyWeb here**: LOCAL builds it and checks JA4 against CfT 131, a real connection to servers that
accept X25519MLKEM768 (Google, Cloudflare) and to one that does not, and the kill switch.

# FlyWeb: ML-KEM and X25519MLKEM768 in BoringSSL (FS.17, engine level 131)

Chrome 131 offers `X25519MLKEM768` (0x11ec) in TLS 1.3 and QUIC instead of `X25519Kyber768Draft00` (0x6399). The
BoringSSL of Chromium 116 (ae88f198) has no ML-KEM. These patches are upstream BoringSSL commits, unchanged except for
the build lists (no FlyWeb cryptography). SEGURIDAD-PORTES-3, 15-10-2026. **Only for FlyWeb 1.15 (level 131)**:
FlyWeb 1.14 declares Chrome 130, which still sends Kyber.

## Upstream commits (cherry-picked onto ae88f198, in this order)

| Commit | What |
|---|---|
| 4df6f97cf | Move Keccak to `crypto/keccak` and add an absorb API (ML-KEM needs it; Kyber adapted by upstream) |
| 261fe436f | Don't prematurely run `keccak_f` in squeeze |
| 1b7fdbd91 | Unroll pi and rho in Keccak |
| 500fa1f9d | Add ML-KEM (`crypto/mlkem/mlkem.cc`, `include/openssl/mlkem.h`, test vectors) |
| f1b043c28 | Silence clang-tidy warnings in ML-KEM |
| 06aff989b | Remove the unused `MLKEM1024_SHARED_SECRET_BYTES` |
| 7fb4d3da5 | Implement X25519MLKEM768 for TLS (`ssl_key_share.cc`, group and NID) |
| 72a60506d | `illegal_parameter` instead of `decode_error` for invalid key shares |

Adapted by hand: only the CMake source lists and the Go test runner (neither is used by Chromium's build). Chromium's
lists are in `patches/third_party-boringssl-BUILD.generated{,_tests}.gni.patch`. Kyber stays (the kill switch needs
it). One patch per file; renamed files (`crypto/kyber/keccak.c` → `crypto/keccak/keccak.c`, `keccak_tests.txt`) are a
deletion plus an addition. The test vectors from upstream have trailing spaces: `git apply` only warns.

Later upstream ML-KEM fixes (to 2026-10): 3ac4542e6 fixes a 2026 regression this version does not contain.

## Chromium side

`net/socket/ssl_client_socket_impl.cc` and `net/quic/quic_stream_factory.cc` as Chromium 1fef3fb10d7 (the QUIC file is
`quic_session_pool.cc` there); `kUseMLKEM` declared and enabled in `chromium_src/net/base/features.{h,cc}` as
4beeb73c814. `PostQuantumKyber` (enabled in FlyWeb since F4.8) still switches all post-quantum key agreement.

**Kill switch:** `--disable-features=UseMLKEM` goes back to Kyber (0x6399) without rebuilding.

## Expected ClientHello (for LOCAL's JA4 against Chrome for Testing 131)

- `supported_groups`: GREASE, **0x11ec** (X25519MLKEM768), 0x001d (X25519), 0x0017 (P-256), 0x0018 (P-384).
- `key_share`: GREASE, **0x11ec** (1216 bytes: the ML-KEM-768 encapsulation key, 1184 bytes, then the X25519 share, 32
  bytes; the reverse of Kyber's order) and **0x001d** (32 bytes: BoringSSL adds a classical share when the first group
  is post-quantum).
- QUIC: the same groups and order.

## Tests (cloud, BoringSSL built alone with CMake and clang, Linux x64)

- `crypto_test` ML-KEM, Keccak and Kyber: **18/18** (BoringSSL's and NIST's vectors, ML-KEM-768 and ML-KEM-1024).
- `crypto_test` complete: 1360/1361; the one is `BIOTest.SocketConnect`, which needs IPv6 (missing in the container).
- `ssl_test`: **339/339**.
- BoringSSL's TLS runner (`CurveTest-*`, `*KeyShare*`, `*Kyber*`, `*MLKEM*`, with the handshaker): **213/213**, with
  the runner and shim switched to IPv4 for the container only.
- The 32 patches apply on a clean ae88f198 and give exactly the tested tree.

**Not compiled in FlyWeb**: LOCAL builds it and checks JA4 against CfT 131, a real connection to servers that accept
X25519MLKEM768 (Google, Cloudflare) and to one that does not, and the kill switch.

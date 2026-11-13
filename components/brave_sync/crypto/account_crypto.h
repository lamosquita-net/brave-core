/* Copyright (c) 2026 The FlyWeb Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_COMPONENTS_BRAVE_SYNC_CRYPTO_ACCOUNT_CRYPTO_H_
#define BRAVE_COMPONENTS_BRAVE_SYNC_CRYPTO_ACCOUNT_CRYPTO_H_

// FlyWeb 3.2: cryptography of the sync account (FlyWeb/docs/sync-cuenta.md
// §3.1, §4 and §11; protocol in FlyWeb/servidor/sync/cuenta/README.md).
//
// The account wraps the chain: the 32-byte seed travels to a new Mac wrapped
// with a key that only the password opens. From the password and the email:
//
//   KE = scrypt(password, salt = SHA-256("flyweb-cuenta-v1" || email),
//               N = 2^17, r = 8, p = 1, 32 bytes)
//   KL = HKDF-SHA256(KE, info = "flyweb-cuenta-v1/entrar")    -> to the server
//   KU = HKDF-SHA256(KE, info = "flyweb-cuenta-v1/envolver")  -> never leaves
//   E  = nonce(12) || AES-256-GCM(KU, nonce, seed,
//                       ad = "flyweb-envoltura-v1" || SHA-256(email))
//
// and the one-use chain proof, signed with the chain's Ed25519 key:
//
//   M = "flyweb-cuenta-v1" || lp(method) || lp(path) || lp(op) ||
//       lp(account_id) || lp(nonce) || SHA-256(body),  lp(x) = u16be(|x|) || x
//
// Everything is BoringSSL from the tree; nothing depends on macOS.
// The password, KE, KL and KU are never stored: they live in SecretKey
// objects that wipe themselves with OPENSSL_cleanse.

#include <stddef.h>
#include <stdint.h>

#include <array>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "base/containers/span.h"

namespace brave_sync::crypto::account {

inline constexpr size_t kKeySize = 32;
inline constexpr size_t kSeedSize = 32;
inline constexpr size_t kGcmNonceSize = 12;
inline constexpr size_t kGcmTagSize = 16;
// nonce || encrypted seed || tag (§4.4); the server checks this size.
inline constexpr size_t kWrappedSeedSize =
    kGcmNonceSize + kSeedSize + kGcmTagSize;
inline constexpr size_t kProofSignatureSize = 64;
inline constexpr size_t kChainPublicKeySize = 32;
inline constexpr size_t kChainPrivateKeySize = 64;

// Client stretching (§4, «Coste del estiramiento»). Tied to the
// "flyweb-cuenta-v1" label: changing them needs a v2 and every user typing
// the password again. P23 decides between 2^17 and 2^16 before publishing.
inline constexpr uint64_t kScryptN = uint64_t{1} << 17;
inline constexpr uint64_t kScryptR = 8;
inline constexpr uint64_t kScryptP = 1;
// EVP_PBE_scrypt refuses more than 65 MiB by default; 2^17 needs 128 MiB.
inline constexpr size_t kScryptMaxMem = size_t{130} * 1024 * 1024;

// Minimum password length, in code points after NFC (§4.1).
inline constexpr size_t kMinPasswordLength = 10;

// A 32-byte secret (KE, KL, KU, a seed) wiped on destruction. Move-only.
class SecretKey {
 public:
  SecretKey();
  explicit SecretKey(base::span<const uint8_t, kKeySize> bytes);
  SecretKey(SecretKey&& other);
  SecretKey& operator=(SecretKey&& other);
  SecretKey(const SecretKey&) = delete;
  SecretKey& operator=(const SecretKey&) = delete;
  ~SecretKey();

  base::span<const uint8_t, kKeySize> bytes() const { return bytes_; }
  base::span<uint8_t, kKeySize> mutable_bytes() { return bytes_; }

 private:
  std::array<uint8_t, kKeySize> bytes_;
};

// The chain's Ed25519 key pair, derived from the seed exactly as
// BraveSyncAuthManager::DeriveSigningKeys does (HKDF-SHA512 with Brave's salt
// and "sync-auth-key"), so the public key is the go-sync client_id. The
// private half is wiped on destruction.
class ChainKeys {
 public:
  ChainKeys();
  ChainKeys(ChainKeys&& other);
  ChainKeys& operator=(ChainKeys&& other);
  ChainKeys(const ChainKeys&) = delete;
  ChainKeys& operator=(const ChainKeys&) = delete;
  ~ChainKeys();

  static ChainKeys FromSeed(base::span<const uint8_t, kSeedSize> seed);
  ChainKeys Clone() const;

  // Upper-case hex of the public key: the X-FlyWeb-Cadena header and the
  // go-sync client_id.
  std::string ChainId() const;
  base::span<const uint8_t, kChainPublicKeySize> public_key() const {
    return public_key_;
  }
  bool empty() const { return empty_; }

  // Signs the chain proof (§3.1, decision 10 of §10.1). `path` is the full
  // path the server sees ("/cuenta/v1/eventos"); `account_id` is empty in
  // alta/confirmar and empezar-de-cero/confirmar; `body` is the exact HTTP
  // body.
  std::array<uint8_t, kProofSignatureSize> SignProof(
      std::string_view method,
      std::string_view path,
      std::string_view op,
      std::string_view account_id,
      base::span<const uint8_t> nonce,
      std::string_view body) const;

 private:
  bool empty_ = true;
  std::array<uint8_t, kChainPublicKeySize> public_key_{};
  std::array<uint8_t, kChainPrivateKeySize> private_key_{};
};

// The proof message M; exposed for the tests.
std::vector<uint8_t> BuildProofMessage(std::string_view method,
                                       std::string_view path,
                                       std::string_view op,
                                       std::string_view account_id,
                                       base::span<const uint8_t> nonce,
                                       std::string_view body);

// KL and KU from one stretching. KE is wiped before returning.
struct AccountKeys {
  AccountKeys();
  AccountKeys(AccountKeys&&);
  AccountKeys& operator=(AccountKeys&&);
  ~AccountKeys();

  SecretKey login_key;  // KL
  SecretKey wrap_key;   // KU
};

// Email as the server normalises it (§4.1; cuenta/cripto.go NormalizarEmail):
// trimmed, lower case (simple case mapping, like Go's strings.ToLower), NFC.
// std::nullopt if it is not a plain address (one '@', a dot in the domain,
// no spaces or controls, 3–254 bytes). The server does the full check.
std::optional<std::string> NormalizeEmail(std::string_view email);

// Password in NFC (§4.1). Not trimmed: spaces are part of it.
std::string NormalizePassword(std::string_view password);

// Number of code points, for kMinPasswordLength.
size_t CountCodePoints(std::string_view utf8);

// The slow step: ~128 MiB and around a second of one core. Never on the UI
// thread. Both inputs must already be normalised. std::nullopt only if
// scrypt fails (out of memory).
std::optional<AccountKeys> DeriveAccountKeys(std::string_view password,
                                             std::string_view email);

// Same with another N, for the tests and the P23 timing only.
std::optional<AccountKeys> DeriveAccountKeysWithCostForTesting(
    std::string_view password,
    std::string_view email,
    uint64_t scrypt_n);

// E for `seed` (§4.4) with a random nonce. `email` normalised.
std::vector<uint8_t> WrapSeed(const SecretKey& wrap_key,
                              base::span<const uint8_t, kSeedSize> seed,
                              std::string_view email);
std::vector<uint8_t> WrapSeedWithNonceForTesting(
    const SecretKey& wrap_key,
    base::span<const uint8_t, kSeedSize> seed,
    std::string_view email,
    base::span<const uint8_t, kGcmNonceSize> nonce);

// Opens E. std::nullopt if it does not authenticate: changed by the server,
// made with another password or for another email (§4.4, P15).
std::optional<SecretKey> UnwrapSeed(const SecretKey& wrap_key,
                                    base::span<const uint8_t> wrapped,
                                    std::string_view email);

// Wipes a string that held a password (std::string has no wipe of its own).
void WipeString(std::string& s);

}  // namespace brave_sync::crypto::account

#endif  // BRAVE_COMPONENTS_BRAVE_SYNC_CRYPTO_ACCOUNT_CRYPTO_H_

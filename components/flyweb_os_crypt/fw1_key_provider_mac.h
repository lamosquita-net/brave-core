/* Copyright (c) 2026 lamosquita. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_COMPONENTS_FLYWEB_OS_CRYPT_FW1_KEY_PROVIDER_MAC_H_
#define BRAVE_COMPONENTS_FLYWEB_OS_CRYPT_FW1_KEY_PROVIDER_MAC_H_

#include <cstddef>

#include "base/memory/raw_ptr.h"
#include "base/types/expected.h"
#include "components/os_crypt/async/browser/key_provider.h"

namespace crypto::apple {
class KeychainV2;
}  // namespace crypto::apple

namespace os_crypt_async {

// FlyWeb `fw1` key provider (FS.25): AES-256-GCM with a random 256-bit key
// kept in the classic login keychain ("FlyWeb Safe Storage" / "FlyWeb fw1"),
// Base64 encoded. No KDF, no T2 or Secure Enclave, so it works the same on
// Mojave and later. Same item and format as FlyWeb 2.5-2.7 (which read it
// through crypto::AppleKeychain); 156 reads it through KeychainV2, which
// still queries the file-based login keychain.
//
// Registered next to upstream's KeychainKeyProvider (`v10`, precedence 10),
// so `fw1` encrypts new data whenever its key loads, and `v10` takes over
// encryption when it does not, as in 2.x. Every failure is reported as
// kTemporarilyUnavailable: callers may delete data on a permanent failure.
class Fw1KeyProvider : public KeyProvider {
 public:
  static constexpr char kKeyTag[] = "fw1";
  // Above the 10 of upstream's KeychainKeyProvider.
  static constexpr size_t kPrecedence = 20u;

  Fw1KeyProvider();
  // For tests. `keychain` must outlive the GetKey() reply.
  explicit Fw1KeyProvider(crypto::apple::KeychainV2* keychain);

  Fw1KeyProvider(const Fw1KeyProvider&) = delete;
  Fw1KeyProvider& operator=(const Fw1KeyProvider&) = delete;

  ~Fw1KeyProvider() override;

  // Reads the key from the keychain, creating it on first use. Never builds a
  // Key from an item of the wrong length (Encryptor::Key CHECKs the size) and
  // never replaces an existing item.
  static base::expected<Encryptor::Key, KeyError> GetOrCreateKey(
      crypto::apple::KeychainV2& keychain);

 private:
  // KeyProvider:
  void GetKey(KeyCallback callback) override;
  bool UseForEncryption() override;

  raw_ptr<crypto::apple::KeychainV2> keychain_for_testing_ = nullptr;
};

}  // namespace os_crypt_async

#endif  // BRAVE_COMPONENTS_FLYWEB_OS_CRYPT_FW1_KEY_PROVIDER_MAC_H_

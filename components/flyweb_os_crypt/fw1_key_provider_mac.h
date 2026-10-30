/* Copyright (c) 2026 lamosquita. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_COMPONENTS_FLYWEB_OS_CRYPT_FW1_KEY_PROVIDER_MAC_H_
#define BRAVE_COMPONENTS_FLYWEB_OS_CRYPT_FW1_KEY_PROVIDER_MAC_H_

#include <memory>
#include <optional>

#include "components/os_crypt/async/browser/key_provider.h"

namespace crypto {
class AppleKeychain;
}  // namespace crypto

namespace os_crypt_async {

// FlyWeb `fw1` key provider (FS.25): AES-256-GCM with a random 256-bit key
// kept in the classic login keychain ("FlyWeb Safe Storage" / "FlyWeb fw1").
// No KDF, no T2 or Secure Enclave, so it works the same on Mojave and later.
// Data written with this key carries the "fw1" prefix and cannot be read by
// OSCrypt sync; `v10` data stays readable through the OSCrypt fallback.
class Fw1KeyProvider : public KeyProvider {
 public:
  static constexpr char kKeyTag[] = "fw1";
  // Above the 10 of upstream's Keychain provider (level 143+), so `fw1` keeps
  // encrypting if that provider is ever registered next to it.
  static constexpr size_t kPrecedence = 20u;

  Fw1KeyProvider();
  // For tests. The keychain is moved into the thread pool task on GetKey().
  explicit Fw1KeyProvider(std::unique_ptr<crypto::AppleKeychain> keychain);

  Fw1KeyProvider(const Fw1KeyProvider&) = delete;
  Fw1KeyProvider& operator=(const Fw1KeyProvider&) = delete;

  ~Fw1KeyProvider() override;

  // Reads the key from the keychain, creating it on first use. Never builds a
  // Key from an item of the wrong length (Encryptor::Key CHECKs the size).
  static std::optional<Encryptor::Key> GetOrCreateKey(
      const crypto::AppleKeychain& keychain);

 private:
  // KeyProvider:
  void GetKey(KeyCallback callback) override;
  bool UseForEncryption() override;
  bool IsCompatibleWithOsCryptSync() override;

  std::unique_ptr<crypto::AppleKeychain> keychain_for_testing_;
};

}  // namespace os_crypt_async

#endif  // BRAVE_COMPONENTS_FLYWEB_OS_CRYPT_FW1_KEY_PROVIDER_MAC_H_

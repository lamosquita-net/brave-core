/* Copyright (c) 2026 lamosquita. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "brave/components/flyweb_os_crypt/fw1_key_provider_mac.h"

#include <Security/Security.h>

#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "base/apple/osstatus_logging.h"
#include "base/base64.h"
#include "base/command_line.h"
#include "base/containers/span.h"
#include "base/functional/bind.h"
#include "base/logging.h"
#include "base/metrics/histogram_functions.h"
#include "base/task/thread_pool.h"
#include "components/os_crypt/async/common/algorithm.mojom.h"
#include "components/os_crypt/common/os_crypt_switches.h"
#include "crypto/apple/keychain_v2.h"
#include "crypto/random.h"
#include "third_party/boringssl/src/include/openssl/mem.h"

namespace os_crypt_async {

namespace {

// Same service as the `v10` item (chromium_src keychain_password_mac.mm), so
// both show up together in Keychain Access; a different account.
constexpr char kServiceName[] = "FlyWeb Safe Storage";
constexpr char kAccountName[] = "FlyWeb fw1";

// Recorded once per startup, to see how often `fw1` falls back to `v10`.
// Same values as in 2.x.
enum class Fw1KeyResult {
  kFound = 0,
  kCreated = 1,
  kFindFailed = 2,
  kAddFailed = 3,
  kBadItem = 4,
  kMockKeychain = 5,
  kMaxValue = kMockKeychain,
};

void RecordResult(Fw1KeyResult result) {
  base::UmaHistogramEnumeration("FlyWeb.OSCrypt.Fw1KeyResult", result);
}

void Cleanse(std::string& s) {
  OPENSSL_cleanse(s.data(), s.size());
}

void Cleanse(std::vector<uint8_t>& v) {
  OPENSSL_cleanse(v.data(), v.size());
}

base::unexpected<KeyProvider::KeyError> Unavailable(Fw1KeyResult result) {
  RecordResult(result);
  // Never kPermanentlyUnavailable: the key may load on the next start (the
  // user unlocks the keychain or allows access) and callers may delete data
  // they think is lost for good. `v10` encrypts meanwhile.
  return base::unexpected(KeyProvider::KeyError::kTemporarilyUnavailable);
}

base::expected<Encryptor::Key, KeyProvider::KeyError> GetKeyOnThreadPool(
    crypto::apple::KeychainV2* keychain_for_testing) {
  if (keychain_for_testing) {
    return Fw1KeyProvider::GetOrCreateKey(*keychain_for_testing);
  }
  // Same switch upstream's KeychainKeyProvider honours; tests must not touch
  // the real keychain. No `fw1` key then: the Encryptor writes `v10`.
  if (base::CommandLine::ForCurrentProcess()->HasSwitch(
          os_crypt::switches::kUseMockKeychain)) {
    return Unavailable(Fw1KeyResult::kMockKeychain);
  }
  return Fw1KeyProvider::GetOrCreateKey(
      crypto::apple::KeychainV2::GetInstance());
}

}  // namespace

Fw1KeyProvider::Fw1KeyProvider() = default;

Fw1KeyProvider::Fw1KeyProvider(crypto::apple::KeychainV2* keychain)
    : keychain_for_testing_(keychain) {}

Fw1KeyProvider::~Fw1KeyProvider() = default;

// static
base::expected<Encryptor::Key, KeyProvider::KeyError>
Fw1KeyProvider::GetOrCreateKey(crypto::apple::KeychainV2& keychain) {
  std::string encoded;
  bool created = false;
  auto found = keychain.FindGenericPassword(kServiceName, kAccountName);
  if (found.has_value()) {
    encoded.assign(found->begin(), found->end());
    Cleanse(*found);
  } else if (found.error() == errSecItemNotFound) {
    std::vector<uint8_t> raw =
        crypto::RandBytesAsVector(Encryptor::Key::kAES256GCMKeySize);
    encoded = base::Base64Encode(raw);
    Cleanse(raw);
    // KeychainV2 updates an existing item instead of adding one; it is only
    // reached here when the item was not found.
    OSStatus status = keychain.AddGenericPassword(
        kServiceName, kAccountName, base::as_byte_span(encoded));
    if (status != noErr) {
      OSSTATUS_LOG(ERROR, status) << "fw1: could not add the keychain item";
      Cleanse(encoded);
      return Unavailable(Fw1KeyResult::kAddFailed);
    }
    created = true;
  } else {
    // Denied by the user, locked keychain, etc. Not fatal: `v10` is used.
    OSSTATUS_LOG(WARNING, found.error())
        << "fw1: could not read the keychain item";
    return Unavailable(Fw1KeyResult::kFindFailed);
  }

  std::optional<std::vector<uint8_t>> raw = base::Base64Decode(encoded);
  Cleanse(encoded);
  if (!raw || raw->size() != Encryptor::Key::kAES256GCMKeySize) {
    // Never build a Key with another length: Encryptor::Key CHECKs it. The
    // item is left alone; replacing it would lose whatever it encrypted.
    LOG(ERROR) << "fw1: keychain item is not a 256-bit Base64 key";
    if (raw) {
      Cleanse(*raw);
    }
    return Unavailable(Fw1KeyResult::kBadItem);
  }

  RecordResult(created ? Fw1KeyResult::kCreated : Fw1KeyResult::kFound);
  Encryptor::Key key(*raw, mojom::Algorithm::kAES256GCM);
  Cleanse(*raw);
  return key;
}

void Fw1KeyProvider::GetKey(KeyCallback callback) {
  // The keychain may block (and show a dialog): keep it off the UI thread.
  base::ThreadPool::PostTaskAndReplyWithResult(
      FROM_HERE, {base::MayBlock(), base::TaskPriority::USER_BLOCKING},
      base::BindOnce(&GetKeyOnThreadPool, keychain_for_testing_.get()),
      base::BindOnce(std::move(callback), std::string(kKeyTag)));
}

bool Fw1KeyProvider::UseForEncryption() {
  return true;
}

}  // namespace os_crypt_async

/* Copyright (c) 2026 lamosquita. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "brave/components/flyweb_os_crypt/fw1_key_provider_mac.h"

#include <Security/Security.h>

#include <string>
#include <utility>
#include <vector>

#include "base/apple/osstatus_logging.h"
#include "base/base64.h"
#include "base/command_line.h"
#include "base/functional/bind.h"
#include "base/logging.h"
#include "base/metrics/histogram_functions.h"
#include "base/task/thread_pool.h"
#include "components/os_crypt/async/common/algorithm.mojom.h"
#include "components/os_crypt/sync/os_crypt_switches.h"
#include "crypto/apple_keychain.h"
#include "crypto/random.h"
#include "third_party/boringssl/src/include/openssl/mem.h"

namespace os_crypt_async {

namespace {

// Same service as the `v10` item (chromium_src keychain_password_mac.mm), so
// both show up together in Keychain Access; a different account.
constexpr char kServiceName[] = "FlyWeb Safe Storage";
constexpr char kAccountName[] = "FlyWeb fw1";

// Recorded once per startup, to see how often `fw1` falls back to `v10`.
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

std::optional<Encryptor::Key> GetKeyOnThreadPool(
    std::unique_ptr<crypto::AppleKeychain> keychain) {
  if (!keychain) {
    // Same switch OSCrypt sync honours; tests must not touch the real
    // keychain. No `fw1` key then: the Encryptor keeps writing `v10`.
    if (base::CommandLine::ForCurrentProcess()->HasSwitch(
            os_crypt::switches::kUseMockKeychain)) {
      RecordResult(Fw1KeyResult::kMockKeychain);
      return std::nullopt;
    }
    keychain = std::make_unique<crypto::AppleKeychain>();
  }
  return Fw1KeyProvider::GetOrCreateKey(*keychain);
}

}  // namespace

Fw1KeyProvider::Fw1KeyProvider() = default;

Fw1KeyProvider::Fw1KeyProvider(std::unique_ptr<crypto::AppleKeychain> keychain)
    : keychain_for_testing_(std::move(keychain)) {}

Fw1KeyProvider::~Fw1KeyProvider() = default;

// static
std::optional<Encryptor::Key> Fw1KeyProvider::GetOrCreateKey(
    const crypto::AppleKeychain& keychain) {
  UInt32 length = 0;
  void* data = nullptr;
  OSStatus status = keychain.FindGenericPassword(
      strlen(kServiceName), kServiceName, strlen(kAccountName), kAccountName,
      &length, &data, nullptr);

  std::string encoded;
  bool created = false;
  if (status == noErr) {
    encoded.assign(static_cast<const char*>(data), length);
    OPENSSL_cleanse(data, length);
    keychain.ItemFreeContent(data);
  } else if (status == errSecItemNotFound) {
    std::vector<uint8_t> raw =
        crypto::RandBytesAsVector(Encryptor::Key::kAES256GCMKeySize);
    encoded = base::Base64Encode(raw);
    Cleanse(raw);
    status = keychain.AddGenericPassword(
        strlen(kServiceName), kServiceName, strlen(kAccountName), kAccountName,
        encoded.size(), encoded.data(), nullptr);
    if (status != noErr) {
      OSSTATUS_LOG(ERROR, status) << "fw1: could not add the keychain item";
      Cleanse(encoded);
      RecordResult(Fw1KeyResult::kAddFailed);
      return std::nullopt;
    }
    created = true;
  } else {
    // Denied by the user, locked keychain, etc. Not fatal: `v10` is used.
    OSSTATUS_LOG(WARNING, status) << "fw1: could not read the keychain item";
    RecordResult(Fw1KeyResult::kFindFailed);
    return std::nullopt;
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
    RecordResult(Fw1KeyResult::kBadItem);
    return std::nullopt;
  }

  RecordResult(created ? Fw1KeyResult::kCreated : Fw1KeyResult::kFound);
  std::optional<Encryptor::Key> key(
      std::in_place, *raw, mojom::Algorithm::kAES256GCM);
  Cleanse(*raw);
  return key;
}

void Fw1KeyProvider::GetKey(KeyCallback callback) {
  // The keychain may block (and show a dialog): keep it off the UI thread.
  base::ThreadPool::PostTaskAndReplyWithResult(
      FROM_HERE, {base::MayBlock(), base::TaskPriority::USER_BLOCKING},
      base::BindOnce(&GetKeyOnThreadPool, std::move(keychain_for_testing_)),
      base::BindOnce(
          [](KeyCallback callback, std::optional<Encryptor::Key> key) {
            std::move(callback).Run(kKeyTag, std::move(key));
          },
          std::move(callback)));
}

bool Fw1KeyProvider::UseForEncryption() {
  return true;
}

bool Fw1KeyProvider::IsCompatibleWithOsCryptSync() {
  return false;
}

}  // namespace os_crypt_async

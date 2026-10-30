/* Copyright (c) 2026 lamosquita. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "brave/components/flyweb_os_crypt/fw1_key_provider_mac.h"

#include <Security/Security.h>

#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "base/base64.h"
#include "base/memory/raw_ptr.h"
#include "base/test/task_environment.h"
#include "base/test/test_future.h"
#include "components/os_crypt/async/browser/os_crypt_async.h"
#include "components/os_crypt/async/common/encryptor.h"
#include "components/os_crypt/sync/os_crypt.h"
#include "components/os_crypt/sync/os_crypt_mocker.h"
#include "crypto/apple_keychain.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace os_crypt_async {

namespace {

using Items = std::map<std::string, std::string>;

// In-memory keychain: generic passwords keyed by "service/account".
class FakeKeychain : public crypto::AppleKeychain {
 public:
  explicit FakeKeychain(Items* items) : items_(items) {}
  ~FakeKeychain() override = default;

  void set_find_status(OSStatus status) { find_status_ = status; }
  void set_add_status(OSStatus status) { add_status_ = status; }

  OSStatus FindGenericPassword(UInt32 service_name_length,
                               const char* service_name,
                               UInt32 account_name_length,
                               const char* account_name,
                               UInt32* password_length,
                               void** password_data,
                               AppleSecKeychainItemRef* item) const override {
    if (find_status_ != noErr) {
      return find_status_;
    }
    auto it = items_->find(Key(service_name, service_name_length, account_name,
                               account_name_length));
    if (it == items_->end()) {
      return errSecItemNotFound;
    }
    *password_length = it->second.size();
    *password_data = const_cast<char*>(it->second.data());
    return noErr;
  }

  OSStatus ItemFreeContent(void* data) const override { return noErr; }

  OSStatus AddGenericPassword(UInt32 service_name_length,
                              const char* service_name,
                              UInt32 account_name_length,
                              const char* account_name,
                              UInt32 password_length,
                              const void* password_data,
                              AppleSecKeychainItemRef* item) const override {
    if (add_status_ != noErr) {
      return add_status_;
    }
    (*items_)[Key(service_name, service_name_length, account_name,
                  account_name_length)] =
        std::string(static_cast<const char*>(password_data), password_length);
    return noErr;
  }

  static std::string Key(const char* service,
                         UInt32 service_length,
                         const char* account,
                         UInt32 account_length) {
    return std::string(service, service_length) + "/" +
           std::string(account, account_length);
  }

 private:
  raw_ptr<Items> items_;
  OSStatus find_status_ = noErr;
  OSStatus add_status_ = noErr;
};

constexpr char kItem[] = "FlyWeb Safe Storage/FlyWeb fw1";

class Fw1KeyProviderTest : public ::testing::Test {
 protected:
  void SetUp() override { OSCryptMocker::SetUp(); }
  void TearDown() override { OSCryptMocker::TearDown(); }

  std::unique_ptr<OSCryptAsync> MakeOSCryptAsync(
      std::unique_ptr<FakeKeychain> keychain) {
    std::vector<std::pair<size_t, std::unique_ptr<KeyProvider>>> providers;
    providers.emplace_back(
        Fw1KeyProvider::kPrecedence,
        std::make_unique<Fw1KeyProvider>(std::move(keychain)));
    return std::make_unique<OSCryptAsync>(std::move(providers));
  }

  Encryptor GetEncryptor(OSCryptAsync& os_crypt_async) {
    base::test::TestFuture<Encryptor, bool> future;
    auto sub = os_crypt_async.GetInstance(future.GetCallback());
    return std::get<0>(future.Take());
  }

  base::test::TaskEnvironment task_environment_;
  Items items_;
};

TEST_F(Fw1KeyProviderTest, CreatesKeyAndWritesFw1) {
  auto os_crypt = MakeOSCryptAsync(std::make_unique<FakeKeychain>(&items_));
  Encryptor encryptor = GetEncryptor(*os_crypt);

  ASSERT_EQ(1u, items_.count(kItem));
  auto raw = base::Base64Decode(items_[kItem]);
  ASSERT_TRUE(raw);
  EXPECT_EQ(Encryptor::Key::kAES256GCMKeySize, raw->size());

  std::string ciphertext;
  ASSERT_TRUE(encryptor.EncryptString("secret", &ciphertext));
  EXPECT_EQ(0u, ciphertext.find("fw1"));
  std::string plaintext;
  ASSERT_TRUE(encryptor.DecryptString(ciphertext, &plaintext));
  EXPECT_EQ("secret", plaintext);
}

TEST_F(Fw1KeyProviderTest, ReusesKeyAcrossRestarts) {
  std::string ciphertext;
  {
    auto os_crypt = MakeOSCryptAsync(std::make_unique<FakeKeychain>(&items_));
    ASSERT_TRUE(GetEncryptor(*os_crypt).EncryptString("secret", &ciphertext));
  }
  const std::string stored = items_[kItem];
  auto os_crypt = MakeOSCryptAsync(std::make_unique<FakeKeychain>(&items_));
  std::string plaintext;
  ASSERT_TRUE(GetEncryptor(*os_crypt).DecryptString(ciphertext, &plaintext));
  EXPECT_EQ("secret", plaintext);
  EXPECT_EQ(stored, items_[kItem]);
}

// Migration: `v10` data written by OSCrypt sync stays readable; new data
// is `fw1`.
TEST_F(Fw1KeyProviderTest, ReadsV10) {
  std::string v10;
  ASSERT_TRUE(OSCrypt::EncryptString("old", &v10));
  ASSERT_EQ(0u, v10.find("v10"));

  auto os_crypt = MakeOSCryptAsync(std::make_unique<FakeKeychain>(&items_));
  Encryptor encryptor = GetEncryptor(*os_crypt);
  std::string plaintext;
  ASSERT_TRUE(encryptor.DecryptString(v10, &plaintext));
  EXPECT_EQ("old", plaintext);

  std::string rewritten;
  ASSERT_TRUE(encryptor.EncryptString(plaintext, &rewritten));
  EXPECT_EQ(0u, rewritten.find("fw1"));
}

// A corrupt item (wrong length or not Base64) must not crash and must not
// be replaced: the Encryptor falls back to `v10`.
TEST_F(Fw1KeyProviderTest, BadItemFallsBackToV10) {
  for (const std::string& bad :
       {base::Base64Encode(std::string(16, 'x')), std::string("my_password"),
        base::Base64Encode(std::string(33, 'x'))}) {
    items_[kItem] = bad;
    auto os_crypt = MakeOSCryptAsync(std::make_unique<FakeKeychain>(&items_));
    std::string ciphertext;
    ASSERT_TRUE(GetEncryptor(*os_crypt).EncryptString("secret", &ciphertext));
    EXPECT_EQ(0u, ciphertext.find("v10"));
    EXPECT_EQ(bad, items_[kItem]);
  }
}

TEST_F(Fw1KeyProviderTest, DeniedKeychainFallsBackToV10) {
  auto keychain = std::make_unique<FakeKeychain>(&items_);
  keychain->set_find_status(errSecAuthFailed);
  auto os_crypt = MakeOSCryptAsync(std::move(keychain));
  std::string ciphertext;
  ASSERT_TRUE(GetEncryptor(*os_crypt).EncryptString("secret", &ciphertext));
  EXPECT_EQ(0u, ciphertext.find("v10"));
  EXPECT_TRUE(items_.empty());
}

TEST_F(Fw1KeyProviderTest, AddFailureFallsBackToV10) {
  auto keychain = std::make_unique<FakeKeychain>(&items_);
  keychain->set_add_status(errSecWrPerm);
  auto os_crypt = MakeOSCryptAsync(std::move(keychain));
  std::string ciphertext;
  ASSERT_TRUE(GetEncryptor(*os_crypt).EncryptString("secret", &ciphertext));
  EXPECT_EQ(0u, ciphertext.find("v10"));
}

// Without its key, `fw1` data must fail to decrypt. OSCrypt sync would hand
// it back as "cleartext" (no `v10` prefix), corrupting cookies or passwords.
TEST_F(Fw1KeyProviderTest, Fw1WithoutKeyFailsToDecrypt) {
  std::string ciphertext;
  {
    auto os_crypt = MakeOSCryptAsync(std::make_unique<FakeKeychain>(&items_));
    ASSERT_TRUE(GetEncryptor(*os_crypt).EncryptString("secret", &ciphertext));
  }
  auto keychain = std::make_unique<FakeKeychain>(&items_);
  keychain->set_find_status(errSecAuthFailed);
  auto os_crypt = MakeOSCryptAsync(std::move(keychain));
  std::string plaintext;
  EXPECT_FALSE(GetEncryptor(*os_crypt).DecryptString(ciphertext, &plaintext));
}

// A different key in the item (e.g. the user deleted and recreated it):
// GCM authentication fails, nothing is returned.
TEST_F(Fw1KeyProviderTest, Fw1WithOtherKeyFailsToDecrypt) {
  std::string ciphertext;
  {
    auto os_crypt = MakeOSCryptAsync(std::make_unique<FakeKeychain>(&items_));
    ASSERT_TRUE(GetEncryptor(*os_crypt).EncryptString("secret", &ciphertext));
  }
  items_.clear();
  auto os_crypt = MakeOSCryptAsync(std::make_unique<FakeKeychain>(&items_));
  std::string plaintext;
  EXPECT_FALSE(GetEncryptor(*os_crypt).DecryptString(ciphertext, &plaintext));
}

}  // namespace

}  // namespace os_crypt_async

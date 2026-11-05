/* Copyright (c) 2026 lamosquita. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "brave/components/flyweb_os_crypt/fw1_key_provider_mac.h"

#include <Security/Security.h>

#include <map>
#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "base/base64.h"
#include "base/containers/span.h"
#include "base/memory/raw_ptr.h"
#include "base/memory/scoped_refptr.h"
#include "base/test/task_environment.h"
#include "base/test/test_future.h"
#include "base/types/expected.h"
#include "components/os_crypt/async/browser/keychain_key_provider.h"
#include "components/os_crypt/async/browser/os_crypt_async.h"
#include "components/os_crypt/async/common/encryptor.h"
#include "crypto/aead.h"
#include "crypto/apple/keychain_v2.h"
#include "crypto/apple/scoped_fake_keychain_v2.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace os_crypt_async {

namespace {

using Items = std::map<std::string, std::string>;

// In-memory generic passwords keyed by "service/account". Only the two calls
// Fw1KeyProvider makes; upstream's FakeKeychainV2 returns a fixed password
// for any item, which cannot tell `fw1` from `v10`.
class FakeKeychain : public crypto::apple::KeychainV2 {
 public:
  explicit FakeKeychain(Items* items) : items_(items) {}
  ~FakeKeychain() override = default;

  void set_find_status(OSStatus status) { find_status_ = status; }
  void set_add_status(OSStatus status) { add_status_ = status; }
  int add_calls() const { return add_calls_; }

  base::expected<std::vector<uint8_t>, OSStatus> FindGenericPassword(
      std::string_view service_name,
      std::string_view account_name) override {
    if (find_status_ != noErr) {
      return base::unexpected(find_status_);
    }
    auto it = items_->find(Key(service_name, account_name));
    if (it == items_->end()) {
      return base::unexpected(errSecItemNotFound);
    }
    return std::vector<uint8_t>(it->second.begin(), it->second.end());
  }

  OSStatus AddGenericPassword(std::string_view service_name,
                              std::string_view account_name,
                              base::span<const uint8_t> password) override {
    ++add_calls_;
    if (add_status_ != noErr) {
      return add_status_;
    }
    (*items_)[Key(service_name, account_name)] =
        std::string(password.begin(), password.end());
    return noErr;
  }

  static std::string Key(std::string_view service, std::string_view account) {
    return std::string(service) + "/" + std::string(account);
  }

 private:
  raw_ptr<Items> items_;
  OSStatus find_status_ = noErr;
  OSStatus add_status_ = noErr;
  int add_calls_ = 0;
};

constexpr char kItem[] = "FlyWeb Safe Storage/FlyWeb fw1";

// A fixed 256-bit key, as FlyWeb 2.5-2.7 stores it (Base64 in the item).
std::vector<uint8_t> GoldenKey() {
  std::vector<uint8_t> key(32);
  for (size_t i = 0; i < key.size(); ++i) {
    key[i] = static_cast<uint8_t>(i * 7 + 1);
  }
  return key;
}

using KeyResult = base::expected<Encryptor::Key, KeyProvider::KeyError>;

class Fw1KeyProviderTest : public ::testing::Test {
 protected:
  KeyResult GetKey(FakeKeychain& keychain) {
    Fw1KeyProvider provider(&keychain);
    base::test::TestFuture<const std::string&, KeyResult> future;
    static_cast<KeyProvider&>(provider).GetKey(future.GetCallback());
    auto [tag, key] = future.Take();
    EXPECT_EQ("fw1", tag);
    return std::move(key);
  }

  // `fw1` next to upstream's `v10` provider, as browser_process_impl.cc
  // registers them. `v10` reads the process-wide fake keychain.
  scoped_refptr<Encryptor> GetEncryptor(FakeKeychain* fw1_keychain) {
    std::vector<std::pair<size_t, std::unique_ptr<KeyProvider>>> providers;
    providers.emplace_back(10u, std::make_unique<KeychainKeyProvider>());
    if (fw1_keychain) {
      providers.emplace_back(Fw1KeyProvider::kPrecedence,
                             std::make_unique<Fw1KeyProvider>(fw1_keychain));
    }
    OSCryptAsync os_crypt_async(std::move(providers));
    base::test::TestFuture<scoped_refptr<Encryptor>> future;
    os_crypt_async.GetInstance(future.GetCallback());
    return future.Take();
  }

  base::test::TaskEnvironment task_environment_{
      base::test::TaskEnvironment::MainThreadType::UI,
      base::test::TaskEnvironment::ThreadPoolExecutionMode::ASYNC};
  crypto::apple::ScopedFakeKeychainV2 v10_keychain_{"test-access-group"};
  Items items_;
};

TEST_F(Fw1KeyProviderTest, CreatesKeyOnFirstUse) {
  FakeKeychain keychain(&items_);
  ASSERT_TRUE(GetKey(keychain).has_value());
  ASSERT_EQ(1u, items_.count(kItem));
  auto raw = base::Base64Decode(items_[kItem]);
  ASSERT_TRUE(raw);
  EXPECT_EQ(Encryptor::Key::kAES256GCMKeySize, raw->size());
}

TEST_F(Fw1KeyProviderTest, ReusesKeyAndNeverReplacesIt) {
  items_[kItem] = base::Base64Encode(GoldenKey());
  const std::string stored = items_[kItem];
  FakeKeychain keychain(&items_);
  ASSERT_TRUE(GetKey(keychain).has_value());
  EXPECT_EQ(0, keychain.add_calls());
  EXPECT_EQ(stored, items_[kItem]);
}

// Denied, locked or unreadable keychain: temporary, so nothing gets deleted.
TEST_F(Fw1KeyProviderTest, FindFailureIsTemporary) {
  for (OSStatus status : {errSecAuthFailed, errSecInteractionNotAllowed,
                          errSecNotAvailable}) {
    FakeKeychain keychain(&items_);
    keychain.set_find_status(status);
    KeyResult key = GetKey(keychain);
    ASSERT_FALSE(key.has_value());
    EXPECT_EQ(KeyProvider::KeyError::kTemporarilyUnavailable, key.error());
    EXPECT_EQ(0, keychain.add_calls());
    EXPECT_TRUE(items_.empty());
  }
}

TEST_F(Fw1KeyProviderTest, AddFailureIsTemporary) {
  FakeKeychain keychain(&items_);
  keychain.set_add_status(errSecWrPerm);
  KeyResult key = GetKey(keychain);
  ASSERT_FALSE(key.has_value());
  EXPECT_EQ(KeyProvider::KeyError::kTemporarilyUnavailable, key.error());
}

// A corrupt item (wrong length or not Base64) must not crash and must not
// be replaced.
TEST_F(Fw1KeyProviderTest, BadItemIsTemporaryAndKept) {
  for (const std::string& bad :
       {base::Base64Encode(std::string(16, 'x')), std::string("my_password"),
        base::Base64Encode(std::string(33, 'x'))}) {
    items_[kItem] = bad;
    FakeKeychain keychain(&items_);
    KeyResult key = GetKey(keychain);
    ASSERT_FALSE(key.has_value());
    EXPECT_EQ(KeyProvider::KeyError::kTemporarilyUnavailable, key.error());
    EXPECT_EQ(0, keychain.add_calls());
    EXPECT_EQ(bad, items_[kItem]);
  }
}

// Format check against 2.x: "fw1" + 12-byte nonce + AES-256-GCM(ciphertext,
// tag) with the raw key from the item. Both directions.
TEST_F(Fw1KeyProviderTest, SameFormatAs2x) {
  items_[kItem] = base::Base64Encode(GoldenKey());
  FakeKeychain keychain(&items_);
  scoped_refptr<Encryptor> encryptor = GetEncryptor(&keychain);

  std::string ciphertext;
  ASSERT_TRUE(encryptor->EncryptString("secret", &ciphertext));
  ASSERT_EQ(0u, ciphertext.find("fw1"));
  auto data = base::as_byte_span(ciphertext).subspan(3u);
  auto opened = crypto::aead::Open(crypto::aead::AES_256_GCM, GoldenKey(),
                                   data.subspan(12u), data.first(12u),
                                   /*associated_data=*/{});
  ASSERT_TRUE(opened);
  EXPECT_EQ("secret", std::string(opened->begin(), opened->end()));

  const std::vector<uint8_t> nonce(12, 0x42);
  std::vector<uint8_t> sealed =
      crypto::aead::Seal(crypto::aead::AES_256_GCM, GoldenKey(),
                         base::byte_span_from_cstring("old"), nonce,
                         /*associated_data=*/{});
  std::string written_by_2x = "fw1" + std::string(nonce.begin(), nonce.end()) +
                              std::string(sealed.begin(), sealed.end());
  Encryptor::DecryptFlags flags;
  std::string plaintext;
  ASSERT_TRUE(encryptor->DecryptString(written_by_2x, &plaintext, &flags));
  EXPECT_EQ("old", plaintext);
  EXPECT_FALSE(flags.should_reencrypt);
}

// Migration: `v10` data stays readable and is marked for `fw1`.
TEST_F(Fw1KeyProviderTest, ReadsV10AndEncryptsFw1) {
  std::string v10;
  ASSERT_TRUE(GetEncryptor(nullptr)->EncryptString("old", &v10));
  ASSERT_EQ(0u, v10.find("v10"));

  FakeKeychain keychain(&items_);
  scoped_refptr<Encryptor> encryptor = GetEncryptor(&keychain);
  Encryptor::DecryptFlags flags;
  std::string plaintext;
  ASSERT_TRUE(encryptor->DecryptString(v10, &plaintext, &flags));
  EXPECT_EQ("old", plaintext);
  EXPECT_TRUE(flags.should_reencrypt);

  std::string rewritten;
  ASSERT_TRUE(encryptor->EncryptString(plaintext, &rewritten));
  EXPECT_EQ(0u, rewritten.find("fw1"));
}

// Without its key, `fw1` data fails as temporarily unavailable (never handed
// back as cleartext) and new data is `v10`, as in 2.x.
TEST_F(Fw1KeyProviderTest, Fw1KeychainDenied) {
  std::string fw1;
  {
    FakeKeychain keychain(&items_);
    ASSERT_TRUE(GetEncryptor(&keychain)->EncryptString("secret", &fw1));
  }
  FakeKeychain keychain(&items_);
  keychain.set_find_status(errSecAuthFailed);
  scoped_refptr<Encryptor> encryptor = GetEncryptor(&keychain);

  Encryptor::DecryptFlags flags;
  std::string plaintext;
  EXPECT_FALSE(encryptor->DecryptString(fw1, &plaintext, &flags));
  EXPECT_TRUE(flags.temporarily_unavailable);
  EXPECT_TRUE(plaintext.empty());

  std::string ciphertext;
  ASSERT_TRUE(encryptor->EncryptString("new", &ciphertext));
  EXPECT_EQ(0u, ciphertext.find("v10"));
}

// `v10` keychain denied: `v10` data is temporarily unavailable, `fw1` works.
TEST_F(Fw1KeyProviderTest, V10KeychainDenied) {
  std::string v10;
  ASSERT_TRUE(GetEncryptor(nullptr)->EncryptString("old", &v10));
  v10_keychain_.SetFindGenericResult(errSecAuthFailed);

  FakeKeychain keychain(&items_);
  scoped_refptr<Encryptor> encryptor = GetEncryptor(&keychain);
  Encryptor::DecryptFlags flags;
  std::string plaintext;
  EXPECT_FALSE(encryptor->DecryptString(v10, &plaintext, &flags));
  EXPECT_TRUE(flags.temporarily_unavailable);

  std::string ciphertext;
  ASSERT_TRUE(encryptor->EncryptString("new", &ciphertext));
  EXPECT_EQ(0u, ciphertext.find("fw1"));
}

// A different key in the item (e.g. the user deleted it and it was
// recreated): GCM authentication fails, permanently, nothing is returned.
TEST_F(Fw1KeyProviderTest, Fw1WithOtherKeyFails) {
  std::string fw1;
  {
    FakeKeychain keychain(&items_);
    ASSERT_TRUE(GetEncryptor(&keychain)->EncryptString("secret", &fw1));
  }
  items_.clear();
  FakeKeychain keychain(&items_);
  Encryptor::DecryptFlags flags;
  std::string plaintext;
  EXPECT_FALSE(GetEncryptor(&keychain)->DecryptString(fw1, &plaintext, &flags));
  EXPECT_FALSE(flags.temporarily_unavailable);
  EXPECT_TRUE(plaintext.empty());
}

// Data without a prefix (what OSCrypt in 2.x handed back as is) comes back
// in clear and marked for re-encryption; empty stays empty. See the FlyWeb
// part of chromium_src/components/os_crypt/async/common/encryptor.cc.
TEST_F(Fw1KeyProviderTest, UnprefixedAndEmpty) {
  FakeKeychain keychain(&items_);
  scoped_refptr<Encryptor> encryptor = GetEncryptor(&keychain);
  Encryptor::DecryptFlags flags;
  std::string plaintext;
  ASSERT_TRUE(encryptor->DecryptString("legacy", &plaintext, &flags));
  EXPECT_EQ("legacy", plaintext);
  EXPECT_TRUE(flags.should_reencrypt);

  ASSERT_TRUE(encryptor->DecryptString(std::string(), &plaintext, &flags));
  EXPECT_TRUE(plaintext.empty());
  EXPECT_FALSE(flags.should_reencrypt);
}

}  // namespace

}  // namespace os_crypt_async

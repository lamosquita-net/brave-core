/* Copyright (c) 2026 lamosquita. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

// FlyWeb part of chromium_src/components/os_crypt/async/common/encryptor.cc
// (Mac and Linux): what Encryptor::DecryptData returns for data written by
// FlyWeb 2.x, whose OSCrypt handed unprefixed data back as is.

#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "base/memory/scoped_refptr.h"
#include "base/test/task_environment.h"
#include "base/test/test_future.h"
#include "base/types/expected.h"
#include "build/build_config.h"
#include "components/os_crypt/async/browser/key_provider.h"
#include "components/os_crypt/async/browser/os_crypt_async.h"
#include "components/os_crypt/async/common/algorithm.mojom.h"
#include "components/os_crypt/async/common/encryptor.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace os_crypt_async {

namespace {

// Hands out a fixed AES-256-GCM key derived from `seed`, or an error.
class FixedKeyProvider : public KeyProvider {
 public:
  FixedKeyProvider(std::string tag,
                   uint8_t seed,
                   std::optional<KeyError> error = std::nullopt)
      : tag_(std::move(tag)), seed_(seed), error_(error) {}

  void GetKey(KeyCallback callback) override {
    if (error_) {
      std::move(callback).Run(tag_, base::unexpected(*error_));
      return;
    }
    std::vector<uint8_t> key(Encryptor::Key::kAES256GCMKeySize, seed_);
    std::move(callback).Run(tag_,
                            Encryptor::Key(key, mojom::Algorithm::kAES256GCM));
  }
  bool UseForEncryption() override { return true; }

 private:
  const std::string tag_;
  const uint8_t seed_;
  const std::optional<KeyError> error_;
};

struct ProviderSpec {
  const char* tag;
  uint8_t seed;
  std::optional<KeyProvider::KeyError> error = std::nullopt;
};

class FlyWebEncryptorTest : public ::testing::Test {
 protected:
  // Providers in increasing precedence: the last one that loads encrypts.
  scoped_refptr<Encryptor> GetEncryptor(std::vector<ProviderSpec> specs) {
    std::vector<std::pair<size_t, std::unique_ptr<KeyProvider>>> providers;
    size_t precedence = 1u;
    for (const auto& spec : specs) {
      providers.emplace_back(
          precedence++,
          std::make_unique<FixedKeyProvider>(spec.tag, spec.seed, spec.error));
    }
    OSCryptAsync os_crypt_async(std::move(providers));
    base::test::TestFuture<scoped_refptr<Encryptor>> future;
    os_crypt_async.GetInstance(future.GetCallback());
    return future.Take();
  }

  static std::string Encrypt(const Encryptor& encryptor,
                             const std::string& plaintext) {
    std::string ciphertext;
    EXPECT_TRUE(encryptor.EncryptString(plaintext, &ciphertext));
    return ciphertext;
  }

  base::test::TaskEnvironment task_environment_;
};

#if BUILDFLAG(IS_MAC) || BUILDFLAG(IS_LINUX)

// (c) Unprefixed data: cleartext from before encryption. Returned as is and
// marked for re-encryption, as OSCrypt did in 2.x.
TEST_F(FlyWebEncryptorTest, UnprefixedIsCleartextToReencrypt) {
  auto encryptor = GetEncryptor({{"v10", 1}, {"fw1", 2}});
  for (const std::string& legacy :
       {std::string("a"), std::string("session=abc"), std::string("v1"),
        std::string("fw"), std::string("V10abc"), std::string("\x01\x02", 2)}) {
    Encryptor::DecryptFlags flags;
    std::string plaintext;
    ASSERT_TRUE(encryptor->DecryptString(legacy, &plaintext, &flags)) << legacy;
    EXPECT_EQ(legacy, plaintext);
    EXPECT_TRUE(flags.should_reencrypt);
    EXPECT_FALSE(flags.temporarily_unavailable);
  }
}

// Same when the only real provider is temporarily unavailable.
TEST_F(FlyWebEncryptorTest, UnprefixedWithKeyUnavailable) {
  auto encryptor = GetEncryptor(
      {{"v10", 1, KeyProvider::KeyError::kTemporarilyUnavailable}});
  Encryptor::DecryptFlags flags;
  std::string plaintext;
  ASSERT_TRUE(encryptor->DecryptString("legacy", &plaintext, &flags));
  EXPECT_EQ("legacy", plaintext);
  EXPECT_TRUE(flags.should_reencrypt);
}

// A provider tag whose provider is not loaded at all (e.g. Linux `v11` with
// no secret service, a future `fw2` after a downgrade) is not cleartext.
TEST_F(FlyWebEncryptorTest, UnknownProviderTagFails) {
  auto encryptor = GetEncryptor({{"v10", 1}});
  for (const std::string& tagged :
       {std::string("v11abcdefghijklmnopqrstuvwxyz"), std::string("fw1xxxx"),
        std::string("fw2xxxx"), std::string("v20xxxx"), std::string("v99")}) {
    Encryptor::DecryptFlags flags;
    std::string plaintext;
    EXPECT_FALSE(encryptor->DecryptString(tagged, &plaintext, &flags))
        << tagged;
    EXPECT_TRUE(plaintext.empty());
    EXPECT_FALSE(flags.should_reencrypt);
  }
}

// Upstream's test keyrings (no real provider tag) keep upstream's nullopt.
TEST_F(FlyWebEncryptorTest, OnlyWithRealProviders) {
  auto encryptor = GetEncryptor({{"k1", 1}});
  std::string plaintext;
  EXPECT_FALSE(encryptor->DecryptString("legacy", &plaintext));
}

#endif  // BUILDFLAG(IS_MAC) || BUILDFLAG(IS_LINUX)

// (d) Empty stays empty, not marked.
TEST_F(FlyWebEncryptorTest, EmptyIsEmpty) {
  auto encryptor = GetEncryptor({{"v10", 1}, {"fw1", 2}});
  Encryptor::DecryptFlags flags;
  std::string plaintext = "x";
  ASSERT_TRUE(encryptor->DecryptString(std::string(), &plaintext, &flags));
  EXPECT_TRUE(plaintext.empty());
  EXPECT_FALSE(flags.should_reencrypt);
  EXPECT_FALSE(flags.temporarily_unavailable);
}

// (e) Known prefix that does not decrypt (truncated, flipped bit, other key):
// permanent error, never cleartext.
TEST_F(FlyWebEncryptorTest, CorruptFailsPermanently) {
  auto encryptor = GetEncryptor({{"v10", 1}, {"fw1", 2}});
  std::string fw1 = Encrypt(*encryptor, "secret");
  ASSERT_EQ(0u, fw1.find("fw1"));
  std::string flipped = fw1;
  flipped.back() ^= 1;
  auto other_key = GetEncryptor({{"v10", 1}, {"fw1", 3}});
  for (const auto& [enc, data] :
       std::vector<std::pair<Encryptor*, std::string>>{
           {encryptor.get(), "fw1"},
           {encryptor.get(), fw1.substr(0, 10)},
           {encryptor.get(), flipped},
           {other_key.get(), fw1}}) {
    Encryptor::DecryptFlags flags;
    std::string plaintext;
    EXPECT_FALSE(enc->DecryptString(data, &plaintext, &flags));
    EXPECT_TRUE(plaintext.empty());
    EXPECT_FALSE(flags.temporarily_unavailable);
    EXPECT_FALSE(flags.should_reencrypt);
  }
}

// Known prefix whose key is temporarily unavailable: error, flagged.
TEST_F(FlyWebEncryptorTest, KeyTemporarilyUnavailable) {
  std::string fw1 = Encrypt(*GetEncryptor({{"v10", 1}, {"fw1", 2}}), "secret");
  auto encryptor = GetEncryptor(
      {{"v10", 1}, {"fw1", 2, KeyProvider::KeyError::kTemporarilyUnavailable}});
  Encryptor::DecryptFlags flags;
  std::string plaintext;
  EXPECT_FALSE(encryptor->DecryptString(fw1, &plaintext, &flags));
  EXPECT_TRUE(flags.temporarily_unavailable);
  // New data goes to `v10` meanwhile.
  EXPECT_EQ(0u, Encrypt(*encryptor, "new").find("v10"));
}

// Round trip and migration flag with both keys.
TEST_F(FlyWebEncryptorTest, V10ReadAndMarked) {
  std::string v10 = Encrypt(*GetEncryptor({{"v10", 1}}), "old");
  auto encryptor = GetEncryptor({{"v10", 1}, {"fw1", 2}});
  Encryptor::DecryptFlags flags;
  std::string plaintext;
  ASSERT_TRUE(encryptor->DecryptString(v10, &plaintext, &flags));
  EXPECT_EQ("old", plaintext);
  EXPECT_TRUE(flags.should_reencrypt);
}

}  // namespace

}  // namespace os_crypt_async

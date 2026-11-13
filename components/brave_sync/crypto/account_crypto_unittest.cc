/* Copyright (c) 2026 The FlyWeb Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "brave/components/brave_sync/crypto/account_crypto.h"

#include <string>
#include <vector>

#include "base/check.h"
#include "base/logging.h"
#include "base/strings/string_number_conversions.h"
#include "base/strings/string_util.h"
#include "base/time/time.h"
#include "brave/components/brave_sync/crypto/crypto.h"
#include "testing/gtest/include/gtest/gtest.h"

// Vectors computed outside Chromium (Python hashlib.scrypt, HMAC-based HKDF,
// `cryptography` AES-GCM and Ed25519; script in FlyWeb/parches/3.2-sync-cuenta/
// vectores.py). The chain id is the one of brave_sync_auth_manager_unittest.cc
// for the same words, so the proof is signed with the go-sync key.

namespace brave_sync::crypto::account {

namespace {

constexpr char kSyncCode[] =
    "badge unique kiwi orient spring venue piano "
    "lake admit ill roof brother grant hour better "
    "proud cabbage fee slow economy wage final fox cancel";
constexpr char kSeedHex[] =
    "117db1edce4d33e4e903e503ae1eee8e765adc8565661faa9330a31f62ad1711";
constexpr char kChainId[] =
    "502042270C8145247ED70A18F87022A39886900AB36F2FFF655635DBE516765E";

// "  Prueba.Cuenta@Example.COM " and "café con léche 2026" written in NFD.
constexpr char kRawEmail[] = "  Prueba.Cuenta@Example.COM ";
constexpr char kEmail[] = "prueba.cuenta@example.com";
constexpr char kRawPassword[] = "cafe\xcc\x81 con le\xcc\x81" "che 2026";
constexpr char kPasswordNfcHex[] = "636166C3A920636F6E206CC3A96368652032303236";

constexpr char kKl1024[] =
    "899259831d277608bd76f88b26f2785aeb796fb538c695e35f7e0a51dbc72809";
constexpr char kKu1024[] =
    "ebc1929357ce7ef5d90ae98b7d4360a958b2a33a7cdebb978e7bdef3f9fb6ac6";
constexpr char kKl[] =
    "1b4d93a09eabe0c53c82616537838f45fcff669aa5e0679b4ded48537ac7f664";
constexpr char kKu[] =
    "e45fc1039d0f09178bb6df64782a03ad111d4cc6f7fe6eee3d8bc5812e9dbad4";
constexpr char kWrapped[] =
    "000102030405060708090a0b9f4e3b4b73fc02ec2d4ac6246bb6d250fde38d2df28eeb85"
    "5b532881f2f2ec7d97a815f6323e87132de80f9e1ca7f270";

constexpr char kProofMessage[] =
    "666c797765622d6375656e74612d76310004504f535400122f6375656e74612f76312f65"
    "76656e746f7300076576656e746f730020303132333435363738396162636465663031"
    "32333435363738396162636465660028000102030405060708090a0b0c0d0e0f1011121314"
    "15161718191a1b1c1d1e1f20212223242526278b5194d06d1404cd75cfb875f75cf9b9a3"
    "364a42827e23de580b47a50be7f0c2";
constexpr char kProofSignature[] =
    "8b7adea8e62404414a26d68c92ed472fcc07d0a8f9d9fa9fa71fa82b867e373a9002f776"
    "e4d38aea6bb878030738b2e8c919f0f8f04768a2ff9aa857cc18240a";
constexpr char kProofSignatureEmptyAccount[] =
    "c3b89a0f37c1bf2757cadca68c96aae99313635ba739d7f4610ec80b87653670d6df6f88"
    "939413f0f1a030d88abc511919bcfe32259d8ee262cd94510852fb05";

std::vector<uint8_t> FromHex(std::string_view hex) {
  std::vector<uint8_t> out;
  CHECK(base::HexStringToBytes(hex, &out));
  return out;
}

std::vector<uint8_t> Nonce40() {
  std::vector<uint8_t> n(40);
  for (size_t i = 0; i < n.size(); ++i) {
    n[i] = static_cast<uint8_t>(i);
  }
  return n;
}

std::array<uint8_t, kSeedSize> Seed() {
  std::vector<uint8_t> words_seed;
  CHECK(PassphraseToBytes32(kSyncCode, &words_seed));
  std::array<uint8_t, kSeedSize> seed;
  std::ranges::copy(words_seed, seed.begin());
  return seed;
}

}  // namespace

TEST(AccountCryptoTest, SeedFromWords) {
  EXPECT_EQ(base::HexEncode(Seed()), base::ToUpperASCII(kSeedHex));
}

TEST(AccountCryptoTest, ChainIdIsTheGoSyncClientId) {
  const ChainKeys keys = ChainKeys::FromSeed(Seed());
  EXPECT_EQ(keys.ChainId(), kChainId);

  // Same as the derivation of BraveSyncAuthManager::DeriveSigningKeys.
  const std::vector<uint8_t> salt = {
      72,  203, 156, 43,  64,  229, 225, 127, 214, 158, 50,  29,  130,
      186, 182, 207, 6,   108, 47,  254, 245, 71,  198, 109, 44,  108,
      32,  193, 221, 126, 119, 143, 112, 113, 87,  184, 239, 231, 230,
      234, 28,  135, 54,  42,  9,   243, 39,  30,  179, 147, 194, 211,
      212, 239, 225, 52,  192, 219, 145, 40,  95,  19,  142, 98};
  const std::string info_str = "sync-auth-key";
  const std::vector<uint8_t> info(info_str.begin(), info_str.end());
  const auto seed = Seed();
  std::vector<uint8_t> pub, priv;
  DeriveSigningKeysFromSeed(std::vector<uint8_t>(seed.begin(), seed.end()),
                            &salt, &info, &pub, &priv);
  EXPECT_EQ(base::HexEncode(pub), kChainId);
}

TEST(AccountCryptoTest, ProofMessageAndSignature) {
  const ChainKeys keys = ChainKeys::FromSeed(Seed());
  const auto nonce = Nonce40();
  EXPECT_EQ(base::HexEncode(BuildProofMessage(
                "POST", "/cuenta/v1/eventos", "eventos",
                "0123456789abcdef0123456789abcdef", nonce, "{\"desde\":0}")),
            base::ToUpperASCII(kProofMessage));
  const auto sig =
      keys.SignProof("POST", "/cuenta/v1/eventos", "eventos",
                     "0123456789abcdef0123456789abcdef", nonce,
                     "{\"desde\":0}");
  EXPECT_EQ(base::HexEncode(sig), base::ToUpperASCII(kProofSignature));
  const auto sig0 = keys.SignProof("POST", "/cuenta/v1/alta/confirmar", "alta",
                                   "", nonce, "{}");
  EXPECT_EQ(base::HexEncode(sig0),
            base::ToUpperASCII(kProofSignatureEmptyAccount));
  EXPECT_TRUE(Verify(BuildProofMessage("POST", "/cuenta/v1/eventos", "eventos",
                                       "0123456789abcdef0123456789abcdef",
                                       nonce, "{\"desde\":0}"),
                     std::vector<uint8_t>(sig.begin(), sig.end()),
                     std::vector<uint8_t>(keys.public_key().begin(),
                                          keys.public_key().end())));
}

TEST(AccountCryptoTest, ProofDependsOnEveryField) {
  const ChainKeys keys = ChainKeys::FromSeed(Seed());
  const auto nonce = Nonce40();
  const auto base_sig =
      keys.SignProof("POST", "/cuenta/v1/salir", "salir", "id", nonce, "{}");
  EXPECT_NE(base_sig,
            keys.SignProof("PUT", "/cuenta/v1/salir", "salir", "id", nonce,
                           "{}"));
  EXPECT_NE(base_sig, keys.SignProof("POST", "/cuenta/v1/salir/", "salir",
                                     "id", nonce, "{}"));
  EXPECT_NE(base_sig, keys.SignProof("POST", "/cuenta/v1/salir", "salir2",
                                     "id", nonce, "{}"));
  EXPECT_NE(base_sig, keys.SignProof("POST", "/cuenta/v1/salir", "salir",
                                     "id2", nonce, "{}"));
  EXPECT_NE(base_sig, keys.SignProof("POST", "/cuenta/v1/salir", "salir", "id",
                                     nonce, "{} "));
  // Length prefixes: moving a byte from one field to the next changes M.
  EXPECT_NE(BuildProofMessage("POST", "/a", "bc", "", nonce, ""),
            BuildProofMessage("POST", "/ab", "c", "", nonce, ""));
}

TEST(AccountCryptoTest, Normalization) {
  EXPECT_EQ(NormalizeEmail(kRawEmail), kEmail);
  EXPECT_EQ(NormalizeEmail("\xc2\xa0" "A@B.ES\t"), "a@b.es");
  EXPECT_FALSE(NormalizeEmail("a b@c.es").has_value());
  EXPECT_FALSE(NormalizeEmail("ab.es").has_value());
  EXPECT_FALSE(NormalizeEmail("a@@b.es").has_value());
  EXPECT_FALSE(NormalizeEmail("a@bes").has_value());
  EXPECT_FALSE(NormalizeEmail("@b.es").has_value());
  EXPECT_FALSE(NormalizeEmail("a@b.es,c@d.es").has_value());
  EXPECT_FALSE(NormalizeEmail("a\x01@b.es").has_value());
  EXPECT_FALSE(NormalizeEmail("\xff@b.es").has_value());

  const std::string password = NormalizePassword(kRawPassword);
  EXPECT_EQ(base::HexEncode(base::as_byte_span(password)), kPasswordNfcHex);
  EXPECT_EQ(CountCodePoints(password), 19u);
  EXPECT_EQ(NormalizePassword("  ascii stays  "), "  ascii stays  ");
}

TEST(AccountCryptoTest, KeysLowCost) {
  const auto keys = DeriveAccountKeysWithCostForTesting(
      NormalizePassword(kRawPassword), kEmail, 1 << 10);
  ASSERT_TRUE(keys.has_value());
  EXPECT_EQ(base::HexEncode(keys->login_key.bytes()),
            base::ToUpperASCII(kKl1024));
  EXPECT_EQ(base::HexEncode(keys->wrap_key.bytes()),
            base::ToUpperASCII(kKu1024));
}

// The real cost: 128 MiB. Also checks that max_mem is enough (§4: with the
// default 65 MiB EVP_PBE_scrypt fails with EVP_R_MEMORY_LIMIT_EXCEEDED).
TEST(AccountCryptoTest, KeysRealCost) {
  const base::TimeTicks start = base::TimeTicks::Now();
  const auto keys = DeriveAccountKeys(NormalizePassword(kRawPassword), kEmail);
  LOG(INFO) << "FlyWeb scrypt N=2^17: "
            << (base::TimeTicks::Now() - start).InMilliseconds() << " ms";
  ASSERT_TRUE(keys.has_value());
  EXPECT_EQ(base::HexEncode(keys->login_key.bytes()), base::ToUpperASCII(kKl));
  EXPECT_EQ(base::HexEncode(keys->wrap_key.bytes()), base::ToUpperASCII(kKu));
}

TEST(AccountCryptoTest, WrapAndUnwrap) {
  const std::vector<uint8_t> ku_bytes = FromHex(kKu);
  const SecretKey ku{base::span<const uint8_t, kKeySize>(ku_bytes)};
  std::array<uint8_t, kGcmNonceSize> nonce;
  for (size_t i = 0; i < nonce.size(); ++i) {
    nonce[i] = static_cast<uint8_t>(i);
  }
  const auto seed = Seed();
  const auto wrapped = WrapSeedWithNonceForTesting(ku, seed, kEmail, nonce);
  EXPECT_EQ(base::HexEncode(wrapped), base::ToUpperASCII(kWrapped));
  ASSERT_EQ(wrapped.size(), kWrappedSeedSize);

  auto opened = UnwrapSeed(ku, wrapped, kEmail);
  ASSERT_TRUE(opened.has_value());
  EXPECT_TRUE(std::ranges::equal(opened->bytes(), seed));

  // Random nonce: two wraps differ and both open.
  const auto w1 = WrapSeed(ku, seed, kEmail);
  const auto w2 = WrapSeed(ku, seed, kEmail);
  EXPECT_NE(w1, w2);
  EXPECT_TRUE(UnwrapSeed(ku, w1, kEmail).has_value());

  // P15: a changed wrap, another email, another key or another size: nothing.
  auto bad = wrapped;
  bad[20] ^= 1;
  EXPECT_FALSE(UnwrapSeed(ku, bad, kEmail).has_value());
  EXPECT_FALSE(UnwrapSeed(ku, wrapped, "otra@example.com").has_value());
  const std::vector<uint8_t> kl_bytes = FromHex(kKl);
  const SecretKey kl{base::span<const uint8_t, kKeySize>(kl_bytes)};
  EXPECT_FALSE(UnwrapSeed(kl, wrapped, kEmail).has_value());
  EXPECT_FALSE(
      UnwrapSeed(ku, base::span(wrapped).first(kWrappedSeedSize - 1), kEmail)
          .has_value());
}

TEST(AccountCryptoTest, SecretKeyWipesOnMove) {
  std::array<uint8_t, kKeySize> raw;
  raw.fill(0xab);
  SecretKey a(raw);
  SecretKey b(std::move(a));
  EXPECT_TRUE(std::ranges::all_of(a.bytes(), [](uint8_t c) { return c == 0; }));
  EXPECT_TRUE(
      std::ranges::all_of(b.bytes(), [](uint8_t c) { return c == 0xab; }));
  std::string s = "secreto";
  WipeString(s);
  EXPECT_TRUE(s.empty());
}

}  // namespace brave_sync::crypto::account

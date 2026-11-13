/* Copyright (c) 2026 The FlyWeb Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "brave/components/brave_sync/crypto/account_crypto.h"

#include <algorithm>
#include <utility>

#include "base/check.h"
#include "base/check_op.h"
#include "base/containers/span.h"
#include "base/strings/string_number_conversions.h"
#include "base/strings/string_util.h"
#include "brave/components/brave_sync/crypto/crypto.h"
#include "crypto/random.h"
#include "third_party/boringssl/src/include/openssl/aead.h"
#include "third_party/boringssl/src/include/openssl/curve25519.h"
#include "third_party/boringssl/src/include/openssl/digest.h"
#include "third_party/boringssl/src/include/openssl/evp.h"
#include "third_party/boringssl/src/include/openssl/hkdf.h"
#include "third_party/boringssl/src/include/openssl/mem.h"
#include "third_party/boringssl/src/include/openssl/sha.h"
#include "third_party/icu/source/common/unicode/normalizer2.h"
#include "third_party/icu/source/common/unicode/uchar.h"
#include "third_party/icu/source/common/unicode/unistr.h"
#include "third_party/icu/source/common/unicode/utf8.h"

namespace brave_sync::crypto::account {

namespace {

constexpr char kDomain[] = "flyweb-cuenta-v1";
constexpr char kLoginInfo[] = "flyweb-cuenta-v1/entrar";
constexpr char kWrapInfo[] = "flyweb-cuenta-v1/envolver";
constexpr char kWrapAdPrefix[] = "flyweb-envoltura-v1";

// Same constants as BraveSyncAuthManager::DeriveSigningKeys: the account proof
// has to be signed with the key whose public half is the go-sync client_id.
// BraveSyncAuthManager is not touched (§11); the unit test checks both give
// the same client_id for the same words.
constexpr uint8_t kChainHkdfSalt[] = {
    72,  203, 156, 43,  64,  229, 225, 127, 214, 158, 50,  29,  130,
    186, 182, 207, 6,   108, 47,  254, 245, 71,  198, 109, 44,  108,
    32,  193, 221, 126, 119, 143, 112, 113, 87,  184, 239, 231, 230,
    234, 28,  135, 54,  42,  9,   243, 39,  30,  179, 147, 194, 211,
    212, 239, 225, 52,  192, 219, 145, 40,  95,  19,  142, 98};
constexpr char kChainHkdfInfo[] = "sync-auth-key";

std::array<uint8_t, SHA256_DIGEST_LENGTH> Sha256(base::span<const uint8_t> in) {
  std::array<uint8_t, SHA256_DIGEST_LENGTH> out;
  SHA256(in.data(), in.size(), out.data());
  return out;
}

void AppendWithLength(std::vector<uint8_t>& out, base::span<const uint8_t> in) {
  CHECK_LE(in.size(), 0xffffu);
  out.push_back(static_cast<uint8_t>(in.size() >> 8));
  out.push_back(static_cast<uint8_t>(in.size() & 0xff));
  out.insert(out.end(), in.begin(), in.end());
}

std::vector<uint8_t> WrapAssociatedData(std::string_view email) {
  std::vector<uint8_t> ad(std::begin(kWrapAdPrefix),
                          std::end(kWrapAdPrefix) - 1);  // no '\0'
  const auto email_hash = Sha256(base::as_byte_span(email));
  ad.insert(ad.end(), email_hash.begin(), email_hash.end());
  return ad;
}

bool IsAscii(std::string_view s) {
  return std::all_of(s.begin(), s.end(), [](char c) {
    return static_cast<unsigned char>(c) < 0x80;
  });
}

// NFC through ICU. ICU keeps its own buffers, which this cannot wipe; ASCII
// (most passwords) is already NFC and never goes through it.
std::string ToNfc(std::string_view utf8) {
  if (IsAscii(utf8)) {
    return std::string(utf8);
  }
  UErrorCode status = U_ZERO_ERROR;
  const icu::Normalizer2* nfc = icu::Normalizer2::getNFCInstance(status);
  CHECK(U_SUCCESS(status));
  icu::UnicodeString in = icu::UnicodeString::fromUTF8(
      icu::StringPiece(utf8.data(), static_cast<int32_t>(utf8.size())));
  icu::UnicodeString out = nfc->normalize(in, status);
  CHECK(U_SUCCESS(status));
  std::string result;
  out.toUTF8String(result);
  // Overwrite what we can reach of ICU's copies.
  in.setTo(static_cast<UChar>(0), in.length());
  out.setTo(static_cast<UChar>(0), out.length());
  return result;
}

std::optional<AccountKeys> DeriveWithCost(std::string_view password,
                                          std::string_view email,
                                          uint64_t scrypt_n) {
  // salt = SHA-256("flyweb-cuenta-v1" || email): no per-account salt to ask
  // the server for before logging in, which would tell whether the email has
  // an account (§4.2).
  std::vector<uint8_t> salt_input(std::begin(kDomain), std::end(kDomain) - 1);
  salt_input.insert(salt_input.end(), email.begin(), email.end());
  const auto salt = Sha256(salt_input);

  SecretKey ke;
  if (!EVP_PBE_scrypt(password.data(), password.size(), salt.data(),
                      salt.size(), scrypt_n, kScryptR, kScryptP, kScryptMaxMem,
                      ke.mutable_bytes().data(), kKeySize)) {
    return std::nullopt;
  }

  AccountKeys keys;
  CHECK(HKDF(keys.login_key.mutable_bytes().data(), kKeySize, EVP_sha256(),
             ke.bytes().data(), kKeySize, /*salt=*/nullptr, 0,
             reinterpret_cast<const uint8_t*>(kLoginInfo),
             sizeof(kLoginInfo) - 1));
  CHECK(HKDF(keys.wrap_key.mutable_bytes().data(), kKeySize, EVP_sha256(),
             ke.bytes().data(), kKeySize, /*salt=*/nullptr, 0,
             reinterpret_cast<const uint8_t*>(kWrapInfo),
             sizeof(kWrapInfo) - 1));
  return keys;  // `ke` is wiped here.
}

std::vector<uint8_t> Seal(const SecretKey& key,
                          base::span<const uint8_t, kSeedSize> seed,
                          std::string_view email,
                          base::span<const uint8_t, kGcmNonceSize> nonce) {
  bssl::ScopedEVP_AEAD_CTX ctx;
  CHECK(EVP_AEAD_CTX_init(ctx.get(), EVP_aead_aes_256_gcm(),
                          key.bytes().data(), kKeySize, kGcmTagSize,
                          /*engine=*/nullptr));
  const std::vector<uint8_t> ad = WrapAssociatedData(email);
  std::vector<uint8_t> out(kWrappedSeedSize);
  std::ranges::copy(nonce, out.begin());
  size_t sealed_len = 0;
  CHECK(EVP_AEAD_CTX_seal(ctx.get(), out.data() + kGcmNonceSize, &sealed_len,
                          out.size() - kGcmNonceSize, nonce.data(),
                          nonce.size(), seed.data(), seed.size(), ad.data(),
                          ad.size()));
  CHECK_EQ(sealed_len, kSeedSize + kGcmTagSize);
  return out;
}

}  // namespace

SecretKey::SecretKey() {
  bytes_.fill(0);
}

SecretKey::SecretKey(base::span<const uint8_t, kKeySize> bytes) {
  std::ranges::copy(bytes, bytes_.begin());
}

SecretKey::SecretKey(SecretKey&& other) {
  bytes_ = other.bytes_;
  OPENSSL_cleanse(other.bytes_.data(), other.bytes_.size());
}

SecretKey& SecretKey::operator=(SecretKey&& other) {
  if (this != &other) {
    bytes_ = other.bytes_;
    OPENSSL_cleanse(other.bytes_.data(), other.bytes_.size());
  }
  return *this;
}

SecretKey::~SecretKey() {
  OPENSSL_cleanse(bytes_.data(), bytes_.size());
}

ChainKeys::ChainKeys() = default;

ChainKeys::ChainKeys(ChainKeys&& other) {
  *this = std::move(other);
}

ChainKeys& ChainKeys::operator=(ChainKeys&& other) {
  if (this != &other) {
    empty_ = other.empty_;
    public_key_ = other.public_key_;
    private_key_ = other.private_key_;
    other.empty_ = true;
    other.public_key_.fill(0);
    OPENSSL_cleanse(other.private_key_.data(), other.private_key_.size());
  }
  return *this;
}

ChainKeys::~ChainKeys() {
  OPENSSL_cleanse(private_key_.data(), private_key_.size());
}

// static
ChainKeys ChainKeys::FromSeed(base::span<const uint8_t, kSeedSize> seed) {
  std::vector<uint8_t> seed_v(seed.begin(), seed.end());
  const std::vector<uint8_t> salt(std::begin(kChainHkdfSalt),
                                  std::end(kChainHkdfSalt));
  const std::vector<uint8_t> info(std::begin(kChainHkdfInfo),
                                  std::end(kChainHkdfInfo) - 1);
  std::vector<uint8_t> ed_seed =
      HKDFSha512(seed_v, &salt, &info, ED25519_SEED_LEN);
  ChainKeys keys;
  ED25519_keypair_from_seed(keys.public_key_.data(), keys.private_key_.data(),
                            ed_seed.data());
  keys.empty_ = false;
  OPENSSL_cleanse(ed_seed.data(), ed_seed.size());
  // seed_v is a copy of the seed: wipe it too.
  OPENSSL_cleanse(seed_v.data(), seed_v.size());
  return keys;
}

ChainKeys ChainKeys::Clone() const {
  ChainKeys copy;
  copy.empty_ = empty_;
  copy.public_key_ = public_key_;
  copy.private_key_ = private_key_;
  return copy;
}

std::string ChainKeys::ChainId() const {
  CHECK(!empty_);
  return base::HexEncode(public_key_);
}

std::array<uint8_t, kProofSignatureSize> ChainKeys::SignProof(
    std::string_view method,
    std::string_view path,
    std::string_view op,
    std::string_view account_id,
    base::span<const uint8_t> nonce,
    std::string_view body) const {
  CHECK(!empty_);
  const std::vector<uint8_t> message =
      BuildProofMessage(method, path, op, account_id, nonce, body);
  std::array<uint8_t, kProofSignatureSize> signature;
  CHECK(ED25519_sign(signature.data(), message.data(), message.size(),
                     private_key_.data()));
  return signature;
}

std::vector<uint8_t> BuildProofMessage(std::string_view method,
                                       std::string_view path,
                                       std::string_view op,
                                       std::string_view account_id,
                                       base::span<const uint8_t> nonce,
                                       std::string_view body) {
  std::vector<uint8_t> m(std::begin(kDomain), std::end(kDomain) - 1);
  AppendWithLength(m, base::as_byte_span(method));
  AppendWithLength(m, base::as_byte_span(path));
  AppendWithLength(m, base::as_byte_span(op));
  AppendWithLength(m, base::as_byte_span(account_id));
  AppendWithLength(m, nonce);
  const auto body_hash = Sha256(base::as_byte_span(body));
  m.insert(m.end(), body_hash.begin(), body_hash.end());
  return m;
}

AccountKeys::AccountKeys() = default;
AccountKeys::AccountKeys(AccountKeys&&) = default;
AccountKeys& AccountKeys::operator=(AccountKeys&&) = default;
AccountKeys::~AccountKeys() = default;

std::optional<std::string> NormalizeEmail(std::string_view email) {
  if (!base::IsStringUTF8(email)) {
    return std::nullopt;
  }
  // Trim (White_Space, like Go's unicode.IsSpace) and lower-case code point by
  // code point (simple mapping, like Go's strings.ToLower).
  std::vector<UChar32> cps;
  for (int32_t i = 0; i < static_cast<int32_t>(email.size());) {
    UChar32 c;
    U8_NEXT(email.data(), i, static_cast<int32_t>(email.size()), c);
    if (c < 0) {
      return std::nullopt;
    }
    cps.push_back(c);
  }
  const auto is_space = [](UChar32 c) { return !!u_isUWhiteSpace(c); };
  auto first = std::ranges::find_if_not(cps, is_space);
  auto last = std::find_if_not(cps.rbegin(), std::make_reverse_iterator(first),
                               is_space)
                  .base();
  std::string lowered;
  for (auto it = first; it != last; ++it) {
    const UChar32 c = u_tolower(*it);
    if (u_isUWhiteSpace(c) || u_charType(c) == U_CONTROL_CHAR || c == '<' ||
        c == '>' || c == '"' || c == ',' || c == ';') {
      return std::nullopt;
    }
    char buf[U8_MAX_LENGTH];
    int32_t len = 0;
    UBool error = false;
    U8_APPEND(buf, len, U8_MAX_LENGTH, c, error);
    if (error) {
      return std::nullopt;
    }
    lowered.append(buf, static_cast<size_t>(len));
  }
  std::string normalized = ToNfc(lowered);
  if (normalized.size() < 3 || normalized.size() > 254 ||
      std::ranges::count(normalized, '@') != 1) {
    return std::nullopt;
  }
  const size_t at = normalized.find('@');
  if (at == 0 || at == normalized.size() - 1 ||
      normalized.find('.', at + 1) == std::string::npos) {
    return std::nullopt;
  }
  return normalized;
}

std::string NormalizePassword(std::string_view password) {
  return ToNfc(password);
}

size_t CountCodePoints(std::string_view utf8) {
  size_t n = 0;
  for (int32_t i = 0; i < static_cast<int32_t>(utf8.size());) {
    UChar32 c;
    U8_NEXT(utf8.data(), i, static_cast<int32_t>(utf8.size()), c);
    ++n;
  }
  return n;
}

std::optional<AccountKeys> DeriveAccountKeys(std::string_view password,
                                             std::string_view email) {
  return DeriveWithCost(password, email, kScryptN);
}

std::optional<AccountKeys> DeriveAccountKeysWithCostForTesting(
    std::string_view password,
    std::string_view email,
    uint64_t scrypt_n) {
  return DeriveWithCost(password, email, scrypt_n);
}

std::vector<uint8_t> WrapSeed(const SecretKey& wrap_key,
                              base::span<const uint8_t, kSeedSize> seed,
                              std::string_view email) {
  std::array<uint8_t, kGcmNonceSize> nonce;
  ::crypto::RandBytes(nonce);
  return Seal(wrap_key, seed, email, nonce);
}

std::vector<uint8_t> WrapSeedWithNonceForTesting(
    const SecretKey& wrap_key,
    base::span<const uint8_t, kSeedSize> seed,
    std::string_view email,
    base::span<const uint8_t, kGcmNonceSize> nonce) {
  return Seal(wrap_key, seed, email, nonce);
}

std::optional<SecretKey> UnwrapSeed(const SecretKey& wrap_key,
                                    base::span<const uint8_t> wrapped,
                                    std::string_view email) {
  if (wrapped.size() != kWrappedSeedSize) {
    return std::nullopt;
  }
  bssl::ScopedEVP_AEAD_CTX ctx;
  if (!EVP_AEAD_CTX_init(ctx.get(), EVP_aead_aes_256_gcm(),
                         wrap_key.bytes().data(), kKeySize, kGcmTagSize,
                         /*engine=*/nullptr)) {
    return std::nullopt;
  }
  const std::vector<uint8_t> ad = WrapAssociatedData(email);
  SecretKey seed;
  size_t out_len = 0;
  if (!EVP_AEAD_CTX_open(ctx.get(), seed.mutable_bytes().data(), &out_len,
                         kSeedSize, wrapped.data(), kGcmNonceSize,
                         wrapped.data() + kGcmNonceSize,
                         wrapped.size() - kGcmNonceSize, ad.data(),
                         ad.size()) ||
      out_len != kSeedSize) {
    return std::nullopt;
  }
  return seed;
}

void WipeString(std::string& s) {
  if (!s.empty()) {
    OPENSSL_cleanse(s.data(), s.size());
  }
  s.clear();
}

}  // namespace brave_sync::crypto::account

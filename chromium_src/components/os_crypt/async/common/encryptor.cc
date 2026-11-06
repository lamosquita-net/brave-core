/* Copyright (c) 2026 lamosquita. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "components/os_crypt/async/common/encryptor.h"

#include <algorithm>
#include <cstdint>

#include "base/containers/span.h"
#include "base/strings/string_util.h"
#include "build/build_config.h"

namespace os_crypt_async {
namespace {

// "v" + two digits (`v10`, `v11`, `v20`...) or "fw" + one digit (`fw1`): the
// tags real providers use. Data starting like this is encrypted, even when
// its provider is not loaded at all, and must never come back as cleartext.
bool FlyWebHasProviderTag(base::span<const uint8_t> data) {
  if (data.size() >= 3u && data[0] == 'v' && base::IsAsciiDigit(data[1]) &&
      base::IsAsciiDigit(data[2])) {
    return true;
  }
  return data.size() >= 3u && data[0] == 'f' && data[1] == 'w' &&
         base::IsAsciiDigit(data[2]);
}

// FlyWeb (2.x on Chromium 128 decrypted through OSCrypt sync): on Mac and
// Linux OSCrypt returned data without the `v10`/`v11` prefix as is, legacy
// cleartext from before encryption. 156 has no OSCrypt and returns nullopt,
// which would drop those values. Keep the 2.x answer for them, marked for
// re-encryption. Only with a real provider in the keyring (`v10`, `v11`,
// `fw1`, even if temporarily unavailable), so test keyrings and Windows
// (DPAPI blobs carry no prefix) keep upstream's behaviour.
bool FlyWebIsLegacyCleartext(const Encryptor::KeyRing& keys,
                             base::span<const uint8_t> data) {
#if BUILDFLAG(IS_MAC) || BUILDFLAG(IS_LINUX)
  if (FlyWebHasProviderTag(data)) {
    return false;
  }
  bool has_real_provider = false;
  for (const auto& [tag, key] : keys) {
    if (data.size() >= tag.size() &&
        std::ranges::equal(base::as_byte_span(tag), data.first(tag.size()))) {
      return false;
    }
    has_real_provider |= FlyWebHasProviderTag(base::as_byte_span(tag));
  }
  return has_real_provider;
#else
  return false;
#endif
}

}  // namespace
}  // namespace os_crypt_async

#define BRAVE_ENCRYPTOR_DECRYPT_DATA                 \
  if (FlyWebIsLegacyCleartext(keys_, data)) {        \
    if (flags) {                                     \
      flags->should_reencrypt = true;                \
    }                                                \
    return std::string(data.begin(), data.end());    \
  }

#include "src/components/os_crypt/async/common/encryptor.cc"
#undef BRAVE_ENCRYPTOR_DECRYPT_DATA

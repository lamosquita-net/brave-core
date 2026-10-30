/* Copyright (c) 2026 lamosquita. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include <string_view>

#include "base/containers/span.h"
#include "base/ranges/algorithm.h"

// FlyWeb `fw1` (FS.25): data tagged "fw1" (Fw1KeyProvider::kKeyTag) that no
// loaded key can open must fail here. OSCrypt sync on Mac returns anything
// without the `v10` prefix as cleartext, which would hand the ciphertext to
// cookies, passwords or the sync "delete undecryptable" logic as if it were
// valid data.
#define BRAVE_ENCRYPTOR_DECRYPT_DATA_BEFORE_OS_CRYPT_FALLBACK            \
  {                                                                      \
    constexpr std::string_view kFlyWebFw1Tag = "fw1";                    \
    if (data.size() >= kFlyWebFw1Tag.size() &&                           \
        base::ranges::equal(base::as_byte_span(kFlyWebFw1Tag),           \
                            data.first(kFlyWebFw1Tag.size()))) {         \
      return std::nullopt;                                               \
    }                                                                    \
  }

#include "src/components/os_crypt/async/common/encryptor.cc"
#undef BRAVE_ENCRYPTOR_DECRYPT_DATA_BEFORE_OS_CRYPT_FALLBACK

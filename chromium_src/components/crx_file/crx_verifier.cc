/* Copyright (c) 2022 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * you can obtain one at http://mozilla.org/MPL/2.0/. */

#include "components/crx_file/crx_verifier.h"

#include <array>
#include <utility>

#include "base/containers/span.h"
#include "base/notreached.h"

namespace {

// FlyWeb: publisher key of FlyWeb's own components (signed on bak), the only
// publisher key accepted besides upstream's kPublisherKeyHash. Brave's
// publisher key is not accepted. Must match `publicador_sha256` in softmac
// FlyWeb/servidor/componentes/claves-publicas.json.
constexpr uint8_t kFlyWebPublisherKeyHash[] = {
    0xb3, 0xa3, 0x57, 0x66, 0x0e, 0xb4, 0x98, 0x58, 0xf8, 0xa3, 0x23,
    0x01, 0xe9, 0xaa, 0x79, 0x80, 0x66, 0xe6, 0xdd, 0x74, 0x5c, 0x7a,
    0x06, 0x08, 0xd7, 0xc8, 0x9f, 0x0d, 0x5e, 0x2a, 0xc7, 0x30};

#if !defined(OFFICIAL_BUILD)
// Writable copy so tests can swap in their own key. Not in official builds.
base::span<uint8_t> GetFlyWebPublisherKeyHash() {
  static auto flyweb_publisher_key = std::to_array(kFlyWebPublisherKeyHash);
  return base::span(flyweb_publisher_key);
}
#endif  // !defined(OFFICIAL_BUILD)

// Used in the patch in crx_verifier.cc.
bool IsFlyWebPublisher(base::span<const uint8_t> key_hash) {
#if defined(OFFICIAL_BUILD)
  return base::span<const uint8_t>(kFlyWebPublisherKeyHash) == key_hash;
#else
  return base::span<const uint8_t>(GetFlyWebPublisherKeyHash()) == key_hash;
#endif
}

}  // namespace

namespace crx_file {

void SetFlyWebPublisherKeyHashForTesting(base::span<const uint8_t> test_key) {
#if defined(OFFICIAL_BUILD)
  NOTREACHED();
#else
  GetFlyWebPublisherKeyHash().copy_from(test_key);
#endif
}

}  // namespace crx_file

#include "src/components/crx_file/crx_verifier.cc"

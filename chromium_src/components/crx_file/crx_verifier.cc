/* Copyright (c) 2022 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * you can obtain one at http://mozilla.org/MPL/2.0/. */

#include "components/crx_file/crx_verifier.h"

#include <utility>
#include <vector>

#include "base/no_destructor.h"
#include "base/ranges/algorithm.h"

namespace {

// The brave publisher key in alternative to google one (kPublisherKeyHash).
constexpr uint8_t kBravePublisherKeyHash[] = {
    0x93, 0x74, 0xd6, 0x2a, 0x32, 0x76, 0x74, 0x74, 0xac, 0x99, 0xd9,
    0xc0, 0x55, 0xea, 0xf2, 0x6e, 0x10, 0x7,  0x45, 0x6,  0xb9, 0xd5,
    0x35, 0xc8, 0x35, 0x8,  0x28, 0x97, 0x5f, 0x7a, 0xc1, 0x97};

// FlyWeb: publisher key of FlyWeb's own components (signed on bak). Accepted in
// addition to Brave's, never instead of it.
constexpr uint8_t kFlyWebPublisherKeyHash[] = {
    0xb3, 0xa3, 0x57, 0x66, 0x0e, 0xb4, 0x98, 0x58, 0xf8, 0xa3, 0x23,
    0x01, 0xe9, 0xaa, 0x79, 0x80, 0x66, 0xe6, 0xdd, 0x74, 0x5c, 0x7a,
    0x06, 0x08, 0xd7, 0xc8, 0x9f, 0x0d, 0x5e, 0x2a, 0xc7, 0x30};

std::vector<uint8_t>& GetBravePublisherKeyHash() {
  static base::NoDestructor<std::vector<uint8_t>> brave_publisher_key(
      std::begin(kBravePublisherKeyHash), std::end(kBravePublisherKeyHash));
  return *brave_publisher_key;
}

// Used in the patch in crx_verifier.cc.
bool IsBravePublisher(const std::vector<uint8_t>& key_hash) {
  return GetBravePublisherKeyHash() == key_hash ||
         base::ranges::equal(kFlyWebPublisherKeyHash, key_hash);
}

}  // namespace

namespace crx_file {

void SetBravePublisherKeyHashForTesting(const std::vector<uint8_t>& test_key) {
  GetBravePublisherKeyHash() = test_key;
}

}  // namespace crx_file

#include "src/components/crx_file/crx_verifier.cc"

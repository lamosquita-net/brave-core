/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_CHROMIUM_SRC_NET_BASE_FEATURES_H_
#define BRAVE_CHROMIUM_SRC_NET_BASE_FEATURES_H_

#include "base/feature_list.h"
#include "base/metrics/field_trial_params.h"
#include "net/base/net_export.h"

namespace net {
namespace features {

NET_EXPORT BASE_DECLARE_FEATURE(kBraveEphemeralStorage);
NET_EXPORT BASE_DECLARE_FEATURE(kBraveEphemeralStorageKeepAlive);
NET_EXPORT extern const base::FeatureParam<int>
    kBraveEphemeralStorageKeepAliveTimeInSeconds;
NET_EXPORT BASE_DECLARE_FEATURE(kBraveFirstPartyEphemeralStorage);
NET_EXPORT BASE_DECLARE_FEATURE(kBraveHttpsByDefault);
NET_EXPORT BASE_DECLARE_FEATURE(kBravePartitionBlobStorage);
NET_EXPORT BASE_DECLARE_FEATURE(kBravePartitionHSTS);
NET_EXPORT BASE_DECLARE_FEATURE(kBraveTorWindowsHttpsOnly);
NET_EXPORT BASE_DECLARE_FEATURE(kBraveForgetFirstPartyStorage);
// FlyWeb (FS.17, level 131): as Chromium 1fef3fb10d7. TLS 1.3 and QUIC offer
// X25519MLKEM768 (0x11ec) instead of X25519Kyber768Draft00 (0x6399) when
// post-quantum key agreement is on. Kill switch: --disable-features=UseMLKEM.
NET_EXPORT BASE_DECLARE_FEATURE(kUseMLKEM);
NET_EXPORT extern const base::FeatureParam<int>
    kBraveForgetFirstPartyStorageStartupCleanupDelayInSeconds;
NET_EXPORT extern const base::FeatureParam<bool>
    kBraveForgetFirstPartyStorageByDefault;

}  // namespace features
}  // namespace net

#include "src/net/base/features.h"  // IWYU pragma: export

#endif  // BRAVE_CHROMIUM_SRC_NET_BASE_FEATURES_H_

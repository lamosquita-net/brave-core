/* Copyright (c) 2026 lamosquita. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#define IsDefaultDecoderSupportedVideoType \
  IsDefaultDecoderSupportedVideoType_ChromiumImpl
#define IsDecoderSupportedVideoType IsDecoderSupportedVideoType_ChromiumImpl
#include "src/media/base/supported_types.cc"
#undef IsDecoderSupportedVideoType
#undef IsDefaultDecoderSupportedVideoType

namespace media {

// Off: pages get H.264. --enable-features=FlyWebSoftwareVp9Av1 brings VP9 and
// AV1 back (software decoding).
BASE_FEATURE(kFlyWebSoftwareVp9Av1,
             "FlyWebSoftwareVp9Av1",
             base::FEATURE_DISABLED_BY_DEFAULT);

namespace {
bool FlyWebHidesCodec(const VideoType& type) {
  return (type.codec == VideoCodec::kVP9 || type.codec == VideoCodec::kAV1) &&
         !base::FeatureList::IsEnabled(kFlyWebSoftwareVp9Av1);
}
}  // namespace

// Both: renderers ask through their media client (ContentRendererClient ->
// IsDefaultDecoderSupportedVideoType), other processes through
// IsDecoderSupportedVideoType.
bool IsDecoderSupportedVideoType(const VideoType& type) {
  return !FlyWebHidesCodec(type) &&
         IsDecoderSupportedVideoType_ChromiumImpl(type);
}

bool IsDefaultDecoderSupportedVideoType(const VideoType& type) {
  return !FlyWebHidesCodec(type) &&
         IsDefaultDecoderSupportedVideoType_ChromiumImpl(type);
}

}  // namespace media

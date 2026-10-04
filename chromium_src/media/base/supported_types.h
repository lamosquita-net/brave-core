/* Copyright (c) 2026 lamosquita. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_CHROMIUM_SRC_MEDIA_BASE_SUPPORTED_TYPES_H_
#define BRAVE_CHROMIUM_SRC_MEDIA_BASE_SUPPORTED_TYPES_H_

#include "base/feature_list.h"

// FlyWeb: Chromium's IsSupportedVideoType() and IsDefaultSupportedVideoType()
// become *_ChromiumImpl(); FlyWeb's versions (see the .cc) wrap them. The .cc
// defines the macros before including this header, so they are only
// undefined here when this header defined them.
#if !defined(IsDefaultSupportedVideoType)
#define IsDefaultSupportedVideoType IsDefaultSupportedVideoType_ChromiumImpl
#define IsSupportedVideoType IsSupportedVideoType_ChromiumImpl
#define FLYWEB_UNDEF_SUPPORTED_VIDEO_TYPE
#endif

#include "src/media/base/supported_types.h"  // IWYU pragma: export

#if defined(FLYWEB_UNDEF_SUPPORTED_VIDEO_TYPE)
#undef IsDefaultSupportedVideoType
#undef IsSupportedVideoType
#undef FLYWEB_UNDEF_SUPPORTED_VIDEO_TYPE
#endif

namespace media {

// FlyWeb: VP9 and AV1 decode in software on every Intel Mac FlyWeb targets
// (measured on the MacPro5,1, 6,1 and 7,1, softmac FlyWeb/docs/multimedia.md),
// which makes 1080p video stutter on the older GPUs. Unless this feature is
// enabled, pages are told those codecs are not supported, so sites such as
// YouTube serve H.264, which VideoToolbox decodes in hardware (like the
// h264ify extension). WebRTC does not use this path.
MEDIA_EXPORT BASE_DECLARE_FEATURE(kFlyWebSoftwareVp9Av1);

MEDIA_EXPORT bool IsSupportedVideoType(const VideoType& type);
MEDIA_EXPORT bool IsDefaultSupportedVideoType(const VideoType& type);

}  // namespace media

#endif  // BRAVE_CHROMIUM_SRC_MEDIA_BASE_SUPPORTED_TYPES_H_

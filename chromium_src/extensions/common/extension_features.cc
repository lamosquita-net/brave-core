/* Copyright (c) 2026 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "src/extensions/common/extension_features.cc"

#include "base/feature_override.h"

namespace extensions_features {

// As 1.98: no glicPrivate (Gemini in Chrome) extension API, and no native
// messaging bridge to Android devices.
OVERRIDE_FEATURE_DEFAULT_STATES({{
    {kApiDesktopAndroidNativeMessaging, base::FEATURE_DISABLED_BY_DEFAULT},
    {kApiGlicPrivate, base::FEATURE_DISABLED_BY_DEFAULT},
}});

}  // namespace extensions_features

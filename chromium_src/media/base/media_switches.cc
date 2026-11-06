/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "src/media/base/media_switches.cc"

#include "base/feature_override.h"

namespace media {

OVERRIDE_FEATURE_DEFAULT_STATES({{
    {kLiveCaption, base::FEATURE_DISABLED_BY_DEFAULT},
    {kEnableTabMuting, base::FEATURE_ENABLED_BY_DEFAULT},
    // A screen-capture grant must not act as a blanket user gesture.
    {kGetDisplayMediaConfersActivation, base::FEATURE_DISABLED_BY_DEFAULT},
    // A mic/camera grant must not allow autoplay for good (audio
    // fingerprinting).
    {kAutoplayBypassForMicCamera, base::FEATURE_DISABLED_BY_DEFAULT},
}});

}  // namespace media

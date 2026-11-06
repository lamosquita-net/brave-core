/* Copyright (c) 2026 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "src/components/private_ai/features.cc"

#include "base/feature_override.h"

namespace private_ai {

OVERRIDE_FEATURE_DEFAULT_STATES({{
    // Google's Private AI server.
    {kPrivateAi, base::FEATURE_DISABLED_BY_DEFAULT},
}});

}  // namespace private_ai

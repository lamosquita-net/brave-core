/* Copyright (c) 2026 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "src/chrome/browser/ui/omnibox/omnibox_next_features.cc"

#include "base/feature_override.h"

namespace omnibox::internal {

// As 1.98: the omnibox keeps the views popup that Brave's omnibox overrides
// target, and the AI Mode popup (composebox) stays off.
OVERRIDE_FEATURE_DEFAULT_STATES({{
    {kWebUIOmniboxAimPopup, base::FEATURE_DISABLED_BY_DEFAULT},
    {kWebUIOmniboxPopup, base::FEATURE_DISABLED_BY_DEFAULT},
}});

}  // namespace omnibox::internal

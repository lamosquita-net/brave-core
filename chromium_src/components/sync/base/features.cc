/* Copyright (c) 2026 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "src/components/sync/base/features.cc"

#include "base/feature_override.h"

namespace syncer {

// As 1.98: no syncing of Google Wallet pass usage metadata. On Android and iOS
// the feature is country-restricted and cannot be overridden this way.
#if !BUILDFLAG(IS_ANDROID) && !BUILDFLAG(IS_IOS)
OVERRIDE_FEATURE_DEFAULT_STATES({{
    {kSyncAutofillValuableMetadata, base::FEATURE_DISABLED_BY_DEFAULT},
}});
#endif

}  // namespace syncer

/* Copyright (c) 2026 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "src/chrome/browser/enterprise/data_protection/data_protection_features.cc"

#include "base/feature_override.h"

namespace enterprise_data_protection {

// As 1.98: no forced download to cloud storage (Google Drive).
OVERRIDE_FEATURE_DEFAULT_STATES({{
    {kEnableForceDownloadToCloud, base::FEATURE_DISABLED_BY_DEFAULT},
}});

}  // namespace enterprise_data_protection

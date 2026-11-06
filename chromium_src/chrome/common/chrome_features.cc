/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "chrome/common/chrome_features.h"

#include "src/chrome/common/chrome_features.cc"

#include "base/feature_override.h"

namespace features {

OVERRIDE_FEATURE_DEFAULT_STATES({{
    {kChromeStructuredMetrics, base::FEATURE_DISABLED_BY_DEFAULT},
    // The HTTPS upgrade setting of Shields decides HTTPS-First.
    {kHttpsFirstBalancedMode, base::FEATURE_DISABLED_BY_DEFAULT},
    // kSCTAuditing is gone; hashdance is what remains of SCT auditing.
    {kSCTAuditingHashdance, base::FEATURE_DISABLED_BY_DEFAULT},
#if !BUILDFLAG(IS_ANDROID)
    {kTrustSafetySentimentSurvey, base::FEATURE_DISABLED_BY_DEFAULT},
    {kTrustSafetySentimentSurveyV2, base::FEATURE_DISABLED_BY_DEFAULT},
#endif
}});

}  // namespace features

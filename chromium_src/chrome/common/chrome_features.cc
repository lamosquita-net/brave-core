/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "chrome/common/chrome_features.h"

#define kDnsOverHttpsShowUiParam kDnsOverHttpsShowUiParamDisabled
#include "src/chrome/common/chrome_features.cc"
#undef kDnsOverHttpsShowUiParam

#include "base/feature_override.h"

namespace features {

OVERRIDE_FEATURE_DEFAULT_STATES({{
    // FlyWeb: secure DNS starts off, so lookups go to the system's DNS only.
    // Enabled, the default mode was "automatic": with a known resolver in the
    // system settings (8.8.8.8, 1.1.1.1) it switched to that provider's DoH
    // endpoint and probed it (dns.google, chrome.cloudflare-dns.com). The
    // setting stays in Privacy and security (kDnsOverHttpsShowUiParam below);
    // the stub resolver follows the pref, not this feature.
    {kDnsOverHttps, base::FEATURE_DISABLED_BY_DEFAULT},
    {kHttpsFirstModeV2, base::FEATURE_ENABLED_BY_DEFAULT},
    {kKAnonymityService, base::FEATURE_DISABLED_BY_DEFAULT},
    {kSCTAuditing, base::FEATURE_DISABLED_BY_DEFAULT},
#if BUILDFLAG(IS_ANDROID)
    {kPrivacyGuideAndroidPostMVP, base::FEATURE_DISABLED_BY_DEFAULT},
#endif
#if !BUILDFLAG(IS_ANDROID)
    {kTrustSafetySentimentSurvey, base::FEATURE_DISABLED_BY_DEFAULT},
    {kTrustSafetySentimentSurveyV2, base::FEATURE_DISABLED_BY_DEFAULT},
#endif
#if BUILDFLAG(IS_MAC)
    {kUseChromiumUpdater, base::FEATURE_DISABLED_BY_DEFAULT},
#endif
}});

// Enable the DoH settings UI in chrome://settings/security on all platforms.
const base::FeatureParam<bool> kDnsOverHttpsShowUiParam{&kDnsOverHttps,
                                                        "ShowUi", true};

}  // namespace features

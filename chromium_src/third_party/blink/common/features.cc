/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "src/third_party/blink/common/features.cc"

#include "base/feature_override.h"

namespace blink {
namespace features {

OVERRIDE_FEATURE_DEFAULT_STATES({{
    // Upgrade all mixed content
    {kMixedContentAutoupgrade, base::FEATURE_ENABLED_BY_DEFAULT},
    {kReducedReferrerGranularity, base::FEATURE_ENABLED_BY_DEFAULT},
    {kUACHOverrideBlank, base::FEATURE_ENABLED_BY_DEFAULT},

    {kAllowURNsInIframes, base::FEATURE_DISABLED_BY_DEFAULT},
    {kBackgroundResourceFetch, base::FEATURE_DISABLED_BY_DEFAULT},
    {kFencedFrames, base::FEATURE_DISABLED_BY_DEFAULT},
    // As 1.98: no eager viewport-based preloading (already off on desktop).
    {kPreloadingEagerViewportHeuristics, base::FEATURE_DISABLED_BY_DEFAULT},

    // FlyWeb: Blink runtime features Brave 1.98 ships off (its plaster
    // rewrite of runtime_enabled_features.json5). On 156 the generated
    // UpdateStatusFromBaseFeatures() copies a disabled base::Feature to a
    // "stable" Blink feature only when the feature counts as overridden;
    // base_feature_status in the json5 patch alone is not, and these listed
    // here are (//brave/chromium_src/base/feature_list.cc).
    {kAdInterestGroupAPI, base::FEATURE_DISABLED_BY_DEFAULT},
    {kAIPromptAPI, base::FEATURE_DISABLED_BY_DEFAULT},
    {kAIPromptAPIMultimodalInput, base::FEATURE_DISABLED_BY_DEFAULT},
    {kAIProofreadingAPI, base::FEATURE_DISABLED_BY_DEFAULT},
    {kAIRewriterAPI, base::FEATURE_DISABLED_BY_DEFAULT},
    {kAISummarizationAPI, base::FEATURE_DISABLED_BY_DEFAULT},
    {kAIWriterAPI, base::FEATURE_DISABLED_BY_DEFAULT},
    {kControlledFrame, base::FEATURE_DISABLED_BY_DEFAULT},
    {kExtensionScriptTagging, base::FEATURE_DISABLED_BY_DEFAULT},
    {kFencedFramesRuntime, base::FEATURE_DISABLED_BY_DEFAULT},
    {kFledge, base::FEATURE_DISABLED_BY_DEFAULT},
    {kLanguageDetectionAPI, base::FEATURE_DISABLED_BY_DEFAULT},
    {kParakeet, base::FEATURE_DISABLED_BY_DEFAULT},
    {kPrerender2, base::FEATURE_DISABLED_BY_DEFAULT},
    {kReduceUserAgentMinorVersion, base::FEATURE_ENABLED_BY_DEFAULT},
    {kTranslationAPI, base::FEATURE_DISABLED_BY_DEFAULT},
    {kUserMediaElement, base::FEATURE_DISABLED_BY_DEFAULT},
}});

BASE_FEATURE(kFileSystemAccessAPI,
             "FileSystemAccessAPI",
             base::FEATURE_DISABLED_BY_DEFAULT);

BASE_FEATURE(kBraveWebBluetoothAPI,
             "BraveWebBluetoothAPI",
             base::FEATURE_DISABLED_BY_DEFAULT);

BASE_FEATURE(kNavigatorConnectionAttribute,
             "NavigatorConnectionAttribute",
             base::FEATURE_DISABLED_BY_DEFAULT);

// Enable blink::MemoryCache partitioning for non SameSite requests.
BASE_FEATURE(kPartitionBlinkMemoryCache,
             "PartitionBlinkMemoryCache",
             base::FEATURE_ENABLED_BY_DEFAULT);

// Enable WebSockets connection pool limit per eTLD+1 for each renderer.
BASE_FEATURE(kRestrictWebSocketsPool,
             "RestrictWebSocketsPool",
             base::FEATURE_ENABLED_BY_DEFAULT);

// Enables protection against fingerprinting on screen dimensions.
BASE_FEATURE(kBraveBlockScreenFingerprinting,
             "BraveBlockScreenFingerprinting",
             base::FEATURE_DISABLED_BY_DEFAULT);

// Enables protection against fingerprinting via high-resolution time stamps.
BASE_FEATURE(kBraveRoundTimeStamps,
             "BraveRoundTimeStamps",
             base::FEATURE_ENABLED_BY_DEFAULT);

// Enables the Global Privacy Control header and navigator APIs.
BASE_FEATURE(kBraveGlobalPrivacyControl,
             "BraveGlobalPrivacyControl",
             base::FEATURE_ENABLED_BY_DEFAULT);

// Enable EventSource connection pool limit per eTLD+1.
BASE_FEATURE(kRestrictEventSourcePool,
             "RestrictEventSourcePool",
#if BUILDFLAG(IS_ANDROID)
             base::FEATURE_DISABLED_BY_DEFAULT
#else
             base::FEATURE_ENABLED_BY_DEFAULT
#endif
);

#if BUILDFLAG(IS_MAC) || BUILDFLAG(IS_LINUX)
BASE_FEATURE(kMiddleButtonClickAutoscroll,
             "MiddelButtonClickAutoscroll",
             base::FEATURE_DISABLED_BY_DEFAULT);
#endif  // BUILDFLAG(IS_MAC) || BUILDFLAG(IS_LINUX)

bool IsPrerender2Enabled() {
  return false;
}

}  // namespace features
}  // namespace blink

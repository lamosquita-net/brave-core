/* Copyright (c) 2026 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "src/chrome/browser/devtools/features.cc"

#include "base/feature_override.h"

namespace features {

OVERRIDE_FEATURE_DEFAULT_STATES({{
    // DevTools AI assistance talks to Google's AIDA endpoint: all off.
    {kDevToolsAiAssistanceAccessibilityAgent,
     base::FEATURE_DISABLED_BY_DEFAULT},
    {kDevToolsAiAssistanceContextSelectionAgent,
     base::FEATURE_DISABLED_BY_DEFAULT},
    {kDevToolsAiAssistanceFileAgent, base::FEATURE_DISABLED_BY_DEFAULT},
    {kDevToolsAiAssistanceNetworkAgent, base::FEATURE_DISABLED_BY_DEFAULT},
    {kDevToolsAiAssistancePerformanceAgent, base::FEATURE_DISABLED_BY_DEFAULT},
    {kDevToolsAiAssistanceStorageAgent, base::FEATURE_DISABLED_BY_DEFAULT},
    {kDevToolsAiCodeCompletion, base::FEATURE_DISABLED_BY_DEFAULT},
    {kDevToolsAiCodeCompletionStyles, base::FEATURE_DISABLED_BY_DEFAULT},
    {kDevToolsAiCodeGeneration, base::FEATURE_DISABLED_BY_DEFAULT},
    {kDevToolsAiGeneratedTimelineLabels, base::FEATURE_DISABLED_BY_DEFAULT},
    {kDevToolsAiNaturalLanguageInterface, base::FEATURE_DISABLED_BY_DEFAULT},
    {kDevToolsAiOriginTrialsApis, base::FEATURE_DISABLED_BY_DEFAULT},
    {kDevToolsAiV2Architecture, base::FEATURE_DISABLED_BY_DEFAULT},
    {kDevToolsConsoleInsights, base::FEATURE_DISABLED_BY_DEFAULT},
    {kDevToolsConsoleInsightsTeasers, base::FEATURE_DISABLED_BY_DEFAULT},
    {kDevToolsFreestyler, base::FEATURE_DISABLED_BY_DEFAULT},
    {kDevToolsGlobalAiButton, base::FEATURE_DISABLED_BY_DEFAULT},
    {kDevToolsNewPermissionDialog, base::FEATURE_DISABLED_BY_DEFAULT},
    {kDevToolsUseGcaApi, base::FEATURE_DISABLED_BY_DEFAULT},
    // Google Developer Program profiles and badges (Google account).
    {kDevToolsGdpProfiles, base::FEATURE_DISABLED_BY_DEFAULT},
}});

}  // namespace features

/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/browser/brave_local_state_prefs.h"

#include <string>

#include "base/values.h"
#include "brave/browser/flyweb/pruned_feature_prefs.h"
#include "brave/browser/metrics/buildflags/buildflags.h"
#include "brave/browser/metrics/metrics_reporting_util.h"
#include "brave/browser/themes/brave_dark_mode_utils.h"
#include "brave/components/brave_shields/content/browser/ad_block_service.h"
#include "brave/components/constants/pref_names.h"
#include "brave/components/l10n/common/prefs.h"
#include "brave/components/ntp_background_images/browser/view_counter_service.h"
#include "build/build_config.h"
#include "chrome/common/pref_names.h"
#include "components/metrics/metrics_pref_names.h"
#include "components/prefs/pref_registry_simple.h"
#include "third_party/widevine/cdm/buildflags.h"

#if !BUILDFLAG(IS_ANDROID)
#include "brave/browser/p3a/p3a_core_metrics.h"
#include "chrome/browser/first_run/first_run.h"
#endif  // !BUILDFLAG(IS_ANDROID)

#if defined(TOOLKIT_VIEWS)
#include "brave/browser/onboarding/onboarding_tab_helper.h"
#endif

#if BUILDFLAG(ENABLE_WIDEVINE)
#include "brave/browser/widevine/widevine_utils.h"
#endif

namespace brave {

void RegisterLocalStatePrefsForMigration(PrefRegistrySimple* registry) {
#if !BUILDFLAG(IS_ANDROID)
  // Added 10/2022
  registry->RegisterBooleanPref(kDefaultBrowserPromptEnabled, true);
#endif

  // FlyWeb: added 11/2026 (T5), NTP sponsored images and super referrals.
  ntp_background_images::ViewCounterService::
      RegisterLocalStatePrefsForMigration(registry);

  // Added 2026-11: features pruned in the Chromium 156 port.
  flyweb::RegisterPrunedFeatureLocalStatePrefs(registry);
}

void RegisterLocalStatePrefs(PrefRegistrySimple* registry) {
  brave_shields::RegisterPrefsForAdBlockService(registry);
  brave_l10n::RegisterL10nLocalStatePrefs(registry);
#if BUILDFLAG(IS_MAC)
  // Turn off super annoying 'Hold to quit'
  registry->SetDefaultPrefValue(prefs::kConfirmToQuitEnabled,
                                base::Value(false));
#endif
  registry->SetDefaultPrefValue(
      metrics::prefs::kMetricsReportingEnabled,
      base::Value(GetDefaultPrefValueForMetricsReporting()));

#if !BUILDFLAG(IS_ANDROID)
  BraveWindowTracker::RegisterPrefs(registry);
  dark_mode::RegisterBraveDarkModeLocalStatePrefs(registry);
#endif

#if defined(TOOLKIT_VIEWS)
  onboarding::RegisterLocalStatePrefs(registry);
#endif

#if BUILDFLAG(ENABLE_CRASH_DIALOG)
  registry->RegisterBooleanPref(kDontAskForCrashReporting, false);
#endif

#if BUILDFLAG(ENABLE_WIDEVINE)
  RegisterWidevineLocalstatePrefs(registry);
#endif

  RegisterLocalStatePrefsForMigration(registry);
}

}  // namespace brave

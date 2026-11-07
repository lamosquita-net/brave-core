/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/browser/brave_local_state_prefs.h"
#include "brave/browser/brave_profile_prefs.h"
#include "brave/browser/flyweb/pruned_feature_prefs.h"
#include "brave/browser/search/ntp_utils.h"
#include "brave/browser/themes/brave_dark_mode_utils.h"
#include "brave/browser/translate/brave_translate_prefs_migration.h"
#include "brave/components/brave_sync/brave_sync_prefs.h"
#include "brave/components/constants/pref_names.h"
#include "brave/components/ntp_background_images/browser/view_counter_service.h"
#include "brave/components/ntp_background_images/buildflags/buildflags.h"
#include "brave/components/omnibox/browser/brave_omnibox_prefs.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/common/channel_info.h"
#include "chrome/common/pref_names.h"
#include "components/gcm_driver/gcm_buildflags.h"
#include "components/translate/core/browser/translate_prefs.h"
#include "extensions/buildflags/buildflags.h"
#include "third_party/widevine/cdm/buildflags.h"

#if !BUILDFLAG(IS_ANDROID)
#include "brave/browser/search_engines/search_engine_provider_util.h"
#include "brave/browser/ui/bookmark/bookmark_helper.h"
#include "brave/browser/ui/webui/welcome_page/brave_welcome_page_prefs.h"
#endif

#if BUILDFLAG(ENABLE_WIDEVINE)
#include "brave/browser/widevine/widevine_utils.h"
#endif

#if !BUILDFLAG(ENABLE_EXTENSIONS)
#define CHROME_BROWSER_WEB_APPLICATIONS_WEB_APP_PROVIDER_H_
#endif  // !BUILDFLAG(ENABLE_EXTENSIONS)

// The renames below must not reach other classes' static
// MigrateObsoleteProfilePrefs, which 156 calls from browser_prefs.cc: include
// their headers first.
#include "build/build_config.h"
#include "chrome/browser/accessibility/page_colors_controller.h"
#if BUILDFLAG(IS_WIN)
#include "chrome/browser/media/cdm_pref_service_helper.h"
#endif

#define MigrateObsoleteProfilePrefs MigrateObsoleteProfilePrefs_ChromiumImpl
#define MigrateObsoleteLocalStatePrefs \
  MigrateObsoleteLocalStatePrefs_ChromiumImpl
#include "src/chrome/browser/prefs/browser_prefs.cc"
#undef MigrateObsoleteProfilePrefs
#undef MigrateObsoleteLocalStatePrefs

#if !BUILDFLAG(USE_GCM_FROM_PLATFORM)
#include "brave/browser/gcm_driver/brave_gcm_utils.h"
#endif

#if BUILDFLAG(ENABLE_CUSTOM_BACKGROUND)
#include "brave/browser/ntp_background/ntp_background_prefs.h"
#endif

// This method should be periodically pruned of year+ old migrations.
void MigrateObsoleteProfilePrefs(PrefService* profile_prefs,
                                 const base::FilePath& profile_path) {
  DCHECK(profile_prefs);
  // BEGIN_MIGRATE_OBSOLETE_PROFILE_PREFS
#if !BUILDFLAG(USE_GCM_FROM_PLATFORM)
  // Added 02/2020.
  // Must be called before ChromiumImpl because it's migrating a Chromium pref
  // to Brave pref.
  gcm::MigrateGCMPrefs(profile_prefs);
#endif

#if !BUILDFLAG(IS_ANDROID)
  // Added 11/2026 (156 removed Chromium's prefs::kHasSeenWelcomePage).
  brave_welcome_page::prefs::MigratePrefs(profile_prefs);
#endif  // !BUILDFLAG(IS_ANDROID)

  MigrateObsoleteProfilePrefs_ChromiumImpl(profile_prefs, profile_path);

  brave_sync::MigrateBraveSyncPrefs(profile_prefs);

#if !BUILDFLAG(IS_ANDROID)
  // Added 9/2020
  new_tab_page::MigrateNewTabPagePrefs(profile_prefs);

  // Added 06/2022
  brave::MigrateSearchEngineProviderPrefs(profile_prefs);

  // Added 10/2022
  profile_prefs->ClearPref(kDefaultBrowserLaunchingCount);
#endif

#if BUILDFLAG(ENABLE_EXTENSIONS)
  // Added 11/2022
  profile_prefs->ClearPref(kBraveSearchVisitCount);
#endif

  // Added 07/2021
  profile_prefs->ClearPref(prefs::kNetworkPredictionOptions);

  // Added 05/2022
  translate::ClearMigrationBraveProfilePrefs(profile_prefs);

  // Added 06/2022
#if BUILDFLAG(ENABLE_CUSTOM_BACKGROUND)
  NTPBackgroundPrefs(profile_prefs).MigrateOldPref();
#endif

  // Added 24/11/2022: https://github.com/brave/brave-core/pull/16027
#if !BUILDFLAG(IS_IOS) && !BUILDFLAG(IS_ANDROID)
  profile_prefs->ClearPref(kFTXAccessToken);
  profile_prefs->ClearPref(kFTXOauthHost);
  profile_prefs->ClearPref(kFTXNewTabPageShowFTX);
  profile_prefs->ClearPref(kCryptoDotComNewTabPageShowCryptoDotCom);
  profile_prefs->ClearPref(kCryptoDotComHasBoughtCrypto);
  profile_prefs->ClearPref(kCryptoDotComHasInteracted);
  profile_prefs->ClearPref(kGeminiAccessToken);
  profile_prefs->ClearPref(kGeminiRefreshToken);
  profile_prefs->ClearPref(kNewTabPageShowGemini);
#endif

  // Added 24/11/2022: https://github.com/brave/brave-core/pull/16027
#if !BUILDFLAG(IS_IOS)
  profile_prefs->ClearPref(kBinanceAccessToken);
  profile_prefs->ClearPref(kBinanceRefreshToken);
  profile_prefs->ClearPref(kNewTabPageShowBinance);
  profile_prefs->ClearPref(kBraveSuggestedSiteSuggestionsEnabled);
#endif

  // Added 03/2024

  // Added 2023-09
  ntp_background_images::ViewCounterService::MigrateObsoleteProfilePrefs(
      profile_prefs);

  // Added 2024-07
  profile_prefs->ClearPref(kHangoutsEnabled);

  // Added 2026-11: features pruned in the Chromium 156 port.
  flyweb::ClearPrunedFeatureProfilePrefs(profile_prefs);

  // Added 2026-11: Brave's bookmark bar state to Chromium's
  // kBookmarkBarVisibilityState.
#if !BUILDFLAG(IS_ANDROID)
  brave::MigrateBookmarkState(profile_prefs);
#endif

  // END_MIGRATE_OBSOLETE_PROFILE_PREFS
}

// This method should be periodically pruned of year+ old migrations.
void MigrateObsoleteLocalStatePrefs(PrefService* local_state) {
  // BEGIN_MIGRATE_OBSOLETE_LOCAL_STATE_PREFS
  MigrateObsoleteLocalStatePrefs_ChromiumImpl(local_state);

#if !BUILDFLAG(IS_ANDROID)
  // Added 10/2022
  local_state->ClearPref(kDefaultBrowserPromptEnabled);
#endif

  // FlyWeb: added 11/2026 (T5), NTP sponsored images and super referrals.
  ntp_background_images::ViewCounterService::MigrateObsoleteLocalStatePrefs(
      local_state);

  // Added 2026-11: features pruned in the Chromium 156 port.
  flyweb::ClearPrunedFeatureLocalStatePrefs(local_state);

  // END_MIGRATE_OBSOLETE_LOCAL_STATE_PREFS
}

#if !BUILDFLAG(ENABLE_EXTENSIONS)
#undef CHROME_BROWSER_WEB_APPLICATIONS_WEB_APP_PROVIDER_H_
#endif  // !BUILDFLAG(ENABLE_EXTENSIONS)

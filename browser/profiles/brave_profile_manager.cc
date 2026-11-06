// Copyright (c) 2019 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// you can obtain one at http://mozilla.org/MPL/2.0/.

#include "brave/browser/profiles/brave_profile_manager.h"

#include "brave/browser/perf/brave_perf_features_processor.h"
#include "brave/browser/profiles/profile_util.h"
#include "brave/browser/request_otr/request_otr_service_factory.h"
#include "brave/browser/url_sanitizer/url_sanitizer_service_factory.h"
#include "brave/components/constants/pref_names.h"
#include "brave/components/content_settings/core/browser/brave_content_settings_pref_provider.h"
#include "brave/components/request_otr/common/buildflags/buildflags.h"
#include "chrome/browser/browser_process.h"
#include "chrome/browser/profiles/profiles_state.h"
#include "chrome/common/pref_names.h"
#include "components/bookmarks/common/bookmark_pref_names.h"
#include "components/gcm_driver/gcm_buildflags.h"
#include "components/prefs/pref_service.h"
#include "components/signin/public/base/signin_pref_names.h"
#include "content/public/browser/browser_thread.h"

#if !BUILDFLAG(USE_GCM_FROM_PLATFORM)
#include "brave/browser/gcm_driver/brave_gcm_channel_status.h"
#endif

using content::BrowserThread;

BraveProfileManager::BraveProfileManager(const base::FilePath& user_data_dir)
    : ProfileManager(user_data_dir) {}

void BraveProfileManager::InitProfileUserPrefs(Profile* profile) {
  // migrate obsolete plugin prefs to temporary migration pref because otherwise
  // they get deleteed by PrefProvider before we can migrate them in
  // BravePrefProvider
  content_settings::BravePrefProvider::CopyPluginSettingsForMigration(
      profile->GetPrefs());

// Chromecast is enabled by default on Android.
#if !BUILDFLAG(IS_ANDROID)
  auto* pref_service = profile->GetPrefs();
  // At start, the value of kEnableMediaRouterOnRestart is updated to match
  // kEnableMediaRouter so users don't lose their current setting
  if (pref_service->FindPreference(kEnableMediaRouterOnRestart)
          ->IsDefaultValue()) {
    auto enabled = pref_service->GetBoolean(::prefs::kEnableMediaRouter);
    pref_service->SetBoolean(kEnableMediaRouterOnRestart, enabled);
  } else {
    // For Desktop, kEnableMediaRouterOnRestart is used to track the current
    // state of the media router switch in flyweb://settings/extensions. The
    // value of kEnableMediaRouter is only updated to match
    // kEnableMediaRouterOnRestart on restart
    auto enabled = pref_service->GetBoolean(kEnableMediaRouterOnRestart);
    pref_service->SetBoolean(::prefs::kEnableMediaRouter, enabled);
  }
#endif

  ProfileManager::InitProfileUserPrefs(profile);
  brave::SetDefaultSearchVersion(profile, profile->IsNewProfile());
  brave::SetDefaultThirdPartyCookieBlockValue(profile);
  perf::MaybeEnableBraveFeatureForPerfTesting(profile);
  brave::MigrateHttpsUpgradeSettings(profile);
}

void BraveProfileManager::DoFinalInitForServices(Profile* profile,
                                                 bool go_off_the_record) {
  ProfileManager::DoFinalInitForServices(profile, go_off_the_record);
  if (!do_final_services_init_)
    return;
#if !BUILDFLAG(USE_GCM_FROM_PLATFORM)
  gcm::BraveGCMChannelStatus* status =
      gcm::BraveGCMChannelStatus::GetForProfile(profile);
  DCHECK(status);
  status->UpdateGCMDriverStatus();
#endif
  brave::URLSanitizerServiceFactory::GetForBrowserContext(profile);
#if BUILDFLAG(ENABLE_REQUEST_OTR)
  request_otr::RequestOTRServiceFactory::GetForBrowserContext(profile);
#endif
}

bool BraveProfileManager::IsAllowedProfilePath(
    const base::FilePath& path) const {
  // Chromium only allows profiles to be created in the user_data_dir, but we
  // want to also be able to create profile in subfolders of user_data_dir.
  return ProfileManager::IsAllowedProfilePath(path) ||
         user_data_dir().IsParent(path.DirName());
}

// This overridden method doesn't clear |kDefaultSearchProviderDataPrefName|.
// W/o this, prefs set by TorWindowSearchEngineProviderService is cleared
// during the initialization.
void BraveProfileManager::SetNonPersonalProfilePrefs(Profile* profile) {
  PrefService* prefs = profile->GetPrefs();
  prefs->SetBoolean(prefs::kSigninAllowed, false);
  prefs->SetBoolean(bookmarks::prefs::kEditBookmarksEnabled, false);
  prefs->SetBoolean(bookmarks::prefs::kShowBookmarkBar, false);
}

BraveProfileManagerWithoutInit::BraveProfileManagerWithoutInit(
    const base::FilePath& user_data_dir)
    : BraveProfileManager(user_data_dir) {
  set_do_final_services_init(false);
}

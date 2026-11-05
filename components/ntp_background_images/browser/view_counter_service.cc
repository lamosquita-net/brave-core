// Copyright (c) 2020 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// you can obtain one at http://mozilla.org/MPL/2.0/.

#include "brave/components/ntp_background_images/browser/view_counter_service.h"

#include <string>

#include "brave/components/ntp_background_images/browser/brave_ntp_custom_background_service.h"
#include "brave/components/ntp_background_images/buildflags/buildflags.h"
#include "brave/components/ntp_background_images/common/pref_names.h"
#include "components/pref_registry/pref_registry_syncable.h"
#include "components/prefs/pref_registry_simple.h"
#include "components/prefs/pref_service.h"

namespace {

// Obsolete pref
constexpr char kObsoleteCountToBrandedWallpaperPref[] =
    "brave.count_to_branded_wallpaper";

// FlyWeb: obsolete since sponsored images and super referral wallpapers were
// removed (T5). Profiles from 2.7.0 may still carry them.
constexpr char kObsoleteBrandedWallpaperNotificationDismissed[] =
    "brave.branded_wallpaper_notification_dismissed";
constexpr char kObsoleteNewTabPageShowSponsoredImagesBackgroundImage[] =
    "brave.new_tab_page.show_branded_background_image";
constexpr char kObsoleteNewTabPageSuperReferralThemesOption[] =
    "brave.new_tab_page.super_referral_themes_option";

// Local state.
constexpr char kObsoleteNewTabsCreated[] =
    "brave.new_tab_page.p3a_new_tabs_created";
constexpr char kObsoleteSponsoredNewTabsCreated[] =
    "brave.new_tab_page.p3a_sponsored_new_tabs_created";
constexpr char kObsoleteCachedSuperReferralComponentInfo[] =
    "brave.new_tab_page.cached_super_referral_component_info";
constexpr char kObsoleteCachedSuperReferralComponentData[] =
    "brave.new_tab_page.cached_super_referral_component_data";
constexpr char kObsoleteGetInitialSRComponentInProgress[] =
    "brave.new_tab_page.get_initial_sr_component_in_progress";
constexpr char kObsoleteCachedSuperReferralCode[] =
    "brave.new_tab_page.cached_referral_code";

}  // namespace

namespace ntp_background_images {

// static
void ViewCounterService::RegisterProfilePrefs(
    user_prefs::PrefRegistrySyncable* registry) {
  registry->RegisterBooleanPref(prefs::kNewTabPageShowBackgroundImage, true);
}

// static
void ViewCounterService::RegisterProfilePrefsForMigration(
    user_prefs::PrefRegistrySyncable* registry) {
  // Added 09/2023
  registry->RegisterIntegerPref(kObsoleteCountToBrandedWallpaperPref, 0);

  // FlyWeb: added 11/2026 (T5).
  registry->RegisterBooleanPref(kObsoleteBrandedWallpaperNotificationDismissed,
                                false);
  registry->RegisterBooleanPref(
      kObsoleteNewTabPageShowSponsoredImagesBackgroundImage, true);
  registry->RegisterIntegerPref(kObsoleteNewTabPageSuperReferralThemesOption,
                                1);
}

// static
void ViewCounterService::MigrateObsoleteProfilePrefs(PrefService* prefs) {
  // Added 09/2023
  prefs->ClearPref(kObsoleteCountToBrandedWallpaperPref);

  // FlyWeb: added 11/2026 (T5).
  prefs->ClearPref(kObsoleteBrandedWallpaperNotificationDismissed);
  prefs->ClearPref(kObsoleteNewTabPageShowSponsoredImagesBackgroundImage);
  prefs->ClearPref(kObsoleteNewTabPageSuperReferralThemesOption);
}

// static
void ViewCounterService::RegisterLocalStatePrefsForMigration(
    PrefRegistrySimple* registry) {
  // FlyWeb: added 11/2026 (T5).
  registry->RegisterListPref(kObsoleteNewTabsCreated);
  registry->RegisterListPref(kObsoleteSponsoredNewTabsCreated);
  registry->RegisterDictionaryPref(kObsoleteCachedSuperReferralComponentInfo);
  registry->RegisterStringPref(kObsoleteCachedSuperReferralComponentData,
                               std::string());
  registry->RegisterStringPref(kObsoleteCachedSuperReferralCode,
                               std::string());
  registry->RegisterBooleanPref(kObsoleteGetInitialSRComponentInProgress,
                                false);
}

// static
void ViewCounterService::MigrateObsoleteLocalStatePrefs(
    PrefService* local_state) {
  // FlyWeb: added 11/2026 (T5).
  local_state->ClearPref(kObsoleteNewTabsCreated);
  local_state->ClearPref(kObsoleteSponsoredNewTabsCreated);
  local_state->ClearPref(kObsoleteCachedSuperReferralComponentInfo);
  local_state->ClearPref(kObsoleteCachedSuperReferralComponentData);
  local_state->ClearPref(kObsoleteCachedSuperReferralCode);
  local_state->ClearPref(kObsoleteGetInitialSRComponentInProgress);
}

ViewCounterService::ViewCounterService(
    BraveNTPCustomBackgroundService* custom_service,
    PrefService* prefs)
    : prefs_(prefs), custom_bi_service_(custom_service) {}

ViewCounterService::~ViewCounterService() = default;

std::optional<base::DictValue> ViewCounterService::GetCurrentWallpaper() const {
  if (!prefs_->GetBoolean(prefs::kNewTabPageShowBackgroundImage)) {
    return std::nullopt;
  }

#if BUILDFLAG(ENABLE_CUSTOM_BACKGROUND)
  if (ShouldShowCustomBackground()) {
    if (auto background = custom_bi_service_->GetBackground();
        !background.empty()) {
      return background;
    }
  }
#endif

  return std::nullopt;
}

bool ViewCounterService::ShouldShowCustomBackground() const {
#if BUILDFLAG(ENABLE_CUSTOM_BACKGROUND)
  return custom_bi_service_ && custom_bi_service_->ShouldShowCustomBackground();
#else
  return false;
#endif
}

}  // namespace ntp_background_images

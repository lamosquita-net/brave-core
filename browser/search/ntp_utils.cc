/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/browser/search/ntp_utils.h"

#include "brave/browser/profiles/profile_util.h"
#include "brave/components/constants/pref_names.h"
#include "chrome/browser/new_tab_page/prefs/ntp_pref_names.h"
#include "components/pref_registry/pref_registry_syncable.h"
#include "components/prefs/pref_registry_simple.h"
#include "components/prefs/pref_service.h"

namespace {

// FlyWeb: added 11/2026 (T5). The Rewards and Talk cards and the toggle that
// hid every card are gone from the new tab page.
constexpr char kObsoleteNewTabPageShowRewards[] =
    "brave.new_tab_page.show_rewards";
constexpr char kObsoleteNewTabPageShowBraveTalk[] =
    "brave.new_tab_page.show_together";
constexpr char kObsoleteNewTabPageHideAllWidgets[] =
    "brave.new_tab_page.hide_all_widgets";

void ClearNewTabPageProfilePrefs(PrefService* prefs) {
  prefs->ClearPref(kNewTabPageShowTopSites);
  prefs->ClearPref(kObsoleteNewTabPageShowRewards);
  prefs->ClearPref(kObsoleteNewTabPageShowBraveTalk);
  prefs->ClearPref(kObsoleteNewTabPageHideAllWidgets);
}

}  // namespace

namespace new_tab_page {

void MigrateNewTabPagePrefs(PrefService* prefs) {
  // Migrate over to the Chromium setting for shortcuts visible
  // Only sets the value if user has changed it
  const PrefService::Preference* top_sites_pref =
      prefs->FindPreference(kNewTabPageShowTopSites);
  if (top_sites_pref->HasUserSetting()) {
    prefs->SetBoolean(ntp_prefs::kNtpShortcutsVisible,
                      prefs->GetBoolean(kNewTabPageShowTopSites));
  }

  // Clear deprecated prefs.
  ClearNewTabPageProfilePrefs(prefs);
}

void RegisterNewTabPagePrefsForMigration(
    user_prefs::PrefRegistrySyncable* registry) {
  registry->RegisterBooleanPref(kNewTabPageShowTopSites, true);
  registry->RegisterBooleanPref(kObsoleteNewTabPageShowRewards, true);
  registry->RegisterBooleanPref(kObsoleteNewTabPageShowBraveTalk, false);
  registry->RegisterBooleanPref(kObsoleteNewTabPageHideAllWidgets, false);
}

}  // namespace new_tab_page

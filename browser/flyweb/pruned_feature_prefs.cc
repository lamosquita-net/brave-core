/* Copyright (c) 2026 lamosquita. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "brave/browser/flyweb/pruned_feature_prefs.h"

#include "base/time/time.h"
#include "components/pref_registry/pref_registry_syncable.h"
#include "components/prefs/pref_registry_simple.h"
#include "components/prefs/pref_service.h"

namespace flyweb {

namespace {

// Playlist (removed 2026-11).
constexpr char kPlaylistEnabled[] = "brave.playlist.enabled";
constexpr char kPlaylists[] = "brave.playlist.lists";
constexpr char kPlaylistOrder[] = "brave.playlist.lists_order";
constexpr char kPlaylistItems[] = "brave.playlist.items";
constexpr char kPlaylistCacheByDefault[] = "brave.playlist.cache";
constexpr char kPlaylistDefaultSaveTargetListID[] =
    "brave.playlist.default_save_target_list_id";
constexpr char kPlaylistFirstUsageTime[] = "brave.playlist.first_usage_time";
constexpr char kPlaylistLastUsageTime[] = "brave.playlist.last_usage_time";
constexpr char kPlaylistUsedSecondDay[] = "brave.playlist.used_second_day";
constexpr char kPlaylistUsageWeeklyStorage[] =
    "brave.playlist.usage_weekly_storage";

constexpr const char* kProfilePrefs[] = {
    kPlaylistEnabled,
    kPlaylists,
    kPlaylistOrder,
    kPlaylistItems,
    kPlaylistCacheByDefault,
    kPlaylistDefaultSaveTargetListID,
};

constexpr const char* kLocalStatePrefs[] = {
    kPlaylistFirstUsageTime,
    kPlaylistLastUsageTime,
    kPlaylistUsedSecondDay,
    kPlaylistUsageWeeklyStorage,
};

}  // namespace

void RegisterPrunedFeatureProfilePrefs(
    user_prefs::PrefRegistrySyncable* registry) {
  // Playlist
  registry->RegisterBooleanPref(kPlaylistEnabled, true);
  registry->RegisterDictionaryPref(kPlaylists);
  registry->RegisterListPref(kPlaylistOrder);
  registry->RegisterDictionaryPref(kPlaylistItems);
  registry->RegisterBooleanPref(kPlaylistCacheByDefault, true);
  registry->RegisterStringPref(kPlaylistDefaultSaveTargetListID, "");
}

void ClearPrunedFeatureProfilePrefs(PrefService* profile_prefs) {
  for (const char* pref : kProfilePrefs) {
    profile_prefs->ClearPref(pref);
  }
}

void RegisterPrunedFeatureLocalStatePrefs(PrefRegistrySimple* registry) {
  // Playlist
  registry->RegisterTimePref(kPlaylistFirstUsageTime, base::Time());
  registry->RegisterTimePref(kPlaylistLastUsageTime, base::Time());
  registry->RegisterBooleanPref(kPlaylistUsedSecondDay, false);
  registry->RegisterListPref(kPlaylistUsageWeeklyStorage);
}

void ClearPrunedFeatureLocalStatePrefs(PrefService* local_state) {
  for (const char* pref : kLocalStatePrefs) {
    local_state->ClearPref(pref);
  }
}

}  // namespace flyweb

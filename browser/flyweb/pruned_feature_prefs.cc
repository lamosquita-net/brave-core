/* Copyright (c) 2026 lamosquita. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "brave/browser/flyweb/pruned_feature_prefs.h"

#include "base/time/time.h"
#include "build/build_config.h"
#include "chrome/common/pref_names.h"
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

// Speedreader (removed 2026-11), including its content setting type.
constexpr char kSpeedreaderEnabled[] = "brave.speedreader.enabled";
constexpr char kSpeedreaderEverEnabled[] = "brave.speedreader.ever_enabled";
constexpr char kSpeedreaderToggleCount[] = "brave.speedreader.toggle_count";
constexpr char kSpeedreaderPromptCount[] = "brave.speedreader.prompt_count";
constexpr char kSpeedreaderTheme[] = "brave.speedreader.theme";
constexpr char kSpeedreaderFontSize[] = "brave.speedreader.font_size";
constexpr char kSpeedreaderFontFamily[] = "brave.speedreader.font_family";
constexpr char kSpeedreaderColumnWidth[] = "brave.speedreader.column_width";
constexpr char kSpeedreaderTtsVoice[] = "brave.speedreader.tts_voice";
constexpr char kSpeedreaderTtsSpeed[] = "brave.speedreader.tts_speed";
constexpr char kSpeedreaderContentSettingExceptions[] =
    "profile.content_settings.exceptions.braveSpeedreader";
constexpr char kSpeedreaderContentSettingPartitionedExceptions[] =
    "profile.content_settings.partitioned_exceptions.braveSpeedreader";
constexpr char kSpeedreaderContentSettingDefault[] =
    "profile.default_content_setting_values.braveSpeedreader";

// WebTorrent (removed 2026-11).
constexpr char kWebTorrentEnabled[] = "brave.webtorrent_enabled";

// Wayback Machine (removed 2026-11).
constexpr char kWaybackMachineEnabled[] = "brave.wayback_machine_enabled";

// Commander (removed 2026-11).
constexpr char kCommanderSuggestionsEnabled[] =
    "brave.omnibox.commander_suggestions_enabled";

// Brave Sidebar (removed 2026-11). brave.sidebar.side_panel_width stays: the
// side panel still uses it.
constexpr char kSidebarItems[] = "brave.sidebar.sidebar_items";
constexpr char kSidebarHiddenBuiltInItems[] =
    "brave.sidebar.hidden_built_in_items";
constexpr char kSidebarLeoPanelOneShotOpen[] =
    "brave.sidebar.leo_panel_one_shot_shot";
constexpr char kSidebarShowOption[] = "brave.sidebar.sidebar_show_option";
constexpr char kSidebarItemAddedFeedbackBubbleShowCount[] =
    "brave.sidebar.item_added_feedback_bubble_shown_count";
constexpr char kSidebarLastUsedBuiltInItemType[] =
    "brave.sidebar.last_used_built_in_item_type";
constexpr char kSidebarSettingChangeInitialP3AReport[] =
    "brave.sidebar.setting_change_initial_p3a_reported";
constexpr char kSidebarAlignmentChangedTemporarily[] =
    "brave.sidebar.sidebar_alignment_changed_for_vertical_tabs";
constexpr char kShowSidePanelButton[] = "brave.show_side_panel_button";
constexpr char kTargetUserForSidebarEnabledTest[] =
    "brave.sidebar.target_user_for_sidebar_enabled_test";

// Brave vertical tabs (removed 2026-11; Chromium's native vertical tabs
// replace them).
constexpr char kVerticalTabsEnabled[] = "brave.tabs.vertical_tabs_enabled";
constexpr char kVerticalTabsCollapsed[] = "brave.tabs.vertical_tabs_collapsed";
constexpr char kVerticalTabsShowTitleOnWindow[] =
    "brave.tabs.vertical_tabs_show_title_on_window";
constexpr char kVerticalTabsFloatingEnabled[] =
    "brave.tabs.vertical_tabs_floating_enabled";
constexpr char kVerticalTabsExpandedWidth[] =
    "brave.tabs.vertical_tabs_expanded_width";
constexpr char kVerticalTabsOnRight[] = "brave.tabs.vertical_tabs_on_right";
constexpr char kVerticalTabsShowScrollbar[] =
    "brave.tabs.vertical_tabs_show_scrollbar";

// Brave Wallet (code removed earlier; its leftovers pruned 2026-11). Every
// Wallet pref, in the profile (keyrings, encrypted mnemonic, networks,
// assets, transactions, default wallets, toolbar button, auto-lock...) and in
// local state (unlock times, P3A), lives under "brave.wallet", and nothing
// else FlyWeb keeps does, so the whole dictionary is registered and cleared
// at once instead of one pref per key.
constexpr char kBraveWallet[] = "brave.wallet";

constexpr const char* kProfilePrefs[] = {
    kPlaylistEnabled,
    kPlaylists,
    kPlaylistOrder,
    kPlaylistItems,
    kPlaylistCacheByDefault,
    kPlaylistDefaultSaveTargetListID,
    kSpeedreaderEnabled,
    kSpeedreaderEverEnabled,
    kSpeedreaderToggleCount,
    kSpeedreaderPromptCount,
    kSpeedreaderTheme,
    kSpeedreaderFontSize,
    kSpeedreaderFontFamily,
    kSpeedreaderColumnWidth,
    kSpeedreaderTtsVoice,
    kSpeedreaderTtsSpeed,
    kSpeedreaderContentSettingExceptions,
    kSpeedreaderContentSettingPartitionedExceptions,
    kSpeedreaderContentSettingDefault,
    kWebTorrentEnabled,
    kWaybackMachineEnabled,
    kCommanderSuggestionsEnabled,
    kSidebarItems,
    kSidebarHiddenBuiltInItems,
    kSidebarLeoPanelOneShotOpen,
    kSidebarShowOption,
    kSidebarItemAddedFeedbackBubbleShowCount,
    kSidebarLastUsedBuiltInItemType,
    kSidebarSettingChangeInitialP3AReport,
    kSidebarAlignmentChangedTemporarily,
    kShowSidePanelButton,
    kVerticalTabsEnabled,
    kVerticalTabsCollapsed,
    kVerticalTabsShowTitleOnWindow,
    kVerticalTabsFloatingEnabled,
    kVerticalTabsExpandedWidth,
    kVerticalTabsOnRight,
    kVerticalTabsShowScrollbar,
    kBraveWallet,
};

constexpr const char* kLocalStatePrefs[] = {
    kPlaylistFirstUsageTime,
    kPlaylistLastUsageTime,
    kPlaylistUsedSecondDay,
    kPlaylistUsageWeeklyStorage,
    kTargetUserForSidebarEnabledTest,
    kBraveWallet,
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

  // Speedreader
  registry->RegisterBooleanPref(kSpeedreaderEnabled, false);
  registry->RegisterBooleanPref(kSpeedreaderEverEnabled, false);
  registry->RegisterListPref(kSpeedreaderToggleCount);
  registry->RegisterIntegerPref(kSpeedreaderPromptCount, 0);
  registry->RegisterIntegerPref(kSpeedreaderTheme, 0);
  registry->RegisterIntegerPref(kSpeedreaderFontSize, 0);
  registry->RegisterIntegerPref(kSpeedreaderFontFamily, 0);
  registry->RegisterIntegerPref(kSpeedreaderColumnWidth, 0);
  registry->RegisterStringPref(kSpeedreaderTtsVoice, "");
  registry->RegisterIntegerPref(kSpeedreaderTtsSpeed, 0);
  registry->RegisterDictionaryPref(kSpeedreaderContentSettingExceptions);
  registry->RegisterDictionaryPref(
      kSpeedreaderContentSettingPartitionedExceptions);
  registry->RegisterIntegerPref(kSpeedreaderContentSettingDefault, 0);

  // WebTorrent
  registry->RegisterBooleanPref(kWebTorrentEnabled, true);

  // Wayback Machine
  registry->RegisterBooleanPref(kWaybackMachineEnabled, true);

  // Commander
  registry->RegisterBooleanPref(kCommanderSuggestionsEnabled, true);

  // Brave Sidebar
  registry->RegisterListPref(kSidebarItems);
  registry->RegisterListPref(kSidebarHiddenBuiltInItems);
  registry->RegisterBooleanPref(kSidebarLeoPanelOneShotOpen, false);
  registry->RegisterIntegerPref(kSidebarShowOption, 0);
  registry->RegisterIntegerPref(kSidebarItemAddedFeedbackBubbleShowCount, 0);
  registry->RegisterIntegerPref(kSidebarLastUsedBuiltInItemType, 0);
  registry->RegisterBooleanPref(kSidebarSettingChangeInitialP3AReport, false);
  registry->RegisterBooleanPref(kSidebarAlignmentChangedTemporarily, false);
  registry->RegisterBooleanPref(kShowSidePanelButton, true);

  // Brave vertical tabs
  registry->RegisterBooleanPref(kVerticalTabsEnabled, false);
  registry->RegisterBooleanPref(kVerticalTabsCollapsed, false);
  registry->RegisterBooleanPref(kVerticalTabsShowTitleOnWindow, false);
  registry->RegisterBooleanPref(kVerticalTabsFloatingEnabled, true);
  registry->RegisterIntegerPref(kVerticalTabsExpandedWidth, 220);
  registry->RegisterBooleanPref(kVerticalTabsOnRight, false);
  registry->RegisterBooleanPref(kVerticalTabsShowScrollbar, false);

  // Brave Wallet
  registry->RegisterDictionaryPref(kBraveWallet);
}

void ClearPrunedFeatureProfilePrefs(PrefService* profile_prefs) {
#if !BUILDFLAG(IS_ANDROID)
  // Users of Brave's vertical tabs get Chromium's native ones instead.
  if (profile_prefs->GetBoolean(kVerticalTabsEnabled)) {
    profile_prefs->SetBoolean(prefs::kVerticalTabsEnabled, true);
  }
#endif  // !BUILDFLAG(IS_ANDROID)

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

  // Brave Sidebar
  registry->RegisterBooleanPref(kTargetUserForSidebarEnabledTest, false);

  // Brave Wallet
  registry->RegisterDictionaryPref(kBraveWallet);
}

void ClearPrunedFeatureLocalStatePrefs(PrefService* local_state) {
  for (const char* pref : kLocalStatePrefs) {
    local_state->ClearPref(pref);
  }
}

}  // namespace flyweb

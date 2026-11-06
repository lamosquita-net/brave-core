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

// Brave Sidebar (removed 2026-11).
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

// Brave's resizable side panel (removed 2026-11; Chromium's native side panel
// keeps its own width).
constexpr char kSidePanelWidth[] = "brave.sidebar.side_panel_width";

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

// Brave's What's New tab (removed 2026-11).
constexpr char kWhatsNewLastVersion[] = "brave.whats_new.last_version";

// Shields' cookie list opt-in bubble (removed 2026-11).
constexpr char kAdBlockCookieListOptInShown[] =
    "brave.ad_block.cookie_list_opt_in_shown";

// Web Discovery Project (removed 2026-11): search telemetry sent to Brave.
constexpr char kWebDiscoveryEnabled[] = "brave.web_discovery_enabled";
constexpr char kWebDiscoveryCTAState[] = "brave.web_discovery.cta_state";
constexpr char kDontAskEnableWebDiscovery[] =
    "brave.dont_ask_enable_web_discovery";

// Brave Wallet (code removed earlier; its leftovers pruned 2026-11). Every
// Wallet pref, in the profile (keyrings, encrypted mnemonic, networks,
// assets, transactions, default wallets, toolbar button, auto-lock...) and in
// local state (unlock times, P3A), lives under "brave.wallet", and nothing
// else FlyWeb keeps does, so the whole dictionary is registered and cleared
// at once instead of one pref per key.
constexpr char kBraveWallet[] = "brave.wallet";

// Brave Rewards (code removed earlier; its leftovers pruned 2026-11). Its
// profile prefs (wallet seed, external wallets, notifications, P3A counts...)
// live under "brave.rewards", plus two older toolbar button prefs.
constexpr char kBraveRewards[] = "brave.rewards";
constexpr char kBraveRewardsShowButton[] = "brave.show_brave_rewards_button";
constexpr char kBraveRewardsHideButton[] = "brave.hide_brave_rewards_button";

// Brave Search conversion promotion (removed 2026-11): the omnibox banner and
// the NTP promo. Its profile prefs (dismissed, maybe later, DDG banner) and
// its P3A local state prefs all live under "brave.brave_search_conversion".
// The NTP search box keeps its own "brave.brave_search" prefs.
constexpr char kBraveSearchConversion[] = "brave.brave_search_conversion";

// Brave's fixed top sites suggestions in the omnibox (TopSitesProvider,
// removed 2026-11).
constexpr char kTopSiteSuggestionsEnabled[] =
    "brave.top_site_suggestions_enabled";

// P3A counters whose metrics are gone (removed 2026-11): the omnibox weekly
// search count, the default search engine switches, Request-OTR's
// interstitial and session counts (all under "brave.request_otr.p3a") and
// the perf predictor's daily bandwidth savings.
constexpr char kOmniboxSearchCountP3A[] = "brave.weekly_storage.search_count";
constexpr char kSearchEngineSwitchP3A[] = "brave.search.p3a_default_switch";
constexpr char kRequestOTRP3A[] = "brave.request_otr.p3a";
constexpr char kPerfPredictorBandwidthSavedP3A[] =
    "brave.stats.daily_saving_predictions_bytes";

// FlyWeb: P3A removed (3.0.0); clear stored values. Remove after 3.1.
// Shields P3A: the usage status (local state), the first-report markers and
// the per-domain setting counts (profile).
constexpr char kShieldsUsageP3A[] = "brave_shields.p3a_usage";
constexpr char kShieldsFirstReportedP3A[] =
    "brave_shields.p3a_first_reported_v2";
constexpr char kShieldsFirstReportedRevisionP3A[] =
    "brave_shields.p3a_first_reported_revision";
constexpr char kShieldsAdsStrictCountP3A[] =
    "brave_shields.p3a_ads_strict_domain_count";
constexpr char kShieldsAdsStandardCountP3A[] =
    "brave_shields.p3a_ads_standard_domain_count";
constexpr char kShieldsAdsAllowCountP3A[] =
    "brave_shields.p3a_ads_allow_domain_count";
constexpr char kShieldsFPStrictCountP3A[] =
    "brave_shields.p3a_fp_strict_domain_count";
constexpr char kShieldsFPStandardCountP3A[] =
    "brave_shields.p3a_fp_standard_domain_count";
constexpr char kShieldsFPAllowCountP3A[] =
    "brave_shields.p3a_fp_allow_domain_count";

// Brave's localhost access permission (removed 2026-11; Chromium's native
// Local Network Access replaces it and the old grants are not carried over):
// its content setting type.
constexpr char kLocalhostAccessContentSettingExceptions[] =
    "profile.content_settings.exceptions.brave_localhost_access";
constexpr char kLocalhostAccessContentSettingPartitionedExceptions[] =
    "profile.content_settings.partitioned_exceptions.brave_localhost_access";
constexpr char kLocalhostAccessContentSettingDefault[] =
    "profile.default_content_setting_values.brave_localhost_access";

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
    kSidePanelWidth,
    kVerticalTabsEnabled,
    kVerticalTabsCollapsed,
    kVerticalTabsShowTitleOnWindow,
    kVerticalTabsFloatingEnabled,
    kVerticalTabsExpandedWidth,
    kVerticalTabsOnRight,
    kVerticalTabsShowScrollbar,
    kWebDiscoveryEnabled,
    kWebDiscoveryCTAState,
    kDontAskEnableWebDiscovery,
    kBraveWallet,
    kBraveRewards,
    kBraveRewardsShowButton,
    kBraveRewardsHideButton,
    kBraveSearchConversion,
    kOmniboxSearchCountP3A,
    kSearchEngineSwitchP3A,
    kRequestOTRP3A,
    kPerfPredictorBandwidthSavedP3A,
    kTopSiteSuggestionsEnabled,
    kShieldsFirstReportedP3A,
    kShieldsFirstReportedRevisionP3A,
    kShieldsAdsStrictCountP3A,
    kShieldsAdsStandardCountP3A,
    kShieldsAdsAllowCountP3A,
    kShieldsFPStrictCountP3A,
    kShieldsFPStandardCountP3A,
    kShieldsFPAllowCountP3A,
    kLocalhostAccessContentSettingExceptions,
    kLocalhostAccessContentSettingPartitionedExceptions,
    kLocalhostAccessContentSettingDefault,
};

constexpr const char* kLocalStatePrefs[] = {
    kPlaylistFirstUsageTime,
    kPlaylistLastUsageTime,
    kPlaylistUsedSecondDay,
    kPlaylistUsageWeeklyStorage,
    kTargetUserForSidebarEnabledTest,
    kWhatsNewLastVersion,
    kAdBlockCookieListOptInShown,
    kBraveWallet,
    kBraveSearchConversion,
    kShieldsUsageP3A,
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

  // Brave's resizable side panel
  registry->RegisterIntegerPref(kSidePanelWidth, 320);

  // Brave vertical tabs
  registry->RegisterBooleanPref(kVerticalTabsEnabled, false);
  registry->RegisterBooleanPref(kVerticalTabsCollapsed, false);
  registry->RegisterBooleanPref(kVerticalTabsShowTitleOnWindow, false);
  registry->RegisterBooleanPref(kVerticalTabsFloatingEnabled, true);
  registry->RegisterIntegerPref(kVerticalTabsExpandedWidth, 220);
  registry->RegisterBooleanPref(kVerticalTabsOnRight, false);
  registry->RegisterBooleanPref(kVerticalTabsShowScrollbar, false);

  // Web Discovery Project
  registry->RegisterBooleanPref(kWebDiscoveryEnabled, false);
  registry->RegisterDictionaryPref(kWebDiscoveryCTAState);
  registry->RegisterBooleanPref(kDontAskEnableWebDiscovery, false);

  // Brave Wallet
  registry->RegisterDictionaryPref(kBraveWallet);

  // Brave Rewards
  registry->RegisterDictionaryPref(kBraveRewards);
  registry->RegisterBooleanPref(kBraveRewardsShowButton, true);
  registry->RegisterBooleanPref(kBraveRewardsHideButton, false);

  // Brave Search conversion promotion
  registry->RegisterDictionaryPref(kBraveSearchConversion);

  // Orphaned P3A counters
  registry->RegisterListPref(kOmniboxSearchCountP3A);
  registry->RegisterListPref(kSearchEngineSwitchP3A);
  registry->RegisterDictionaryPref(kRequestOTRP3A);
  registry->RegisterListPref(kPerfPredictorBandwidthSavedP3A);

  // TopSitesProvider
  registry->RegisterBooleanPref(kTopSiteSuggestionsEnabled, true);

  // Shields P3A
  registry->RegisterBooleanPref(kShieldsFirstReportedP3A, false);
  registry->RegisterIntegerPref(kShieldsFirstReportedRevisionP3A, 0);
  registry->RegisterIntegerPref(kShieldsAdsStrictCountP3A, 0);
  registry->RegisterIntegerPref(kShieldsAdsStandardCountP3A, 0);
  registry->RegisterIntegerPref(kShieldsAdsAllowCountP3A, 0);
  registry->RegisterIntegerPref(kShieldsFPStrictCountP3A, 0);
  registry->RegisterIntegerPref(kShieldsFPStandardCountP3A, 0);
  registry->RegisterIntegerPref(kShieldsFPAllowCountP3A, 0);

  // Brave's localhost access permission
  registry->RegisterDictionaryPref(kLocalhostAccessContentSettingExceptions);
  registry->RegisterDictionaryPref(
      kLocalhostAccessContentSettingPartitionedExceptions);
  registry->RegisterIntegerPref(kLocalhostAccessContentSettingDefault, 0);
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

  // Brave's What's New tab
  registry->RegisterDoublePref(kWhatsNewLastVersion, 0);

  // Shields' cookie list opt-in bubble
  registry->RegisterBooleanPref(kAdBlockCookieListOptInShown, false);

  // Brave Wallet
  registry->RegisterDictionaryPref(kBraveWallet);

  // Brave Search conversion promotion
  registry->RegisterDictionaryPref(kBraveSearchConversion);

  // Shields P3A
  registry->RegisterIntegerPref(kShieldsUsageP3A, -1);
}

void ClearPrunedFeatureLocalStatePrefs(PrefService* local_state) {
  for (const char* pref : kLocalStatePrefs) {
    local_state->ClearPref(pref);
  }
}

}  // namespace flyweb

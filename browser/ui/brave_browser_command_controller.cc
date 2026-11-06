/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/browser/ui/brave_browser_command_controller.h"

#include <memory>
#include <optional>
#include <utility>

#include "base/debug/stack_trace.h"
#include "base/feature_list.h"
#include "base/notreached.h"
#include "base/types/to_address.h"
#include "brave/app/brave_command_ids.h"
#include "brave/browser/profiles/profile_util.h"
#include "brave/browser/ui/brave_pages.h"
#include "brave/browser/ui/browser_commands.h"
#include "brave/components/commands/common/features.h"
#include "brave/components/constants/pref_names.h"
#include "chrome/app/chrome_command_ids.h"
#include "chrome/browser/browser_process.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/browser/send_tab_to_self/send_tab_to_self_util.h"
#include "chrome/browser/ui/browser_command_controller.h"
#include "chrome/browser/ui/browser_commands.h"
#include "chrome/browser/ui/browser_window/public/browser_window_interface.h"
#include "chrome/browser/ui/tabs/tab_change_type.h"
#include "chrome/browser/ui/tabs/tab_strip_model.h"
#include "chrome/browser/ui/tabs/tab_strip_model_observer.h"
#include "chrome/common/pref_names.h"
#include "components/prefs/pref_service.h"
#include "components/sync/base/command_line_switches.h"
#include "content/public/browser/web_contents.h"

namespace {

bool IsBraveCommands(int id) {
  return id >= IDC_BRAVE_COMMANDS_START && id <= IDC_BRAVE_COMMANDS_LAST;
}

}  // namespace

namespace chrome {

BraveBrowserCommandController::BraveBrowserCommandController(
    BrowserWindowInterface* browser)
    : BrowserCommandController(browser),
      browser_(*browser),
      brave_command_updater_(nullptr) {
  InitBraveCommandState();
}

BraveBrowserCommandController::~BraveBrowserCommandController() = default;

void BraveBrowserCommandController::OnTabChangedAt(tabs::TabInterface* tab,
                                                   TabChangeType change_type) {
  BrowserCommandController::OnTabChangedAt(tab, change_type);
  UpdateCommandEnabled(IDC_CLOSE_DUPLICATE_TABS,
                       brave::HasDuplicateTabs(&*browser_));
  UpdateCommandsForTabs();
  UpdateCommandsForSend();
}

void BraveBrowserCommandController::OnTabPinnedStateChanged(
    tabs::TabInterface* tab,
    int index) {
  BrowserCommandController::OnTabPinnedStateChanged(tab, index);
  UpdateCommandsForPin();
}

void BraveBrowserCommandController::OnTabStripModelChanged(
    TabStripModel* tab_strip_model,
    const TabStripModelChange& change,
    const TabStripSelectionChange& selection) {
  BrowserCommandController::OnTabStripModelChanged(tab_strip_model, change,
                                                   selection);

  UpdateCommandEnabled(IDC_WINDOW_CLOSE_TABS_TO_LEFT,
                       brave::CanCloseTabsToLeft(&*browser_));
  UpdateCommandEnabled(IDC_CLOSE_DUPLICATE_TABS,
                       brave::HasDuplicateTabs(&*browser_));
  UpdateCommandsForTabs();
  UpdateCommandsForSend();
  UpdateCommandsForPin();
}

void BraveBrowserCommandController::OnTabGroupChanged(
    const TabGroupChange& change) {
  UpdateCommandsForTabs();
}

bool BraveBrowserCommandController::SupportsCommand(int id) const {
  return IsBraveCommands(id) ? brave_command_updater_.SupportsCommand(id)
                             : BrowserCommandController::SupportsCommand(id);
}

bool BraveBrowserCommandController::IsCommandEnabled(int id) const {
  return IsBraveCommands(id) ? brave_command_updater_.IsCommandEnabled(id)
                             : BrowserCommandController::IsCommandEnabled(id);
}

bool BraveBrowserCommandController::ExecuteCommandWithDispositionAndContext(
    int id,
    WindowOpenDisposition disposition,
    std::optional<actions::ActionInvocationContext> context,
    base::TimeTicks time_stamp) {
  return IsBraveCommands(id)
             ? ExecuteBraveCommandWithDisposition(id, disposition, time_stamp)
             : BrowserCommandController::
                   ExecuteCommandWithDispositionAndContext(
                       id, disposition, std::move(context), time_stamp);
}

void BraveBrowserCommandController::AddCommandObserver(
    int id,
    CommandObserver* observer) {
  IsBraveCommands(id)
      ? brave_command_updater_.AddCommandObserver(id, observer)
      : BrowserCommandController::AddCommandObserver(id, observer);
}

void BraveBrowserCommandController::RemoveCommandObserver(
    int id,
    CommandObserver* observer) {
  IsBraveCommands(id)
      ? brave_command_updater_.RemoveCommandObserver(id, observer)
      : BrowserCommandController::RemoveCommandObserver(id, observer);
}

void BraveBrowserCommandController::RemoveCommandObserver(
    CommandObserver* observer) {
  brave_command_updater_.RemoveCommandObserver(observer);
  BrowserCommandController::RemoveCommandObserver(observer);
}

bool BraveBrowserCommandController::UpdateCommandEnabled(int id, bool state) {
  return IsBraveCommands(id)
             ? brave_command_updater_.UpdateCommandEnabled(id, state)
             : BrowserCommandController::UpdateCommandEnabled(id, state);
}

void BraveBrowserCommandController::InitBraveCommandState() {
  // Sync and Rewards pages don't work in tor(guest) sessions.
  // They also don't work in private windows but they are redirected
  // to a normal window in this case.
  const bool is_guest_session = browser_->GetProfile()->IsGuestSession();
  if (!is_guest_session) {
    if (syncer::IsSyncAllowedByFlag()) {
      UpdateCommandForBraveSync();
    }
  }
  UpdateCommandForWebcompatReporter();
  bool add_new_profile_enabled = !is_guest_session;
  bool open_guest_profile_enabled = !is_guest_session;
  if (!is_guest_session) {
    if (PrefService* local_state = g_browser_process->local_state()) {
      add_new_profile_enabled =
          local_state->GetBoolean(prefs::kBrowserAddPersonEnabled);
      open_guest_profile_enabled =
          local_state->GetBoolean(prefs::kBrowserGuestModeEnabled);
    }
  }
  UpdateCommandEnabled(IDC_ADD_NEW_PROFILE, add_new_profile_enabled);
  UpdateCommandEnabled(IDC_OPEN_GUEST_PROFILE, open_guest_profile_enabled);
  UpdateCommandEnabled(IDC_COPY_CLEAN_LINK, true);
  UpdateCommandEnabled(IDC_TOGGLE_TAB_MUTE, true);

  UpdateCommandEnabled(
      IDC_CONFIGURE_SHORTCUTS,
      base::FeatureList::IsEnabled(commands::features::kBraveCommands));

  UpdateCommandEnabled(IDC_SHOW_BRAVE_TALK, true);
  UpdateCommandEnabled(IDC_TOGGLE_SHIELDS, true);
  UpdateCommandEnabled(IDC_TOGGLE_JAVASCRIPT, true);

  UpdateCommandEnabled(IDC_CLOSE_DUPLICATE_TABS,
                       brave::HasDuplicateTabs(&*browser_));
  UpdateCommandEnabled(IDC_WINDOW_ADD_ALL_TABS_TO_NEW_GROUP, true);

  UpdateCommandEnabled(IDC_SCROLL_TAB_TO_TOP, true);
  UpdateCommandEnabled(IDC_SCROLL_TAB_TO_BOTTOM, true);

  UpdateCommandEnabled(IDC_BRAVE_SEND_TAB_TO_SELF, true);

  UpdateCommandsForTabs();
  UpdateCommandsForSend();
  UpdateCommandsForPin();

  UpdateCommandEnabled(IDC_TOGGLE_ALL_BOOKMARKS_BUTTON_VISIBILITY, true);

  if (browser_->GetType() == BrowserWindowInterface::TYPE_NORMAL) {
    // Delete these when upstream enables by default.
    UpdateCommandEnabled(IDC_READING_LIST_MENU, true);
    UpdateCommandEnabled(IDC_READING_LIST_MENU_ADD_TAB, true);
    UpdateCommandEnabled(IDC_READING_LIST_MENU_SHOW_UI, true);
  }
}

void BraveBrowserCommandController::UpdateCommandForBraveRewards() {
  UpdateCommandEnabled(IDC_SHOW_BRAVE_REWARDS, true);
}

void BraveBrowserCommandController::UpdateCommandForWebcompatReporter() {
  UpdateCommandEnabled(IDC_SHOW_BRAVE_WEBCOMPAT_REPORTER, true);
}

void BraveBrowserCommandController::UpdateCommandsForTabs() {
  UpdateCommandEnabled(IDC_WINDOW_MUTE_ALL_TABS,
                       brave::CanMuteAllTabs(&*browser_, false));
  UpdateCommandEnabled(IDC_WINDOW_MUTE_OTHER_TABS,
                       brave::CanMuteAllTabs(&*browser_, true));
  UpdateCommandEnabled(IDC_WINDOW_UNMUTE_ALL_TABS,
                       brave::CanUnmuteAllTabs(&*browser_));

  UpdateCommandEnabled(IDC_GROUP_TABS_ON_CURRENT_ORIGIN, true);
  UpdateCommandEnabled(IDC_MOVE_GROUP_TO_NEW_WINDOW, true);

  bool is_in_group = brave::IsInGroup(&*browser_);
  bool has_ungrouped_tabs = brave::HasUngroupedTabs(&*browser_);
  UpdateCommandEnabled(IDC_WINDOW_GROUP_UNGROUPED_TABS, has_ungrouped_tabs);
  UpdateCommandEnabled(IDC_WINDOW_UNGROUP_GROUP, is_in_group);
  UpdateCommandEnabled(IDC_WINDOW_REMOVE_TAB_FROM_GROUP, is_in_group);
  UpdateCommandEnabled(IDC_WINDOW_NAME_GROUP, is_in_group);
  UpdateCommandEnabled(IDC_WINDOW_NEW_TAB_IN_GROUP, is_in_group);
  UpdateCommandEnabled(IDC_WINDOW_UNGROUP_ALL_TABS,
                       brave::CanUngroupAllTabs(&*browser_));
  UpdateCommandEnabled(IDC_WINDOW_TOGGLE_GROUP_EXPANDED, is_in_group);
  UpdateCommandEnabled(IDC_WINDOW_CLOSE_UNGROUPED_TABS, has_ungrouped_tabs);
  UpdateCommandEnabled(IDC_WINDOW_CLOSE_TABS_NOT_IN_CURRENT_GROUP, is_in_group);
  UpdateCommandEnabled(IDC_WINDOW_CLOSE_GROUP, is_in_group);

  UpdateCommandEnabled(IDC_WINDOW_BRING_ALL_TABS,
                       brave::CanBringAllTabs(&*browser_));
}

void BraveBrowserCommandController::UpdateCommandsForSend() {
  UpdateCommandEnabled(
      IDC_BRAVE_SEND_TAB_TO_SELF,
      send_tab_to_self::ShouldDisplayEntryPoint(
          browser_->tab_strip_model()->GetActiveWebContents()));
}

void BraveBrowserCommandController::UpdateCommandsForPin() {
  UpdateCommandEnabled(IDC_WINDOW_CLOSE_UNPINNED_TABS,
                       brave::CanCloseUnpinnedTabs(&*browser_));
}

void BraveBrowserCommandController::UpdateCommandForBraveSync() {
  UpdateCommandEnabled(IDC_SHOW_BRAVE_SYNC, true);
}

bool BraveBrowserCommandController::ExecuteBraveCommandWithDisposition(
    int id,
    WindowOpenDisposition disposition,
    base::TimeTicks time_stamp) {
  if (!SupportsCommand(id) || !IsCommandEnabled(id)) {
    return false;
  }

  if (browser_->tab_strip_model()->active_index() == TabStripModel::kNoTab) {
    return true;
  }

  DCHECK(IsCommandEnabled(id)) << "Invalid/disabled command " << id;

  switch (id) {
    case IDC_SHOW_BRAVE_REWARDS:
      brave::ShowBraveRewards(&*browser_);
      break;
    case IDC_SHOW_BRAVE_WEBCOMPAT_REPORTER:
      brave::ShowWebcompatReporter(&*browser_);
      break;
    case IDC_SHOW_BRAVE_SYNC:
      brave::ShowSync(&*browser_);
      break;
    case IDC_TOGGLE_AI_CHAT:
      break;
    case IDC_COPY_CLEAN_LINK:
      brave::CopySanitizedURL(
          &*browser_,
          browser_->tab_strip_model()->GetActiveWebContents()->GetVisibleURL());
      break;
    case IDC_TOGGLE_TAB_MUTE:
      brave::ToggleActiveTabAudioMute(&*browser_);
      break;
    case IDC_CONFIGURE_SHORTCUTS:
      brave::ShowShortcutsPage(&*browser_);
      break;
    case IDC_SHOW_BRAVE_TALK:
      brave::ShowBraveTalk(&*browser_);
      break;
    case IDC_TOGGLE_SHIELDS:
      brave::ToggleShieldsEnabled(&*browser_);
      break;
    case IDC_TOGGLE_JAVASCRIPT:
      brave::ToggleJavascriptEnabled(&*browser_);
      break;
    case IDC_GROUP_TABS_ON_CURRENT_ORIGIN:
      brave::GroupTabsOnCurrentOrigin(&*browser_);
      break;
    case IDC_MOVE_GROUP_TO_NEW_WINDOW:
      brave::MoveGroupToNewWindow(&*browser_);
      break;
    case IDC_CLOSE_DUPLICATE_TABS:
      brave::CloseDuplicateTabs(&*browser_);
      break;
    case IDC_WINDOW_CLOSE_TABS_TO_LEFT:
      brave::CloseTabsToLeft(&*browser_);
      break;
    case IDC_WINDOW_CLOSE_UNPINNED_TABS:
      brave::CloseUnpinnedTabs(&*browser_);
      break;
    case IDC_WINDOW_ADD_ALL_TABS_TO_NEW_GROUP:
      brave::AddAllTabsToNewGroup(&*browser_);
      break;
    case IDC_WINDOW_MUTE_ALL_TABS:
      brave::MuteAllTabs(&*browser_, false);
      break;
    case IDC_WINDOW_MUTE_OTHER_TABS:
      brave::MuteAllTabs(&*browser_, true);
      break;
    case IDC_WINDOW_UNMUTE_ALL_TABS:
      brave::UnmuteAllTabs(&*browser_);
      break;
    case IDC_SCROLL_TAB_TO_TOP:
      brave::ScrollTabToTop(&*browser_);
      break;
    case IDC_SCROLL_TAB_TO_BOTTOM:
      brave::ScrollTabToBottom(&*browser_);
      break;
    case IDC_BRAVE_SEND_TAB_TO_SELF:
      chrome::SendTabToSelf(&*browser_);
      break;
    case IDC_TOGGLE_ALL_BOOKMARKS_BUTTON_VISIBILITY:
      brave::ToggleAllBookmarksButtonVisibility(base::to_address(browser_));
      break;
    case IDC_WINDOW_GROUP_UNGROUPED_TABS:
      brave::GroupUngroupedTabs(&*browser_);
      break;
    case IDC_WINDOW_UNGROUP_GROUP:
      brave::UngroupCurrentGroup(&*browser_);
      break;
    case IDC_WINDOW_REMOVE_TAB_FROM_GROUP:
      brave::RemoveTabFromGroup(&*browser_);
      break;
    case IDC_WINDOW_UNGROUP_ALL_TABS:
      brave::UngroupAllTabs(&*browser_);
      break;
    case IDC_WINDOW_NAME_GROUP:
      brave::NameGroup(&*browser_);
      break;
    case IDC_WINDOW_NEW_TAB_IN_GROUP:
      brave::NewTabInGroup(&*browser_);
      break;
    case IDC_WINDOW_TOGGLE_GROUP_EXPANDED:
      brave::ToggleGroupExpanded(&*browser_);
      break;
    case IDC_WINDOW_CLOSE_UNGROUPED_TABS:
      brave::CloseUngroupedTabs(&*browser_);
      break;
    case IDC_WINDOW_CLOSE_TABS_NOT_IN_CURRENT_GROUP:
      brave::CloseTabsNotInCurrentGroup(&*browser_);
      break;
    case IDC_WINDOW_CLOSE_GROUP:
      brave::CloseGroup(&*browser_);
      break;
    case IDC_WINDOW_BRING_ALL_TABS:
      brave::BringAllTabs(&*browser_);
      break;
    default:
      LOG(WARNING) << "Received Unimplemented Command: " << id;
      break;
  }

  return true;
}

}  // namespace chrome

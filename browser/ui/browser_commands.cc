/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/browser/ui/browser_commands.h"

#include <algorithm>
#include <memory>
#include <numeric>
#include <stack>
#include <string>
#include <utility>
#include <vector>

#include "base/feature_list.h"
#include "base/functional/callback_helpers.h"
#include "base/strings/utf_string_conversions.h"
#include "brave/app/brave_command_ids.h"
#include "brave/browser/brave_shields/brave_shields_tab_helper.h"
#include "brave/browser/debounce/debounce_service_factory.h"
#include "brave/browser/ui/bookmark/brave_bookmark_prefs.h"
#include "brave/browser/ui/brave_browser.h"
#include "brave/browser/ui/tabs/brave_tab_prefs.h"
#include "brave/browser/ui/tabs/brave_tab_strip_model.h"
#include "brave/browser/ui/tabs/features.h"
#include "brave/browser/url_sanitizer/url_sanitizer_service_factory.h"
#include "brave/components/constants/pref_names.h"
#include "brave/components/debounce/core/browser/debounce_service.h"
#include "brave/components/query_filter/utils.h"
#include "brave/components/url_sanitizer/browser/url_sanitizer_service.h"
#include "chrome/browser/browser_process.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/browser/profiles/profile_manager.h"
#include "chrome/browser/profiles/profile_metrics.h"
#include "chrome/browser/profiles/profile_window.h"
#include "chrome/browser/ui/browser_commands.h"
#include "chrome/browser/ui/browser_tabstrip.h"
#include "chrome/browser/ui/browser_window.h"
#include "chrome/browser/ui/browser_window/public/browser_window_features.h"
#include "chrome/browser/ui/browser_window/public/browser_window_interface.h"
#include "chrome/browser/ui/browser_window/public/global_browser_collection.h"
#include "chrome/browser/ui/navigator/browser_navigator_params.h"
#include "chrome/browser/ui/profiles/profile_picker.h"
#include "chrome/browser/ui/tabs/tab_enums.h"
#include "chrome/browser/ui/tabs/tab_group_model.h"
#include "chrome/browser/ui/tabs/tab_model.h"
#include "chrome/browser/ui/tabs/tab_strip_model.h"
#include "chrome/browser/ui/tabs/tab_utils.h"
#include "chrome/browser/ui/unload_controller.h"
#include "chrome/common/channel_info.h"
#include "chrome/common/pref_names.h"
#include "components/tab_groups/tab_group_visual_data.h"
#include "components/tabs/public/tab_group.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/web_contents.h"
#include "ui/base/clipboard/clipboard_buffer.h"
#include "ui/base/clipboard/scoped_clipboard_writer.h"
#include "url/origin.h"

#if defined(TOOLKIT_VIEWS)
#include "brave/browser/ui/views/frame/brave_browser_view.h"
#include "chrome/browser/ui/side_panel/side_panel_entry.h"
#include "chrome/browser/ui/side_panel/side_panel_entry_id.h"
#include "chrome/browser/ui/side_panel/side_panel_enums.h"
#include "chrome/browser/ui/side_panel/side_panel_ui.h"
#endif

using content::WebContents;

namespace brave {

namespace {

bool CanTakeTabs(const BrowserWindowInterface* from,
                 const BrowserWindowInterface* to) {
  return from != to && from->GetType() == BrowserWindowInterface::TYPE_NORMAL &&
         !UnloadController::From(from)->is_attempting_to_close_browser() &&
         !from->IsDeleteScheduled() && to->GetProfile() == from->GetProfile();
}

}  // namespace

void CopySanitizedURL(BrowserWindowInterface* browser, const GURL& url) {
  if (!browser || !browser->GetProfile()) {
    return;
  }
  GURL sanitized_url = brave::URLSanitizerServiceFactory::GetForBrowserContext(
                           browser->GetProfile())
                           ->SanitizeURL(url);

  ui::ScopedClipboardWriter scw(ui::ClipboardBuffer::kCopyPaste);
  scw.WriteText(base::UTF8ToUTF16(sanitized_url.spec()));
}

// Copies an url cleared through:
// - Debouncer (potentially debouncing many levels)
// - Query filter
// - URLSanitizerService
void CopyLinkWithStrictCleaning(BrowserWindowInterface* browser,
                                const GURL& url) {
  if (!browser || !browser->GetProfile()) {
    return;
  }
  DCHECK(url.SchemeIsHTTPOrHTTPS());
  GURL final_url;
  // Apply debounce rules.
  auto* debounce_service =
      debounce::DebounceServiceFactory::GetForBrowserContext(
          browser->GetProfile());
  if (debounce_service && !debounce_service->Debounce(url, &final_url)) {
    VLOG(1) << "Unable to apply debounce rules";
    final_url = url;
  }
  // Apply query filters.
  auto filtered_url = query_filter::ApplyQueryFilter(final_url);
  if (filtered_url.has_value()) {
    final_url = filtered_url.value();
  }
  // Sanitize url.
  final_url = brave::URLSanitizerServiceFactory::GetForBrowserContext(
                  browser->GetProfile())
                  ->SanitizeURL(final_url);

  ui::ScopedClipboardWriter scw(ui::ClipboardBuffer::kCopyPaste);
  scw.WriteText(base::UTF8ToUTF16(final_url.spec()));
}

void ToggleWindowTitleVisibilityForVerticalTabs(
    BrowserWindowInterface* browser) {
  auto* prefs = browser->GetProfile()->GetOriginalProfile()->GetPrefs();
  prefs->SetBoolean(
      brave_tabs::kVerticalTabsShowTitleOnWindow,
      !prefs->GetBoolean(brave_tabs::kVerticalTabsShowTitleOnWindow));
}

void ToggleVerticalTabStrip(BrowserWindowInterface* browser) {
  auto* profile = browser->GetProfile()->GetOriginalProfile();
  auto* prefs = profile->GetPrefs();
  const bool was_using_vertical_tab_strip =
      prefs->GetBoolean(brave_tabs::kVerticalTabsEnabled);
  prefs->SetBoolean(brave_tabs::kVerticalTabsEnabled,
                    !was_using_vertical_tab_strip);
}

void ToggleVerticalTabStripFloatingMode(BrowserWindowInterface* browser) {
  auto* prefs = browser->GetProfile()->GetOriginalProfile()->GetPrefs();
  prefs->SetBoolean(
      brave_tabs::kVerticalTabsFloatingEnabled,
      !prefs->GetBoolean(brave_tabs::kVerticalTabsFloatingEnabled));
}

void ToggleVerticalTabStripExpanded(BrowserWindowInterface* browser) {
  auto* prefs = browser->GetProfile()->GetPrefs();
  prefs->SetBoolean(brave_tabs::kVerticalTabsCollapsed,
                    !prefs->GetBoolean(brave_tabs::kVerticalTabsCollapsed));
}

void ToggleActiveTabAudioMute(BrowserWindowInterface* browser) {
  WebContents* contents = browser->tab_strip_model()->GetActiveWebContents();
  if (!contents || !contents->IsCurrentlyAudible()) {
    return;
  }

  bool mute_tab = !contents->IsAudioMuted();
  chrome::SetTabAudioMuted(contents, mute_tab, TabMutedReason::AUDIO_INDICATOR,
                           std::string());
}

bool HasSelectedURL(BrowserWindowInterface* browser) {
  if (!browser) {
    return false;
  }
  auto* brave_browser_window = BraveBrowserWindow::FromBrowser(browser);
  return brave_browser_window && brave_browser_window->HasSelectedURL();
}

void CleanAndCopySelectedURL(BrowserWindowInterface* browser) {
  if (!browser) {
    return;
  }
  auto* brave_browser_window = BraveBrowserWindow::FromBrowser(browser);
  if (brave_browser_window) {
    brave_browser_window->CleanAndCopySelectedURL();
  }
}

void ToggleShieldsEnabled(BrowserWindowInterface* browser) {
  if (!browser) {
    return;
  }

  auto* contents = browser->tab_strip_model()->GetActiveWebContents();
  if (!contents) {
    return;
  }
  auto* shields =
      brave_shields::BraveShieldsTabHelper::FromWebContents(contents);
  if (!shields) {
    return;
  }

  shields->SetBraveShieldsEnabled(!shields->GetBraveShieldsEnabled());
}

void ToggleJavascriptEnabled(BrowserWindowInterface* browser) {
  if (!browser) {
    return;
  }

  auto* contents = browser->tab_strip_model()->GetActiveWebContents();
  if (!contents) {
    return;
  }
  auto* shields =
      brave_shields::BraveShieldsTabHelper::FromWebContents(contents);
  if (!shields) {
    return;
  }

  shields->SetIsNoScriptEnabled(!shields->GetNoScriptEnabled());
}

void GroupTabsOnCurrentOrigin(BrowserWindowInterface* browser) {
  auto url =
      browser->tab_strip_model()->GetActiveWebContents()->GetVisibleURL();
  auto origin = url::Origin::Create(url);

  std::vector<int> group_indices;
  for (int index = 0; index < browser->tab_strip_model()->count(); ++index) {
    auto* tab = browser->tab_strip_model()->GetWebContentsAt(index);
    auto tab_origin = url::Origin::Create(tab->GetVisibleURL());
    if (origin.IsSameOriginWith(tab_origin)) {
      group_indices.push_back(index);
    }
  }
  auto group_id = browser->tab_strip_model()->AddToNewGroup(group_indices);
  auto* group =
      browser->tab_strip_model()->group_model()->GetTabGroup(group_id);

  auto data = *group->visual_data();
  data.SetTitle(base::UTF8ToUTF16(origin.host()));
  group->SetVisualData(data);
}

void MoveGroupToNewWindow(BrowserWindowInterface* browser) {
  auto* tsm = browser->tab_strip_model();
  auto current_group_id = tsm->GetTabGroupForTab(tsm->active_index());
  if (!current_group_id.has_value()) {
    return;
  }

  tsm->delegate()->MoveGroupToNewWindow(current_group_id.value());
}

bool IsInGroup(BrowserWindowInterface* browser) {
  if (!browser) {
    return false;
  }

  auto* tsm = browser->tab_strip_model();
  auto current_group_id = tsm->GetTabGroupForTab(tsm->active_index());
  return current_group_id.has_value();
}

bool HasUngroupedTabs(BrowserWindowInterface* browser) {
  if (!browser) {
    return false;
  }

  auto* tsm = browser->tab_strip_model();
  for (int i = 0; i < tsm->GetTabCount(); ++i) {
    if (!tsm->GetTabGroupForTab(i)) {
      return true;
    }
  }
  return false;
}

void GroupUngroupedTabs(BrowserWindowInterface* browser) {
  if (!browser) {
    return;
  }
  auto* tsm = browser->tab_strip_model();
  std::vector<int> group_indices;

  for (int i = 0; i < tsm->GetTabCount(); ++i) {
    if (tsm->GetTabGroupForTab(i)) {
      continue;
    }
    group_indices.push_back(i);
  }

  if (group_indices.empty()) {
    return;
  }

  tsm->AddToNewGroup(group_indices);
}

void UngroupCurrentGroup(BrowserWindowInterface* browser) {
  if (!browser) {
    return;
  }
  auto* tsm = browser->tab_strip_model();

  auto group_id = tsm->GetTabGroupForTab(tsm->active_index());
  if (!group_id) {
    return;
  }

  auto* group = tsm->group_model()->GetTabGroup(group_id.value());
  std::vector<int> indices(group->tab_count());
  std::iota(indices.begin(), indices.end(), group->GetFirstTab().value());
  tsm->RemoveFromGroup(indices);
}

void RemoveTabFromGroup(BrowserWindowInterface* browser) {
  if (!browser) {
    return;
  }
  auto* tsm = browser->tab_strip_model();
  tsm->RemoveFromGroup({tsm->active_index()});
}

void NameGroup(BrowserWindowInterface* browser) {
  if (!browser) {
    return;
  }

  auto* tsm = browser->tab_strip_model();
  auto group_id = tsm->GetTabGroupForTab(tsm->active_index());
  if (!group_id) {
    return;
  }

  tsm->OpenTabGroupEditor(*group_id);
}

void NewTabInGroup(BrowserWindowInterface* browser) {
  if (!browser) {
    return;
  }

  auto* tsm = browser->tab_strip_model();
  auto group_id = tsm->GetTabGroupForTab(tsm->active_index());
  if (!group_id) {
    return;
  }

  const auto tabs = tsm->group_model()->GetTabGroup(*group_id)->ListTabs();
  tsm->delegate()->AddTabAt(GURL(), tabs.end(), true, *group_id);
}

bool CanUngroupAllTabs(BrowserWindowInterface* browser) {
  if (!browser) {
    return false;
  }
  auto* tsm = browser->tab_strip_model();
  for (int i = 0; i < tsm->GetTabCount(); ++i) {
    if (tsm->GetTabGroupForTab(i)) {
      return true;
    }
  }
  return false;
}

void UngroupAllTabs(BrowserWindowInterface* browser) {
  if (!browser) {
    return;
  }

  std::vector<int> indices(browser->tab_strip_model()->GetTabCount());
  std::iota(indices.begin(), indices.end(), 0);
  browser->tab_strip_model()->RemoveFromGroup(indices);
}

void ToggleGroupExpanded(BrowserWindowInterface* browser) {
  if (!browser) {
    return;
  }
  auto* tsm = browser->tab_strip_model();
  auto group_id = tsm->GetTabGroupForTab(tsm->active_index());
  if (!group_id) {
    return;
  }

  auto* group = tsm->group_model()->GetTabGroup(*group_id);
  auto* vd = group->visual_data();
  tab_groups::TabGroupVisualData vd_update(vd->title(), vd->color(),
                                           !vd->is_collapsed());
  group->SetVisualData(vd_update);
}

void CloseUngroupedTabs(BrowserWindowInterface* browser) {
  if (!browser) {
    return;
  }
  auto* tsm = static_cast<BraveTabStripModel*>(browser->tab_strip_model());
  CHECK(tsm);

  std::vector<int> indices;

  for (int i = tsm->GetTabCount() - 1; i >= 0; --i) {
    if (!tsm->GetTabGroupForTab(i)) {
      indices.push_back(i);
    }
  }

  for (const auto& index : indices) {
    tsm->CloseWebContentsAt(index,
                            TabCloseTypes::CLOSE_USER_GESTURE |
                                TabCloseTypes::CLOSE_CREATE_HISTORICAL_TAB);
  }
}

void CloseTabsNotInCurrentGroup(BrowserWindowInterface* browser) {
  if (!browser) {
    return;
  }

  auto* tsm = static_cast<BraveTabStripModel*>(browser->tab_strip_model());
  CHECK(tsm);

  auto group_id = tsm->GetTabGroupForTab(tsm->active_index());
  if (!group_id) {
    return;
  }

  std::vector<int> indices;
  for (int i = tsm->GetTabCount() - 1; i >= 0; --i) {
    if (tsm->GetTabGroupForTab(i) != *group_id) {
      indices.push_back(i);
    }
  }

  for (const auto& index : indices) {
    tsm->CloseWebContentsAt(index,
                            TabCloseTypes::CLOSE_USER_GESTURE |
                                TabCloseTypes::CLOSE_CREATE_HISTORICAL_TAB);
  }
}

void CloseGroup(BrowserWindowInterface* browser) {
  if (!browser) {
    return;
  }

  auto* tsm = browser->tab_strip_model();
  auto group_id = tsm->GetTabGroupForTab(tsm->active_index());
  if (!group_id) {
    return;
  }
  tsm->CloseAllTabsInGroup(*group_id);
}

bool CanBringAllTabs(BrowserWindowInterface* browser) {
  if (!browser) {
    return false;
  }

  bool result = false;
  GlobalBrowserCollection::GetInstance()->ForEach(
      [browser, &result](BrowserWindowInterface* from) {
        result = CanTakeTabs(from, browser);
        return !result;
      });
  return result;
}

void BringAllTabs(BrowserWindowInterface* browser) {
  if (!browser) {
    return;
  }

  // Find all browsers with the same profile
  std::vector<BrowserWindowInterface*> browsers;
  base::flat_set<BrowserWindowInterface*> browsers_to_close;
  GlobalBrowserCollection::GetInstance()->ForEach(
      [&browsers, browser](BrowserWindowInterface* from) {
        if (CanTakeTabs(from, browser)) {
          browsers.push_back(from);
        }
        return true;
      });

  // Detach all tabs from other browsers
  std::stack<std::unique_ptr<tabs::TabModel>> detached_pinned_tabs;
  std::stack<std::unique_ptr<tabs::TabModel>> detached_unpinned_tabs;

  const bool shared_pinned_tab_enabled =
      base::FeatureList::IsEnabled(tabs::features::kBraveSharedPinnedTabs) &&
      browser->GetProfile()->GetPrefs()->GetBoolean(
          brave_tabs::kSharedPinnedTab);

  std::ranges::for_each(browsers, [&detached_pinned_tabs,
                                   &detached_unpinned_tabs, &browsers_to_close,
                                   shared_pinned_tab_enabled](auto* other) {
    static_cast<BraveBrowser*>(other)
        ->set_ignore_enable_closing_last_tab_pref();

    auto* tab_strip_model = other->tab_strip_model();
    const int pinned_tab_count = tab_strip_model->IndexOfFirstNonPinnedTab();
    for (int i = tab_strip_model->count() - 1; i >= 0; --i) {
      const bool is_pinned = i < pinned_tab_count;
      if (is_pinned && shared_pinned_tab_enabled) {
        // SharedPinnedTabService is responsible for synchronizing pinned
        // tabs, thus we shouldn't manually detach and attach tabs here.
        // Meanwhile, the tab strips don't get empty when they have dummy
        // contents, we should close the browsers manually.
        browsers_to_close.insert(other);
        continue;
      }

      auto tab = tab_strip_model->DetachTabAtForInsertion(i);
      if (is_pinned) {
        detached_pinned_tabs.push(std::move(tab));
      } else {
        detached_unpinned_tabs.push(std::move(tab));
      }
    }
  });

  // Insert pinned tabs
  auto* tab_strip_model = browser->tab_strip_model();
  while (!detached_pinned_tabs.empty()) {
    tab_strip_model->InsertDetachedTabAt(
        tab_strip_model->IndexOfFirstNonPinnedTab(),
        std::move(detached_pinned_tabs.top()), AddTabTypes::ADD_PINNED);
    detached_pinned_tabs.pop();
  }

  // Insert unpinned tabs
  while (!detached_unpinned_tabs.empty()) {
    tab_strip_model->InsertDetachedTabAt(
        tab_strip_model->count(), std::move(detached_unpinned_tabs.top()),
        AddTabTypes::ADD_NONE);
    detached_unpinned_tabs.pop();
  }

  if (shared_pinned_tab_enabled) {
    std::ranges::for_each(browsers_to_close, [](auto* other) {
      BrowserWindow::FromBrowser(other)->Close();
    });
  }
}

bool HasDuplicateTabs(BrowserWindowInterface* browser) {
  if (!browser) {
    return false;
  }

  auto* tsm = browser->tab_strip_model();
  auto* active_web_contents = tsm->GetActiveWebContents();
  if (!active_web_contents) {
    return false;
  }

  auto url = active_web_contents->GetVisibleURL();
  for (int i = 0; i < tsm->GetTabCount(); ++i) {
    // Don't check the active tab.
    if (tsm->active_index() == i) {
      continue;
    }

    auto* tab = tsm->GetWebContentsAt(i);
    if (tab->GetVisibleURL() == url) {
      return true;
    }
  }

  return false;
}

void CloseDuplicateTabs(BrowserWindowInterface* browser) {
  auto* tsm = browser->tab_strip_model();
  auto url = tsm->GetActiveWebContents()->GetVisibleURL();

  for (int i = tsm->GetTabCount() - 1; i >= 0; --i) {
    // Don't close the active tab.
    if (tsm->active_index() == i) {
      continue;
    }

    auto* tab = tsm->GetWebContentsAt(i);
    if (tab->GetVisibleURL() == url) {
      tab->Close();
    }
  }
}

bool CanCloseTabsToLeft(BrowserWindowInterface* browser) {
  auto* tsm = browser->tab_strip_model();
  const auto& selection = tsm->selection_model();
  if (selection.empty()) {
    return false;
  }

  int left_selected = *(selection.selected_indices().begin());
  return left_selected > 0;
}

void CloseTabsToLeft(BrowserWindowInterface* browser) {
  auto* tsm = browser->tab_strip_model();
  const auto& selection = tsm->selection_model();
  if (selection.empty()) {
    return;
  }

  int left_selected = *(selection.selected_indices().begin());
  for (int i = left_selected - 1; i >= 0; --i) {
    tsm->CloseWebContentsAt(i, TabCloseTypes::CLOSE_CREATE_HISTORICAL_TAB |
                                   TabCloseTypes::CLOSE_USER_GESTURE);
  }
}

bool CanCloseUnpinnedTabs(BrowserWindowInterface* browser) {
  auto first_unpinned_index =
      browser->tab_strip_model()->IndexOfFirstNonPinnedTab();
  return first_unpinned_index < browser->tab_strip_model()->count();
}

void CloseUnpinnedTabs(BrowserWindowInterface* browser) {
  auto* tsm = browser->tab_strip_model();
  DCHECK(CanCloseUnpinnedTabs(browser));

  for (int i = tsm->count() - 1; i >= tsm->IndexOfFirstNonPinnedTab(); --i) {
    tsm->CloseWebContentsAt(i, TabCloseTypes::CLOSE_CREATE_HISTORICAL_TAB |
                                   TabCloseTypes::CLOSE_USER_GESTURE);
  }
}

void AddAllTabsToNewGroup(BrowserWindowInterface* browser) {
  std::vector<int> indices(browser->tab_strip_model()->count());
  std::iota(indices.begin(), indices.end(), 0);
  browser->tab_strip_model()->AddToNewGroup(indices);
}

bool CanMuteAllTabs(BrowserWindowInterface* browser, bool exclude_active) {
  auto* tsm = browser->tab_strip_model();
  for (int i = 0; i < tsm->count(); ++i) {
    if (exclude_active && tsm->active_index() == i) {
      continue;
    }

    auto* contents = tsm->GetWebContentsAt(i);
    if (contents->IsCurrentlyAudible()) {
      return true;
    }
  }
  return false;
}

void MuteAllTabs(BrowserWindowInterface* browser, bool exclude_active) {
  auto* tsm = browser->tab_strip_model();
  for (int i = 0; i < tsm->count(); ++i) {
    if (exclude_active && tsm->active_index() == i) {
      continue;
    }

    auto* contents = tsm->GetWebContentsAt(i);
    if (contents->IsCurrentlyAudible()) {
      contents->SetAudioMuted(true);
    }
  }
}

bool CanUnmuteAllTabs(BrowserWindowInterface* browser) {
  auto* tsm = browser->tab_strip_model();
  for (int i = 0; i < tsm->count(); ++i) {
    auto* contents = tsm->GetWebContentsAt(i);
    if (contents->IsAudioMuted()) {
      return true;
    }
  }
  return false;
}

void UnmuteAllTabs(BrowserWindowInterface* browser) {
  auto* tsm = browser->tab_strip_model();
  for (int i = 0; i < tsm->count(); ++i) {
    auto* contents = tsm->GetWebContentsAt(i);
    if (contents->IsAudioMuted()) {
      contents->SetAudioMuted(false);
    }
  }
}

void ScrollTabToTop(BrowserWindowInterface* browser) {
  auto* contents = browser->tab_strip_model()->GetActiveWebContents();
  contents->ScrollToTopOfDocument();
}

void ScrollTabToBottom(BrowserWindowInterface* browser) {
  auto* contents = browser->tab_strip_model()->GetActiveWebContents();
  contents->ScrollToBottomOfDocument();
}

void ToggleAllBookmarksButtonVisibility(BrowserWindowInterface* browser) {
  auto* prefs = browser->GetProfile()->GetPrefs();
  prefs->SetBoolean(
      brave::bookmarks::prefs::kShowAllBookmarksButton,
      !prefs->GetBoolean(brave::bookmarks::prefs::kShowAllBookmarksButton));
}

}  // namespace brave

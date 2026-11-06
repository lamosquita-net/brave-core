/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_BROWSER_UI_BROWSER_COMMANDS_H_
#define BRAVE_BROWSER_UI_BROWSER_COMMANDS_H_

#include <optional>
#include <vector>

#include "chrome/browser/ui/tabs/tab_model.h"

class BrowserWindowInterface;
class GURL;

namespace brave {

bool HasSelectedURL(BrowserWindowInterface* browser);
void CleanAndCopySelectedURL(BrowserWindowInterface* browser);

// Copies an url sanitized by URLSanitizerService.
void CopySanitizedURL(BrowserWindowInterface* browser, const GURL& url);
// Copies an url cleared through:
// - Debouncer (potentially debouncing many levels)
// - Query filter
// - URLSanitizerService
void CopyLinkWithStrictCleaning(BrowserWindowInterface* browser,
                                const GURL& url);

void ToggleWindowTitleVisibilityForVerticalTabs(
    BrowserWindowInterface* browser);
void ToggleVerticalTabStrip(BrowserWindowInterface* browser);
void ToggleVerticalTabStripFloatingMode(BrowserWindowInterface* browser);
void ToggleVerticalTabStripExpanded(BrowserWindowInterface* browser);

void ToggleActiveTabAudioMute(BrowserWindowInterface* browser);

void ToggleShieldsEnabled(BrowserWindowInterface* browser);
void ToggleJavascriptEnabled(BrowserWindowInterface* browser);

void GroupTabsOnCurrentOrigin(BrowserWindowInterface* browser);
void MoveGroupToNewWindow(BrowserWindowInterface* browser);

bool IsInGroup(BrowserWindowInterface* browser);
bool HasUngroupedTabs(BrowserWindowInterface* browser);

void GroupUngroupedTabs(BrowserWindowInterface* browser);
void UngroupCurrentGroup(BrowserWindowInterface* browser);
void RemoveTabFromGroup(BrowserWindowInterface* browser);
void NameGroup(BrowserWindowInterface* browser);
void NewTabInGroup(BrowserWindowInterface* browser);

bool CanUngroupAllTabs(BrowserWindowInterface* browser);
void UngroupAllTabs(BrowserWindowInterface* browser);

void ToggleGroupExpanded(BrowserWindowInterface* browser);
void CloseUngroupedTabs(BrowserWindowInterface* browser);
void CloseTabsNotInCurrentGroup(BrowserWindowInterface* browser);
void CloseGroup(BrowserWindowInterface* browser);

bool CanBringAllTabs(BrowserWindowInterface* browser);
void BringAllTabs(BrowserWindowInterface* browser);

bool HasDuplicateTabs(BrowserWindowInterface* browser);
void CloseDuplicateTabs(BrowserWindowInterface* browser);

bool CanCloseTabsToLeft(BrowserWindowInterface* browser);
void CloseTabsToLeft(BrowserWindowInterface* browser);

bool CanCloseUnpinnedTabs(BrowserWindowInterface* browser);
void CloseUnpinnedTabs(BrowserWindowInterface* browser);

void AddAllTabsToNewGroup(BrowserWindowInterface* browser);

bool CanMuteAllTabs(BrowserWindowInterface* browser, bool exclude_active);
void MuteAllTabs(BrowserWindowInterface* browser, bool exclude_active);

bool CanUnmuteAllTabs(BrowserWindowInterface* browser);
void UnmuteAllTabs(BrowserWindowInterface* browser);

void ScrollTabToTop(BrowserWindowInterface* browser);
void ScrollTabToBottom(BrowserWindowInterface* browser);

void ToggleAllBookmarksButtonVisibility(BrowserWindowInterface* browser);

// In case |tab| is not provided, the active tab will be used.
bool CanOpenNewSplitViewForTab(
    BrowserWindowInterface* browser,
    std::optional<tabs::TabHandle> tab = std::nullopt);
void NewSplitViewForTab(BrowserWindowInterface* browser,
                        std::optional<tabs::TabHandle> tab = std::nullopt);
// In case |indices| empty, selected tabs will be used.
void TileTabs(BrowserWindowInterface* browser,
              const std::vector<int>& indices = {});
void BreakTiles(BrowserWindowInterface* browser,
                const std::vector<int>& indices = {});
bool IsTabsTiled(BrowserWindowInterface* browser,
                 const std::vector<int>& indices = {});
bool CanTileTabs(BrowserWindowInterface* browser,
                 const std::vector<int>& indices = {});
void SwapTabsInTile(BrowserWindowInterface* browser);

}  // namespace brave

#endif  // BRAVE_BROWSER_UI_BROWSER_COMMANDS_H_

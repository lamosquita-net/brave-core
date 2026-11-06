/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_CHROMIUM_SRC_CHROME_BROWSER_UI_VIEWS_TABS_TAB_H_
#define BRAVE_CHROMIUM_SRC_CHROME_BROWSER_UI_VIEWS_TABS_TAB_H_

#include "chrome/browser/ui/views/tabs/tab/alert_indicator_button.h"
// Included before the IsApparentlyActive macro below: TabStyleViews also
// declares IsApparentlyActive() and must not get the injected override.
#include "chrome/browser/ui/views/tabs/tab_style_views.h"

class BraveTabTest;
class BraveTab;

#define kMinimumContentsWidthForCloseButtons \
  kMinimumContentsWidthForCloseButtons = 55; \
  friend class ::BraveTabTest;               \
  friend class ::BraveTab;                   \
  static constexpr int kMinimumContentsWidthForCloseButtons_UnUsed

#define GetWidthOfLargestSelectableRegion \
  virtual GetWidthOfLargestSelectableRegion

#define ActiveStateChanged virtual ActiveStateChanged

// AlertIndicatorButton::Delegate, extended in our alert_indicator_button.h.
#define IsApparentlyActive                   \
  IsTabMuteIndicatorNotClickable() override; \
  bool IsApparentlyActive

#include "src/chrome/browser/ui/views/tabs/tab.h"  // IWYU pragma: export

#undef IsApparentlyActive
#undef ActiveStateChanged
#undef GetWidthOfLargestSelectableRegion
#undef kMinimumContentsWidthForCloseButtons

#endif  // BRAVE_CHROMIUM_SRC_CHROME_BROWSER_UI_VIEWS_TABS_TAB_H_

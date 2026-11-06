/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

// Include the corresponding header first since it defines the same macros and
// therefore avoid undef before use.
#include "chrome/browser/ui/views/frame/browser_view.h"

#include "brave/browser/ui/views/frame/brave_browser_view_layout.h"
#include "brave/browser/ui/views/infobars/brave_infobar_container_view.h"
#include "brave/browser/ui/views/tabs/brave_browser_tab_strip_controller.h"
#include "brave/browser/ui/views/tabs/brave_tab_strip.h"
#include "brave/browser/ui/views/toolbar/brave_toolbar_view.h"
#include "brave/components/constants/pref_names.h"
#include "chrome/browser/ui/browser_commands.h"
#include "chrome/browser/ui/views/frame/browser_view_layout.h"

#define InfoBarContainerView BraveInfoBarContainerView
#define BrowserViewLayout BraveBrowserViewLayout
#define ToolbarView BraveToolbarView
#define BrowserTabStripController BraveBrowserTabStripController
#define TabStrip BraveTabStrip
#define BookmarkBarView BraveBookmarkBarView
#define UpdateExclusiveAccessBubble UpdateExclusiveAccessBubble_ChromiumImpl

#include "src/chrome/browser/ui/views/frame/browser_view.cc"

#undef UpdateExclusiveAccessBubble
#undef BookmarkBarView
#undef TabStrip
#undef BrowserTabStripController
#undef ToolbarView
#undef BrowserViewLayout
#undef InfoBarContainerView

void BrowserView::UpdateExclusiveAccessBubble(
    const ExclusiveAccessBubbleParams& params,
    ExclusiveAccessBubbleHideCallback first_hide_callback) {
  // Show/Hide full screen reminder bubble based on our settings preference
  // for tab-initiated ones.
  if (!GetProfile()->GetPrefs()->GetBoolean(kShowFullscreenReminder) &&
      params.type == EXCLUSIVE_ACCESS_BUBBLE_TYPE_FULLSCREEN_EXIT_INSTRUCTION) {
    return;
  }

  UpdateExclusiveAccessBubble_ChromiumImpl(params,
                                           std::move(first_hide_callback));
}

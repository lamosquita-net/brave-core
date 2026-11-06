/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

// Include the corresponding header first since it defines the same macros and
// therefore avoid undef before use.
#include "chrome/browser/ui/views/frame/browser_view.h"

#include "brave/browser/ui/views/infobars/brave_infobar_container_view.h"
#include "brave/browser/ui/views/toolbar/brave_toolbar_view.h"
#include "brave/components/constants/pref_names.h"
#include "chrome/browser/ui/browser_commands.h"
#include "chrome/browser/ui/exclusive_access/exclusive_access_context.h"

#define InfoBarContainerView BraveInfoBarContainerView
#define ToolbarView BraveToolbarView
#define BookmarkBarView BraveBookmarkBarView

// 156 moved UpdateExclusiveAccessBubble() from BrowserView to
// BrowserView::ExclusiveAccessContextImpl. The chromium_src override of
// exclusive_access_context.h adds the virtual _ChromiumImpl that the upstream
// body becomes here.
#define UpdateExclusiveAccessBubble(...)             \
  UpdateExclusiveAccessBubble(__VA_ARGS__) override; \
  virtual void UpdateExclusiveAccessBubble_ChromiumImpl(__VA_ARGS__)

#include "src/chrome/browser/ui/views/frame/browser_view.cc"

#undef UpdateExclusiveAccessBubble
#undef BookmarkBarView
#undef ToolbarView
#undef InfoBarContainerView

void BrowserView::ExclusiveAccessContextImpl::UpdateExclusiveAccessBubble(
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

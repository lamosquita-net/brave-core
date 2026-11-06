// Copyright (c) 2022 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// you can obtain one at http://mozilla.org/MPL/2.0/.

#include "brave/browser/ui/views/frame/brave_browser_view_layout.h"

#include "brave/browser/ui/brave_browser.h"
#include "brave/browser/ui/views/frame/brave_contents_view_util.h"
#include "chrome/browser/ui/browser_window/public/browser_window_features.h"
#include "chrome/browser/ui/exclusive_access/exclusive_access_manager.h"
#include "chrome/browser/ui/exclusive_access/fullscreen_controller.h"
#include "chrome/browser/ui/views/frame/browser_view.h"
#include "chrome/browser/ui/views/frame/browser_view_layout_delegate.h"
#include "chrome/browser/ui/views/infobars/infobar_container_view.h"

BraveBrowserViewLayout::~BraveBrowserViewLayout() = default;

gfx::Insets BraveBrowserViewLayout::GetContentsMargins() const {
  if (!BraveBrowser::ShouldUseBraveWebViewRoundedCorners(
          browser_view_->browser())) {
    return {};
  }

  if (IsFullscreenForTab()) {
    return {};
  }

  gfx::Insets margins(BraveContentsViewUtil::kMarginThickness);

  // If there is a visible view above the contents container, then there is no
  // no need for a top margin.
  if (browser_view_->GetTabStripVisible() ||
      browser_view_->IsToolbarVisible() ||
      browser_view_->IsBookmarkBarVisible() || IsInfobarVisible()) {
    margins.set_top(0);
  }

  return margins;
}

bool BraveBrowserViewLayout::IsFullscreenForTab() const {
  auto* exclusive_access_manager = browser_view_->GetExclusiveAccessManager();
  if (!exclusive_access_manager) {
    return false;
  }
  auto* fullscreen_controller =
      exclusive_access_manager->fullscreen_controller();
  return fullscreen_controller &&
         fullscreen_controller->IsWindowFullscreenForTabOrPending();
}

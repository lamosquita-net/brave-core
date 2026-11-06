/* Copyright (c) 2023 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "chrome/browser/ui/views/tabs/tab_group_style.h"

#include "brave/browser/ui/tabs/brave_tab_layout_constants.h"
#include "brave/browser/ui/tabs/features.h"
#include "brave/browser/ui/views/tabs/brave_tab_group_underline.h"

#define TabGroupUnderline BraveTabGroupUnderline
#define TabGroupStyle TabGroupStyle_ChromiumImpl

#include "src/chrome/browser/ui/views/tabs/tab_group_style.cc"

#undef TabGroupStyle
#undef TabGroupUnderline

bool TabGroupStyle::TabGroupUnderlineShouldBeHidden() const {
  return false;
}

// Upstream currently hides the tab group underline in certain scenarios,
// whereas we always show the underline.
bool TabGroupStyle::TabGroupUnderlineShouldBeHidden(
    const views::View* leading_view,
    const views::View* trailing_view) const {
  return false;
}

gfx::Insets TabGroupStyle::GetInsetsForHeaderChip(
    bool should_show_sync_icon) const {
  if (!tabs::features::HorizontalTabsUpdateEnabled()) {
    return TabGroupStyle_ChromiumImpl::GetInsetsForHeaderChip(
        should_show_sync_icon);
  }
  return gfx::Insets::VH(brave_tabs::kTabGroupTitleVerticalInset,
                         brave_tabs::kTabGroupTitleHorizontalInset);
}

gfx::Point TabGroupStyle::GetTitleChipOffset(
    std::optional<int> text_height) const {
  if (!tabs::features::HorizontalTabsUpdateEnabled()) {
    return TabGroupStyle_ChromiumImpl::GetTitleChipOffset(text_height);
  }
  return gfx::Point(brave_tabs::kHorizontalTabInset,
                    brave_tabs::kHorizontalTabVerticalSpacing);
}

float TabGroupStyle::GetEmptyChipSize() const {
  if (!tabs::features::HorizontalTabsUpdateEnabled()) {
    return TabGroupStyle_ChromiumImpl::GetEmptyChipSize();
  }
  return brave_tabs::kEmptyGroupTitleSize;
}

int TabGroupStyle::GetChipCornerRadius() const {
  if (!tabs::features::HorizontalTabsUpdateEnabled()) {
    return TabGroupStyle_ChromiumImpl::GetChipCornerRadius();
  }
  return brave_tabs::kTabBorderRadius;
}

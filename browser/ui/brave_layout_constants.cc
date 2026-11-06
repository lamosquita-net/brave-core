/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "brave/browser/ui/brave_layout_constants.h"

#include "chrome/browser/ui/layout_constants.h"
#include "ui/base/pointer/touch_ui_controller.h"

std::optional<gfx::Insets> GetBraveLayoutInsets(LayoutInset inset) {
  const bool touch_ui = ui::TouchUiController::Get()->touch_ui();
  switch (inset) {
    case LOCATION_BAR_PAGE_INFO_ICON_PADDING:
      return gfx::Insets::VH(6, 6);
    case LOCATION_BAR_PAGE_ACTION_ICON_PADDING:
      return gfx::Insets::VH(4, 4);
    case TOOLBAR_BUTTON:
      // Use 5 inset - (TOOLBAR_BUTTON_HEIGHT(28) - icon size(18)) / 2
      // icon size - ToolbarButton::kDefaultIconSize
      return gfx::Insets(touch_ui ? 12 : 5);
    case TOOLBAR_INTERIOR_MARGIN:
      return touch_ui ? gfx::Insets() : gfx::Insets::VH(4, 8);
    default:
      break;
  }
  return std::nullopt;
}

// Returns a |nullopt| if the UI color is not handled by Brave.
std::optional<int> GetBraveLayoutConstant(LayoutConstant constant) {
  const bool touch = ui::TouchUiController::Get()->touch_ui();
  switch (constant) {
    case LayoutConstant::kLocationBarChildCornerRadius:
      return 4;
    case LayoutConstant::kToolbarButtonHeight: {
      return touch ? 48 : 28;
    }
    case LayoutConstant::kToolbarCornerRadius: {
      return 0;
    }
    case LayoutConstant::kLocationBarHeight:
      // Consider adjust below element padding also when this height is changed.
      return 32;
    case LayoutConstant::kLocationBarTrailingIconSize:
      return 18;
    case LayoutConstant::kLocationBarIconSize:
      return 16;
    case LayoutConstant::kLocationBarElementPadding:
    case LayoutConstant::kLocationBarPageInfoIconVerticalPadding:
      return 2;
    default:
      break;
  }
  return std::nullopt;
}

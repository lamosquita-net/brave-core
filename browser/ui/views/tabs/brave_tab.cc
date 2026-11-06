// Copyright (c) 2022 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// you can obtain one at http://mozilla.org/MPL/2.0/.

#include "brave/browser/ui/views/tabs/brave_tab.h"

#include <algorithm>

#include "brave/browser/ui/tabs/brave_tab_prefs.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/browser/ui/browser_window/public/browser_window_interface.h"
#include "chrome/browser/ui/views/tabs/tab/alert_indicator_button.h"
#include "chrome/browser/ui/views/tabs/tab/tab_close_button.h"
#include "chrome/browser/ui/views/tabs/tab_slot_controller.h"

BraveTab::BraveTab(tabs::TabHandle handle, TabSlotController* controller)
    : Tab(handle, controller) {}

BraveTab::~BraveTab() = default;

std::u16string BraveTab::GetRenderedTooltipText(const gfx::Point& p) const {
  auto* browser = controller_->GetBrowserWindowInterface();
  if (browser &&
      brave_tabs::AreTooltipsEnabled(browser->GetProfile()->GetPrefs())) {
    return Tab::GetTooltipText(data_.title, data_.alert_state);
  }
  return Tab::GetRenderedTooltipText(p);
}

int BraveTab::GetWidthOfLargestSelectableRegion() const {
  // Assume the entire region except the area that alert indicator/close buttons
  // occupied is available for click-to-select.
  // If neither are visible, the entire tab region is available.
  int selectable_width = width();
  if (alert_indicator_button_->GetVisible()) {
    selectable_width -= alert_indicator_button_->width();
  }

  if (close_button_->GetVisible()) {
    selectable_width -= close_button_->width();
  }

  return std::max(0, selectable_width);
}

void BraveTab::ActiveStateChanged() {
  Tab::ActiveStateChanged();

  // This should be called whenever the active state changes
  // see comment on UpdateEnabledForMuteToggle();
  // https://github.com/brave/brave-browser/issues/23476/
  alert_indicator_button_->UpdateEnabledForMuteToggle();
}

gfx::Insets BraveTab::GetInsets() const {
  // As close button has more padding, it seems favicon is too close to the left
  // edge of the tab left border comppared with close button. Give additional
  // left padding to make both visible with same space from tab border.
  // See https://www.github.com/brave/brave-browser/issues/30469.
  auto insets = Tab::GetInsets();
  insets.set_left(insets.left() + kExtraLeftPadding);
  return insets;
}

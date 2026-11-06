// Copyright (c) 2022 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// you can obtain one at http://mozilla.org/MPL/2.0/.

#include "brave/browser/ui/views/tabs/brave_tab.h"

#include <algorithm>
#include <optional>

#include "brave/browser/ui/tabs/brave_tab_layout_constants.h"
#include "brave/browser/ui/tabs/brave_tab_prefs.h"
#include "brave/browser/ui/tabs/features.h"
#include "brave/browser/ui/views/tabs/brave_tab_strip_layout_helper.h"
#include "brave/browser/ui/views/view_shadow.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/browser/ui/browser.h"
#include "chrome/browser/ui/views/tabs/alert_indicator_button.h"
#include "chrome/browser/ui/views/tabs/tab_close_button.h"
#include "chrome/browser/ui/views/tabs/tab_slot_controller.h"
#include "ui/compositor/layer.h"
#include "ui/compositor/paint_recorder.h"
#include "ui/gfx/skia_paint_util.h"
#include "ui/views/controls/label.h"
#include "ui/views/view_class_properties.h"

namespace {

constexpr ViewShadow::ShadowParameters kShadow{
    .offset_x = 0,
    .offset_y = 1,
    .blur_radius = 4,
    .shadow_color = SkColorSetA(SK_ColorBLACK, 0.07 * 255)};

}  // namespace

BraveTab::BraveTab(TabSlotController* controller) : Tab(controller) {}

BraveTab::~BraveTab() = default;

std::u16string BraveTab::GetTooltipText(const gfx::Point& p) const {
  auto* browser = controller_->GetBrowser();
  if (browser &&
      brave_tabs::AreTooltipsEnabled(browser->profile()->GetPrefs())) {
    return Tab::GetTooltipText(data_.title,
                               GetAlertStateToShow(data_.alert_state));
  }
  return Tab::GetTooltipText(p);
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

  UpdateShadowForActiveTab();
}

std::optional<SkColor> BraveTab::GetGroupColor() const {
  if (!tabs::features::HorizontalTabsUpdateEnabled()) {
    return Tab::GetGroupColor();
  }

  // Unlike upstream, tabs that are within a group are not given a border color.
  return {};
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

void BraveTab::UpdateShadowForActiveTab() {
  if (IsActive() && tabs::features::HorizontalTabsUpdateEnabled()) {
    if (!view_shadow_) {
      view_shadow_ = std::make_unique<ViewShadow>(
          this, tabs::GetTabCornerRadius(*this), kShadow);
      layer()->SetFillsBoundsOpaquely(false);
    }

    // Inset the shadow layer to match the visual rounded rectangle of the
    // tab, which is inset from the tab view.
    view_shadow_->SetInsets(
        gfx::Insets::VH(brave_tabs::kHorizontalTabVerticalSpacing,
                        brave_tabs::kHorizontalTabInset));
  } else if (view_shadow_) {
    view_shadow_.reset();
    DestroyLayer();
  }
}

/* Copyright (c) 2022 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/browser/ui/views/tabs/brave_tab_container.h"

#include <algorithm>
#include <optional>

#include "chrome/browser/ui/views/tabs/tab.h"
#include "ui/base/metadata/metadata_impl_macros.h"

BraveTabContainer::BraveTabContainer(
    TabContainerController& controller,
    TabHoverCardController* hover_card_controller,
    TabDragPositioningDelegateBase* drag_position_delegate,
    TabSlotController& tab_slot_controller)
    : TabContainerImpl(controller,
                       hover_card_controller,
                       drag_position_delegate,
                       tab_slot_controller) {}

BraveTabContainer::~BraveTabContainer() {
  // When the last tab is closed and tab strip is being destructed, the
  // animation for the last removed tab could have been scheduled but not
  // finished yet. In this case, stop the animation.
  CancelAnimation();
}

void BraveTabContainer::OnTabCloseAnimationCompleted(Tab* tab) {
  TabContainerImpl::OnTabCloseAnimationCompleted(tab);

  // we might have to hide this container entirely
  if (!tabs_view_model_.view_size()) {
    PreferredSizeChanged();
  }
}

void BraveTabContainer::CompleteAnimationAndLayout() {
  TabContainerImpl::CompleteAnimationAndLayout();

  // Should force tabs to layout as they might not change bounds, which makes
  // insets not updated.
  std::ranges::for_each(children(), &views::View::DeprecatedLayoutImmediately);
}

void BraveTabContainer::SetTabSlotVisibility() {
  // During multiple tab closing including group, this method could be called
  // but group_views_ could be empty already. We should clear group info in tabs
  // in that case.
  // https://github.com/brave/brave-browser/issues/39298
  for (Tab* tab : layout_helper_->GetTabs()) {
    if (std::optional<tab_groups::TabGroupId> group = tab->group();
        group && !group_views_.contains(*group)) {
      tab->SetGroup(std::nullopt);
    }
  }

  TabContainerImpl::SetTabSlotVisibility();
}

BEGIN_METADATA(BraveTabContainer)
END_METADATA

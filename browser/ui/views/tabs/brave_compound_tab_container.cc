/* Copyright (c) 2022 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/browser/ui/views/tabs/brave_compound_tab_container.h"

#include <algorithm>
#include <utility>
#include <vector>

#include "base/functional/callback_helpers.h"
#include "brave/browser/ui/views/tabs/brave_tab_container.h"
#include "ui/base/metadata/metadata_impl_macros.h"

BraveCompoundTabContainer::BraveCompoundTabContainer(
    TabContainerController& controller,
    TabHoverCardController* hover_card_controller,
    TabDragContextBase* drag_context,
    TabSlotController& tab_slot_controller,
    views::View* scroll_contents_view)
    : CompoundTabContainer(controller,
                           hover_card_controller,
                           drag_context,
                           tab_slot_controller,
                           scroll_contents_view) {}

void BraveCompoundTabContainer::SetAvailableWidthCallback(
    base::RepeatingCallback<int()> available_width_callback) {
  CompoundTabContainer::SetAvailableWidthCallback(available_width_callback);

  // Upstream's compound tab container doesn't use this.
  pinned_tab_container_->SetAvailableWidthCallback(base::NullCallback());
  unpinned_tab_container_->SetAvailableWidthCallback(base::NullCallback());
}

BraveCompoundTabContainer::~BraveCompoundTabContainer() = default;

base::OnceClosure BraveCompoundTabContainer::LockLayout() {
  std::vector<base::OnceClosure> closures;
  for (const auto& tab_container :
       {unpinned_tab_container_, pinned_tab_container_}) {
    closures.push_back(
        static_cast<BraveTabContainer*>(base::to_address(tab_container))
            ->LockLayout());
  }

  return base::BindOnce(
      [](std::vector<base::OnceClosure> closures) {
        std::ranges::for_each(closures,
                              [](auto& closure) { std::move(closure).Run(); });
      },
      std::move(closures));
}

BEGIN_METADATA(BraveCompoundTabContainer)
END_METADATA

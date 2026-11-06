/* Copyright (c) 2023 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "brave/browser/ui/views/tabs/tab_drag_controller.h"

#include <algorithm>
#include <optional>

#include "base/feature_list.h"
#include "brave/browser/ui/tabs/brave_tab_prefs.h"
#include "brave/browser/ui/tabs/features.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/browser/ui/tabs/tab_group_model.h"
#include "chrome/browser/ui/views/frame/browser_view.h"
#include "chrome/browser/ui/views/tabs/tab.h"
#include "chrome/browser/ui/views/tabs/tab_drag_context.h"
#include "components/prefs/pref_service.h"
#include "components/tabs/public/tab_group.h"
#include "ui/views/view_utils.h"

TabDragController::TabDragController() = default;

TabDragController::~TabDragController() = default;

TabDragController::Liveness TabDragController::Init(
    TabDragContext* source_context,
    TabSlotView* source_view,
    const std::vector<raw_ptr<TabSlotView, VectorExperimental>>& dragging_views,
    const gfx::Point& mouse_offset,
    int source_view_offset,
    ui::ListSelectionModel initial_selection_model,
    ui::mojom::DragEventSource event_source) {
  if (TabDragControllerChromium::Init(
          source_context, source_view, dragging_views, mouse_offset,
          source_view_offset, initial_selection_model,
          event_source) == TabDragController::Liveness::DELETED) {
    return TabDragController::Liveness::DELETED;
  }

  auto* widget = source_view->GetWidget();
  DCHECK(widget);
  const auto* browser =
      BrowserView::GetBrowserViewForNativeWindow(widget->GetNativeWindow())
          ->browser();

  if (base::FeatureList::IsEnabled(tabs::features::kBraveSharedPinnedTabs) &&
      browser->profile()->GetPrefs()->GetBoolean(
          brave_tabs::kSharedPinnedTab)) {
    if (std::ranges::any_of(dragging_views, [](TabSlotView* slot_view) {
          // We don't allow sharable pinned tabs to be detached.
          return slot_view->GetTabSlotViewType() ==
                     TabSlotView::ViewType::kTab &&
                 views::AsViewClass<Tab>(slot_view)->data().pinned;
        })) {
      detach_behavior_ = NOT_DETACHABLE;
    }
  }

  return TabDragController::Liveness::ALIVE;
}

void TabDragController::InitDragData(TabSlotView* view,
                                     TabDragData* drag_data) {
  // This seems to be a bug from upstream. If the `view` is a group header,
  // there can't be contents or pinned state which are bound to this `view`.
  if (view->GetTabSlotViewType() == TabSlotView::ViewType::kTabGroupHeader) {
    std::optional<tab_groups::TabGroupId> tab_group_id = view->group();
    DCHECK(tab_group_id.has_value());
    drag_data->tab_group_data = TabDragData::TabGroupData{
        tab_group_id.value(), *source_context_->GetTabStripModel()
                                   ->group_model()
                                   ->GetTabGroup(tab_group_id.value())
                                   ->visual_data()};
    return;
  }

  TabDragControllerChromium::InitDragData(view, drag_data);
}

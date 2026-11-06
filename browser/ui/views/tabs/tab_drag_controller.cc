/* Copyright (c) 2023 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "brave/browser/ui/views/tabs/tab_drag_controller.h"

#include <algorithm>

#include "base/feature_list.h"
#include "brave/browser/ui/tabs/brave_tab_prefs.h"
#include "brave/browser/ui/tabs/features.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/browser/ui/views/frame/browser_view.h"
#include "chrome/browser/ui/views/tabs/dragging/tab_drag_context.h"
#include "chrome/browser/ui/views/tabs/tab.h"
#include "components/prefs/pref_service.h"
#include "ui/views/view_utils.h"

TabDragController::TabDragController() = default;

TabDragController::~TabDragController() = default;

TabDragController::Liveness TabDragController::Init(
    TabDragContext* source_context,
    TabSlotView* source_view,
    const std::vector<TabSlotView*>& dragging_views,
    const gfx::Point& offset_from_source_view,
    ui::ListSelectionModel initial_selection_model,
    ui::mojom::DragEventSource event_source) {
  if (TabDragControllerChromium::Init(source_context, source_view,
                                      dragging_views, offset_from_source_view,
                                      initial_selection_model, event_source) ==
      Liveness::kDeleted) {
    return Liveness::kDeleted;
  }

  auto* widget = source_view->GetWidget();
  DCHECK(widget);
  const auto* browser =
      BrowserView::GetBrowserViewForNativeWindow(widget->GetNativeWindow())
          ->browser();

  if (base::FeatureList::IsEnabled(tabs::features::kBraveSharedPinnedTabs) &&
      browser->GetProfile()->GetPrefs()->GetBoolean(
          brave_tabs::kSharedPinnedTab)) {
    if (std::ranges::any_of(dragging_views, [](TabSlotView* slot_view) {
          // We don't allow sharable pinned tabs to be detached.
          return slot_view->GetTabSlotViewType() ==
                     TabSlotView::ViewType::kTab &&
                 views::AsViewClass<Tab>(slot_view)->data().pinned;
        })) {
      detach_behavior_ = DetachBehavior::kNotDetachable;
    }
  }

  return Liveness::kAlive;
}

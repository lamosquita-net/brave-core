/* Copyright (c) 2023 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_BROWSER_UI_VIEWS_TABS_TAB_DRAG_CONTROLLER_H_
#define BRAVE_BROWSER_UI_VIEWS_TABS_TAB_DRAG_CONTROLLER_H_

#include <vector>

#include "chrome/browser/ui/views/tabs/tab_drag_controller.h"

class TabDragController : public TabDragControllerChromium {
 public:
  TabDragController();
  ~TabDragController() override;

  // Making this virtual method is really painful as "Init" is too common name.
  // So, just hide Chromium's Init and make clients use this version
  Liveness Init(TabDragContext* source_context,
                TabSlotView* source_view,
                const std::vector<raw_ptr<TabSlotView, VectorExperimental>>&
                    dragging_views,
                const gfx::Point& mouse_offset,
                int source_view_offset,
                ui::ListSelectionModel initial_selection_model,
                ui::mojom::DragEventSource event_source);

  // TabDragControllerChromium:
  void InitDragData(TabSlotView* view, TabDragData* drag_data) override;
};

#endif  // BRAVE_BROWSER_UI_VIEWS_TABS_TAB_DRAG_CONTROLLER_H_

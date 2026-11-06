/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_BROWSER_UI_VIEWS_TABS_BRAVE_BROWSER_TAB_STRIP_CONTROLLER_H_
#define BRAVE_BROWSER_UI_VIEWS_TABS_BRAVE_BROWSER_TAB_STRIP_CONTROLLER_H_

#include <memory>
#include <optional>

#include "chrome/browser/ui/views/tabs/browser_tab_strip_controller.h"

class BraveTabContextMenuContents;

namespace ui {
class Accelerator;
}  // namespace ui

class BraveBrowserTabStripController : public BrowserTabStripController {
 public:
  BraveBrowserTabStripController(TabStripModel* model,
                                 BrowserView* browser_view);
  BraveBrowserTabStripController(const BraveBrowserTabStripController&) =
      delete;
  BraveBrowserTabStripController& operator=(
      const BraveBrowserTabStripController&) = delete;
  ~BraveBrowserTabStripController() override;

  const std::optional<int> GetModelIndexOf(Tab* tab);
  TabStripModel* model() const { return model_; }

  // 156 routes the tab context menu through TabContextMenuController's
  // delegate; these expose that delegate to BraveTabContextMenuContents.
  bool IsCommandEnabledForTab(TabStripModel::ContextMenuCommand command_id,
                              const Tab* tab);
  void ExecuteCommandForTab(TabStripModel::ContextMenuCommand command_id,
                            const Tab* tab,
                            int event_flags);
  bool GetAcceleratorForCommandId(int command_id,
                                  ui::Accelerator* accelerator);

  // BrowserTabStripController overrides:
  void ShowContextMenuForTab(Tab* tab,
                             const gfx::Point& p,
                             ui::mojom::MenuSourceType source_type) override;

 private:
  tabs::TabInterface* GetTabInterface(const Tab* tab);

  // If non-NULL it means we're showing a menu for the tab.
  std::unique_ptr<BraveTabContextMenuContents> context_menu_contents_;
};

#endif  // BRAVE_BROWSER_UI_VIEWS_TABS_BRAVE_BROWSER_TAB_STRIP_CONTROLLER_H_

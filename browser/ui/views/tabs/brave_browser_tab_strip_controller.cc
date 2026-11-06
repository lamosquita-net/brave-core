/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/browser/ui/views/tabs/brave_browser_tab_strip_controller.h"

#include "brave/browser/ui/views/tabs/brave_tab_context_menu_contents.h"
#include "chrome/browser/ui/views/tabs/tab_strip.h"
#include "components/tabs/public/tab_interface.h"
#include "ui/base/mojom/menu_source_type.mojom-shared.h"

BraveBrowserTabStripController::BraveBrowserTabStripController(
    TabStripModel* model,
    BrowserView* browser_view)
    : BrowserTabStripController(model, browser_view) {}

BraveBrowserTabStripController::~BraveBrowserTabStripController() {
  if (context_menu_contents_) {
    context_menu_contents_->Cancel();
  }
}

const std::optional<int> BraveBrowserTabStripController::GetModelIndexOf(
    Tab* tab) {
  return tabstrip_->GetModelIndexOf(tab);
}

tabs::TabInterface* BraveBrowserTabStripController::GetTabInterface(
    const Tab* tab) {
  const std::optional<int> index = tabstrip_->GetModelIndexOf(tab);
  if (!index || !model_->ContainsIndex(*index)) {
    return nullptr;
  }
  return model_->GetTabAtIndex(*index);
}

bool BraveBrowserTabStripController::IsCommandEnabledForTab(
    TabStripModel::ContextMenuCommand command_id,
    const Tab* tab) {
  tabs::TabInterface* tab_interface = GetTabInterface(tab);
  return tab_interface &&
         IsContextMenuCommandEnabled(tab_interface, command_id);
}

void BraveBrowserTabStripController::ExecuteCommandForTab(
    TabStripModel::ContextMenuCommand command_id,
    const Tab* tab,
    int event_flags) {
  if (tabs::TabInterface* tab_interface = GetTabInterface(tab)) {
    ExecuteContextMenuCommand(tab_interface, command_id, event_flags);
  }
}

bool BraveBrowserTabStripController::GetAcceleratorForCommandId(
    int command_id,
    ui::Accelerator* accelerator) {
  return GetContextMenuAccelerator(command_id, accelerator);
}

void BraveBrowserTabStripController::ShowContextMenuForTab(
    Tab* tab,
    const gfx::Point& p,
    ui::mojom::MenuSourceType source_type) {
  const auto tab_index = tabstrip_->GetModelIndexOf(tab);
  if (!tab_index) {
    return;
  }
  context_menu_contents_ =
      std::make_unique<BraveTabContextMenuContents>(tab, this, *tab_index);
  context_menu_contents_->RunMenuAt(p, source_type);
}

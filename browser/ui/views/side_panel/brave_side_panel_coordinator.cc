/* Copyright (c) 2023 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "brave/browser/ui/views/side_panel/brave_side_panel_coordinator.h"

#include <optional>
#include <string>
#include <utility>

#include "base/debug/crash_logging.h"
#include "base/debug/dump_without_crashing.h"
#include "chrome/browser/ui/views/side_panel/side_panel_entry.h"

BraveSidePanelCoordinator::~BraveSidePanelCoordinator() = default;

void BraveSidePanelCoordinator::OnTabStripModelChanged(
    TabStripModel* tab_strip_model,
    const TabStripModelChange& change,
    const TabStripSelectionChange& selection) {
  const bool active_tab_changed = selection.active_tab_changed();

  // Code for prevent weird crash.
  // https://github.com/brave/brave-browser/issues/38331#issuecomment-2119901610
  if (active_tab_changed) {
    auto* old_contextual_registry =
        SidePanelRegistry::Get(selection.old_contents);
    // If old tab's WebContents has contextual registry, it should be in
    // observed list. If not, start observe as it's removed from base class'
    // method right after. Otherwise, we get crash.
    if (old_contextual_registry &&
        !registry_observations_.IsObservingSource(old_contextual_registry)) {
      LOG(ERROR) << __func__
                 << " de-activated tab's registry should be observed already!";
      registry_observations_.AddObservation(old_contextual_registry);
      base::debug::DumpWithoutCrashing();
    }
  }

  SidePanelCoordinator::OnTabStripModelChanged(tab_strip_model, change,
                                               selection);
}

void BraveSidePanelCoordinator::PopulateSidePanel(
    bool supress_animations,
    SidePanelEntry* entry,
    std::optional<std::unique_ptr<views::View>> content_view) {
  CHECK(entry);
  actions::ActionItem* const action_item = GetActionItem(entry->key());
  if (!action_item) {
    const std::string entry_id = SidePanelEntryIdToString(entry->key().id());
    LOG(ERROR) << __func__ << " no side panel action item for " << entry_id;
    SCOPED_CRASH_KEY_STRING64("SidePanel", "entry_id", entry_id);
    base::debug::DumpWithoutCrashing();
    return;
  }

  SidePanelCoordinator::PopulateSidePanel(supress_animations, entry,
                                          std::move(content_view));
}

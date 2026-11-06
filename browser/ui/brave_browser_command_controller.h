/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_BROWSER_UI_BRAVE_BROWSER_COMMAND_CONTROLLER_H_
#define BRAVE_BROWSER_UI_BRAVE_BROWSER_COMMAND_CONTROLLER_H_

#include <optional>
#include <string>

#include "base/memory/raw_ref.h"
#include "chrome/browser/ui/browser_command_controller.h"
#include "chrome/browser/ui/tabs/tab_strip_model_observer.h"
#include "components/prefs/pref_change_registrar.h"

class BraveAppMenuBrowserTest;
class BraveAppMenuModelBrowserTest;
class BraveBrowserCommandControllerTest;
enum class TabChangeType;

namespace content {
class WebContents;
}

// This namespace is needed for a chromium_src override
namespace chrome {

class BraveBrowserCommandController : public chrome::BrowserCommandController {
 public:
  explicit BraveBrowserCommandController(BrowserWindowInterface* browser);
  BraveBrowserCommandController(const BraveBrowserCommandController&) = delete;
  BraveBrowserCommandController& operator=(
      const BraveBrowserCommandController&) = delete;
  ~BraveBrowserCommandController() override;

 protected:
  void OnTabChangedAt(tabs::TabInterface* tab,
                      TabChangeType change_type) override;
  void OnTabPinnedStateChanged(tabs::TabInterface* tab, int index) override;
  void OnTabStripModelChanged(
      TabStripModel* tab_strip_model,
      const TabStripModelChange& change,
      const TabStripSelectionChange& selection) override;
  void OnTabGroupChanged(const TabGroupChange& change) override;

 private:
  friend class ::BraveAppMenuBrowserTest;
  friend class ::BraveAppMenuModelBrowserTest;
  friend class ::BraveBrowserCommandControllerTest;

  // Overriden from CommandUpdater:
  bool SupportsCommand(int id) const override;
  bool IsCommandEnabled(int id) const override;
  bool ExecuteCommandWithDispositionAndContext(
      int id,
      WindowOpenDisposition disposition,
      std::optional<actions::ActionInvocationContext> context,
      base::TimeTicks time_stamp) override;
  void AddCommandObserver(int id, CommandObserver* observer) override;
  void RemoveCommandObserver(int id, CommandObserver* observer) override;
  void RemoveCommandObserver(CommandObserver* observer) override;
  bool UpdateCommandEnabled(int id, bool state) override;

  void InitBraveCommandState();
  void UpdateCommandForBraveRewards();
  void UpdateCommandForWebcompatReporter();
  void UpdateCommandForBraveSync();
  void UpdateCommandsForTabs();
  void UpdateCommandsForSend();
  void UpdateCommandsForPin();

  bool ExecuteBraveCommandWithDisposition(int id,
                                          WindowOpenDisposition disposition,
                                          base::TimeTicks time_stamp);
  const raw_ref<BrowserWindowInterface> browser_;

  CommandUpdaterImpl brave_command_updater_;
};

}  // namespace chrome

#endif  // BRAVE_BROWSER_UI_BRAVE_BROWSER_COMMAND_CONTROLLER_H_

/* Copyright (c) 2022 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/browser/ui/views/frame/brave_browser_native_widget_mac.h"

#include "base/feature_list.h"
#include "brave/app/brave_command_ids.h"
#include "brave/browser/ui/tabs/brave_tab_prefs.h"
#include "brave/browser/ui/tabs/features.h"
#include "chrome/app/chrome_command_ids.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/browser/ui/browser_window/public/browser_window_interface.h"
#include "chrome/browser/ui/tabs/tab_strip_model.h"
#include "chrome/browser/ui/views/frame/browser_view.h"
#include "components/prefs/pref_service.h"
#include "components/remote_cocoa/common/native_widget_ns_window_host.mojom.h"

// 156 renamed BrowserFrameMac to BrowserNativeWidgetMac (as in Brave 1.98).
BraveBrowserNativeWidgetMac::BraveBrowserNativeWidgetMac(
    BrowserWidget* browser_widget,
    BrowserView* browser_view)
    : BrowserNativeWidgetMac(browser_widget, browser_view),
      browser_view_(browser_view->GetAsWeakPtr()) {}

BraveBrowserNativeWidgetMac::~BraveBrowserNativeWidgetMac() = default;

void BraveBrowserNativeWidgetMac::ValidateUserInterfaceItem(
    int32_t tag,
    remote_cocoa::mojom::ValidateUserInterfaceItemResult* result) {
  BrowserNativeWidgetMac::ValidateUserInterfaceItem(tag, result);

  if (tag != IDC_TOGGLE_TAB_MUTE || !result->enable) {
    return;
  }

  // Update toggle state for tab mute menu bar entry.
  if (BrowserView* browser_view = browser_view_.get()) {
    TabStripModel* model = browser_view->browser()->GetTabStripModel();
    result->set_toggle_state = true;
    result->new_toggle_state = !model->empty() &&
                               model->GetActiveWebContents() &&
                               model->GetActiveWebContents()->IsAudioMuted();
  }
}

bool BraveBrowserNativeWidgetMac::ExecuteCommand(
    int32_t command,
    WindowOpenDisposition window_open_disposition,
    bool is_before_first_responder) {
  if (base::FeatureList::IsEnabled(tabs::features::kBraveSharedPinnedTabs)) {
    // is_before_first_responder tells whether or not the app/window was in
    // focus while the keyboard command was fired. In current method, it helps
    // in distinguishing the 'file -> close tab' (false, as toolbar was in
    // focus) command from 'ctrl + w' (true, as tab was in focus) command.
    if (BrowserView* browser_view = browser_view_.get()) {
      BrowserWindowInterface* browser = browser_view->browser();
      if (browser->GetProfile()->GetPrefs()->GetBoolean(
              brave_tabs::kSharedPinnedTab) &&
          command == IDC_CLOSE_TAB && is_before_first_responder &&
          browser->GetTabStripModel()->IsTabPinned(
              browser->GetTabStripModel()->active_index())) {
        // Ignoring the ctrl+w command when the active tab is shared pinned
        return true;
      }
    }
  }

  return BrowserNativeWidgetMac::ExecuteCommand(
      command, window_open_disposition, is_before_first_responder);
}

/* Copyright (c) 2023 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "chrome/browser/ui/unload_controller.h"

#include "base/feature_list.h"
#include "brave/browser/ui/tabs/features.h"
#include "brave/browser/ui/tabs/shared_pinned_tab_service.h"
#include "brave/browser/ui/tabs/shared_pinned_tab_service_factory.h"
#include "brave/components/constants/pref_names.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/browser/ui/browser_window/public/browser_window_interface.h"
#include "components/prefs/pref_service.h"

namespace {

// 156 removed BrowserListObserver::OnBrowserClosing(): SharedPinnedTabService
// gets a chance to detach/cache shared pinned tabs before CloseAllTabs() runs,
// otherwise they would close like ordinary tabs (as in Brave 1.98).
bool MaybeHandleSharedPinnedTabsClosing(BrowserWindowInterface* browser) {
  if (!base::FeatureList::IsEnabled(tabs::features::kBraveSharedPinnedTabs)) {
    return false;
  }
  auto* shared_pinned_tab_service =
      SharedPinnedTabServiceFactory::GetForProfile(browser->GetProfile());
  if (!shared_pinned_tab_service) {
    return false;
  }
  // When there are only pinned tabs, OnWindowClosing() will be called again,
  // after we detach all pinned tabs from the browser from
  // shared_pinned_tab_service->BrowserClosing()
  return shared_pinned_tab_service->BrowserClosing(browser->GetTabStripModel());
}

}  // namespace

// The patch also keeps the window open when its last tab closes unless
// kEnableClosingLastTab is set (BraveBrowser::TabStripEmpty() opens a new tab),
// and holds the "confirmed to close" state that BraveBrowser used to keep.
#include "src/chrome/browser/ui/unload_controller.cc"

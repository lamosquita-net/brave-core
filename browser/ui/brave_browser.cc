/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/browser/ui/brave_browser.h"

#include <memory>
#include <optional>
#include <utility>

#include "base/feature_list.h"
#include "base/functional/bind.h"
#include "base/functional/callback_helpers.h"
#include "brave/browser/ui/brave_browser_window.h"
#include "brave/browser/ui/tabs/brave_tab_prefs.h"
#include "brave/browser/ui/tabs/features.h"
#include "brave/components/constants/pref_names.h"
#include "chrome/browser/lifetime/browser_close_manager.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/browser/search/search.h"
#include "chrome/browser/ui/browser_commands.h"
#include "chrome/browser/ui/browser_tabstrip.h"
#include "chrome/browser/ui/browser_window/public/profile_browser_collection.h"
#include "chrome/browser/ui/tabs/tab_strip_model.h"
#include "chrome/browser/ui/tabs/tab_strip_model_observer.h"
#include "chrome/browser/ui/unload_controller.h"
#include "components/prefs/pref_service.h"
#include "components/tabs/public/tab_interface.h"

namespace {

bool g_suppress_dialog_for_testing = false;

}  // namespace

// static
void BraveBrowser::SuppressBrowserWindowClosingDialogForTesting(bool suppress) {
  g_suppress_dialog_for_testing = suppress;
}

BraveBrowser::BraveBrowser(BrowserWindowCreateParams params)
    : Browser(std::move(params)) {}

BraveBrowser::~BraveBrowser() = default;

void BraveBrowser::OnTabClosing(tabs::TabInterface* tab,
                                bool* had_active_modal_dialog) {
  Browser::OnTabClosing(tab, had_active_modal_dialog);

  if (!AreAllTabsSharedPinnedTabs()) {
    return;
  }

  int tabbed_browser_count = 0;
  ProfileBrowserCollection::GetForProfile(GetProfile())
      ->ForEach([&tabbed_browser_count](BrowserWindowInterface* browser) {
        if (browser->GetType() == BrowserWindowInterface::TYPE_NORMAL) {
          ++tabbed_browser_count;
        }
        return tabbed_browser_count < 2;
      });
  if (tabbed_browser_count > 1) {
    base::SequencedTaskRunner::GetCurrentDefault()->PostTask(
        FROM_HERE, base::BindOnce(
                       [](base::WeakPtr<BraveBrowser> browser) {
                         if (browser) {
                           // We don't want close confirm dialog to show up. In
                           // this case, Shared pinned tabs will be moved to
                           // another window, so we don't have to warn users.
                           browser->set_confirmed_to_close(true);
                           chrome::CloseWindow(browser.get());
                         }
                       },
                       weak_ptr_factory_.GetWeakPtr()));
  }
}

void BraveBrowser::TabStripEmpty() {
  if (UnloadController::From(this)->is_attempting_to_close_browser() ||
      GetType() != BrowserWindowInterface::TYPE_NORMAL ||
      ignore_enable_closing_last_tab_pref_) {
    Browser::TabStripEmpty();
    return;
  }

  base::SequencedTaskRunner::GetCurrentDefault()->PostTask(
      FROM_HERE, base::BindOnce(
                     [](base::WeakPtr<BraveBrowser> browser) {
                       if (browser) {
                         chrome::AddTabAt(browser.get(),
                                          chrome::GetNewTabURL(browser.get()),
                                          /*index=*/-1, /*foreground=*/true,
                                          /*group=*/std::nullopt,
                                          /*pinned=*/false);
                       }
                     },
                     weak_ptr_factory_.GetWeakPtr()));
}

void BraveBrowser::set_confirmed_to_close(bool close) {
  UnloadController::From(this)->set_confirmed_to_close(close);
}

bool BraveBrowser::ShouldAskForBrowserClosingBeforeHandlers() {
  if (g_suppress_dialog_for_testing) {
    return false;
  }

  // Don't need to ask when application closing is in-progress.
  if (BrowserCloseManager::BrowserClosingStarted()) {
    return false;
  }

  if (UnloadController::From(this)->confirmed_to_close()) {
    return false;
  }

  PrefService* prefs = GetProfile()->GetPrefs();
  if (!prefs->GetBoolean(kEnableWindowClosingConfirm)) {
    return false;
  }

  // Only launch confirm dialog while closing when browser has multiple tabs.
  return tab_strip_model()->count() > 1;
}

bool BraveBrowser::AreAllTabsSharedPinnedTabs() {
  if (!base::FeatureList::IsEnabled(tabs::features::kBraveSharedPinnedTabs)) {
    return false;
  }

  if (GetType() != BrowserWindowInterface::TYPE_NORMAL) {
    return false;
  }

  if (!GetProfile()->GetPrefs()->GetBoolean(brave_tabs::kSharedPinnedTab)) {
    return false;
  }

  return tab_strip_model()->count() > 0 &&
         tab_strip_model()->count() ==
             tab_strip_model()->IndexOfFirstNonPinnedTab();
}

BraveBrowserWindow* BraveBrowser::brave_window() {
  return BraveBrowserWindow::From(window_.get());
}

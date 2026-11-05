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
#include "brave/browser/brave_browser_features.h"
#include "brave/browser/ui/brave_browser_window.h"
#include "brave/browser/ui/tabs/brave_tab_prefs.h"
#include "brave/browser/ui/tabs/features.h"
#include "brave/components/constants/pref_names.h"
#include "chrome/browser/lifetime/browser_close_manager.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/browser/search/search.h"
#include "chrome/browser/ui/browser_commands.h"
#include "chrome/browser/ui/browser_finder.h"
#include "chrome/browser/ui/browser_tabstrip.h"
#include "chrome/browser/ui/tabs/tab_strip_model.h"
#include "chrome/common/webui_url_constants.h"
#include "components/prefs/pref_service.h"
#include "content/public/common/url_constants.h"
#include "url/gurl.h"

namespace {

bool g_suppress_dialog_for_testing = false;

}  // namespace

// static
void BraveBrowser::SuppressBrowserWindowClosingDialogForTesting(bool suppress) {
  g_suppress_dialog_for_testing = suppress;
}

// static
bool BraveBrowser::ShouldUseBraveWebViewRoundedCorners(Browser* browser) {
  return base::FeatureList::IsEnabled(features::kBraveWebViewRoundedCorners) &&
         browser->is_type_normal();
}

BraveBrowser::BraveBrowser(const CreateParams& params) : Browser(params) {}

BraveBrowser::~BraveBrowser() = default;

void BraveBrowser::OnTabClosing(content::WebContents* contents) {
  Browser::OnTabClosing(contents);

  if (!AreAllTabsSharedPinnedTabs()) {
    return;
  }

  if (chrome::FindAllTabbedBrowsersWithProfile(profile()).size() > 1) {
    base::SequencedTaskRunner::GetCurrentDefault()->PostTask(
        FROM_HERE, base::BindOnce(
                       [](base::WeakPtr<BraveBrowser> browser) {
                         if (browser) {
                           // We don't want close confirm dialog to show up. In
                           // this case, Shared pinned tabs will be moved to
                           // another window, so we don't have to warn users.
                           browser->confirmed_to_close_ = true;
                           chrome::CloseWindow(browser.get());
                         }
                       },
                       weak_ptr_factory_.GetWeakPtr()));
  }
}

void BraveBrowser::TabStripEmpty() {
  if (unload_controller_.is_attempting_to_close_browser() ||
      !is_type_normal() || ignore_enable_closing_last_tab_pref_) {
    Browser::TabStripEmpty();
    return;
  }

  base::SequencedTaskRunner::GetCurrentDefault()->PostTask(
      FROM_HERE, base::BindOnce(&chrome::AddTabAt, this, GetNewTabURL(), -1,
                                true, std::nullopt));
}

bool BraveBrowser::ShouldDisplayFavicon(
    content::WebContents* web_contents) const {
  // Override to not show favicon for NTP in tab.

  // Suppress the icon for the new-tab page, even if a navigation to it is
  // not committed yet. Note that we're looking at the visible URL, so
  // navigations from NTP generally don't hit this case and still show an icon.
  GURL url = web_contents->GetVisibleURL();
  if (url.SchemeIs(content::kChromeUIScheme) &&
      url.host() == chrome::kChromeUINewTabHost) {
    return false;
  }

  // Also suppress instant-NTP. This does not use search::IsInstantNTP since
  // it looks at the last-committed entry and we need to show icons for pending
  // navigations away from it.
  if (search::IsInstantNTPURL(url, profile())) {
    return false;
  }

  return Browser::ShouldDisplayFavicon(web_contents);
}

void BraveBrowser::FinishWarnBeforeClosing(WarnBeforeClosingResult result) {
  // Clear user's choice because user cancelled window closing by some
  // warning(ex, download is in-progress).
  if (result == WarnBeforeClosingResult::kDoNotClose) {
    confirmed_to_close_ = false;
  }
  Browser::FinishWarnBeforeClosing(result);
}

void BraveBrowser::BeforeUnloadFired(content::WebContents* source,
                                     bool proceed,
                                     bool* proceed_to_fire_unload) {
  // Clear user's choice when user cancelled window closing by beforeunload
  // handler.
  if (!proceed) {
    confirmed_to_close_ = false;
  }
  Browser::BeforeUnloadFired(source, proceed, proceed_to_fire_unload);
}

bool BraveBrowser::TryToCloseWindow(
    bool skip_beforeunload,
    const base::RepeatingCallback<void(bool)>& on_close_confirmed) {
  // Window closing could be asked directly to browser object by this method.
  // For example, when user tries to delete profile, this method is called on
  // all its browser object. After all handlers are done, its all browser window
  // start to close. In this case, we should not ask to users about this
  // closing. So, treats like user confirmed closing. If this try blocked by
  // user, |confirmed_to_close_| is set to false by ResetTryToCloseWindow().
  confirmed_to_close_ = true;
  return Browser::TryToCloseWindow(skip_beforeunload,
                                   std::move(on_close_confirmed));
}

void BraveBrowser::ResetTryToCloseWindow() {
  confirmed_to_close_ = false;
  Browser::ResetTryToCloseWindow();
}

void BraveBrowser::UpdateTargetURL(content::WebContents* source,
                                   const GURL& url) {
  GURL target_url = url;
  if (url.SchemeIs(content::kChromeUIScheme)) {
    GURL::Replacements replacements;
    replacements.SetSchemeStr(content::kBraveUIScheme);
    target_url = target_url.ReplaceComponents(replacements);
  }
  Browser::UpdateTargetURL(source, target_url);
}

bool BraveBrowser::ShouldAskForBrowserClosingBeforeHandlers() {
  if (g_suppress_dialog_for_testing) {
    return false;
  }

  // Don't need to ask when application closing is in-progress.
  if (BrowserCloseManager::BrowserClosingStarted()) {
    return false;
  }

  if (confirmed_to_close_) {
    return false;
  }

  PrefService* prefs = profile()->GetPrefs();
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

  if (!is_type_normal()) {
    return false;
  }

  if (!profile()->GetPrefs()->GetBoolean(brave_tabs::kSharedPinnedTab)) {
    return false;
  }

  return tab_strip_model()->count() > 0 &&
         tab_strip_model()->count() ==
             tab_strip_model()->IndexOfFirstNonPinnedTab();
}

BraveBrowserWindow* BraveBrowser::brave_window() {
  return static_cast<BraveBrowserWindow*>(window_);
}

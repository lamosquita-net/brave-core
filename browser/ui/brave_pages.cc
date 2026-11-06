/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/browser/ui/brave_pages.h"

#include "base/strings/strcat.h"
#include "brave/components/constants/url_constants.h"
#include "brave/components/constants/webui_url_constants.h"
#include "chrome/browser/ui/browser_window/public/browser_window_interface.h"
#include "chrome/browser/ui/chrome_pages.h"
#include "chrome/browser/ui/navigator/browser_navigator_params.h"
#include "chrome/browser/ui/singleton_tabs.h"
#include "chrome/browser/ui/tabs/tab_strip_model.h"
#include "chrome/common/webui_url_constants.h"
#include "content/public/browser/page_navigator.h"
#include "content/public/browser/web_contents.h"
#include "ui/base/page_transition_types.h"
#include "ui/base/window_open_disposition.h"
#include "url/gurl.h"

namespace brave {

namespace {
constexpr char kBraveTalkURL[] = "https://talk.brave.com/widget";
}  // namespace

void ShowBraveRewards(BrowserWindowInterface* browser) {
  ShowSingletonTabOverwritingNTP(browser, GURL(kRewardsPageURL));
}

void ShowBraveAdblock(BrowserWindowInterface* browser) {
  ShowSingletonTabOverwritingNTP(browser, GURL(kBraveUIAdblockURL));
}

void ShowSync(BrowserWindowInterface* browser) {
  ShowSingletonTabOverwritingNTP(
      browser, chrome::GetSettingsUrl(chrome::kSyncSetupSubPage));
}

void ShowShortcutsPage(BrowserWindowInterface* browser) {
  ShowSingletonTabOverwritingNTP(browser, GURL(kShortcutsURL));
}

void ShowBraveTalk(BrowserWindowInterface* browser) {
  ShowSingletonTabOverwritingNTP(browser, GURL(kBraveTalkURL));
}

void ShowWebcompatReporter(BrowserWindowInterface* browser) {
  content::WebContents* web_contents =
      browser->GetTabStripModel()->GetActiveWebContents();
  if (!web_contents) {
    return;
  }

  OpenFlyWebReportProblemPage(web_contents);
}

// FlyWeb: Brave's reporter (components/webcompat_reporter, removed) sent the
// site and the Shields settings to webcompat.brave.com. FlyWeb opens its own
// page instead, which says how to report a broken site to us.
void OpenFlyWebReportProblemPage(content::WebContents* initiator) {
  initiator->OpenURL(
      content::OpenURLParams(GURL(kFlyWebReportProblemURL), content::Referrer(),
                             WindowOpenDisposition::NEW_FOREGROUND_TAB,
                             ui::PAGE_TRANSITION_LINK,
                             /*is_renderer_initiated=*/false),
      /*navigation_handle_callback=*/{});
}

void ShowExtensionSettings(BrowserWindowInterface* browser) {
  ShowSingletonTabOverwritingNTP(browser, GURL(kExtensionSettingsURL));
}

}  // namespace brave

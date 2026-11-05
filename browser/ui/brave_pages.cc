/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/browser/ui/brave_pages.h"

#include "base/strings/strcat.h"
#include "brave/components/brave_wallet/common/brave_wallet.mojom.h"
#include "brave/components/constants/url_constants.h"
#include "brave/components/constants/webui_url_constants.h"
#include "brave/components/sidebar/browser/constants.h"
#include "chrome/browser/ui/browser.h"
#include "chrome/browser/ui/browser_navigator_params.h"
#include "chrome/browser/ui/chrome_pages.h"
#include "chrome/browser/ui/singleton_tabs.h"
#include "chrome/common/webui_url_constants.h"
#include "content/public/browser/page_navigator.h"
#include "content/public/browser/web_contents.h"
#include "ui/base/page_transition_types.h"
#include "ui/base/window_open_disposition.h"
#include "url/gurl.h"

namespace brave {

void ShowBraveRewards(Browser* browser) {
  ShowSingletonTabOverwritingNTP(browser, GURL(kRewardsPageURL));
}

void ShowBraveAdblock(Browser* browser) {
  ShowSingletonTabOverwritingNTP(browser, GURL(kBraveUIAdblockURL));
}

void ShowSync(Browser* browser) {
  ShowSingletonTabOverwritingNTP(
      browser, chrome::GetSettingsUrl(chrome::kSyncSetupSubPage));
}

void ShowShortcutsPage(Browser* browser) {
  ShowSingletonTabOverwritingNTP(browser, GURL(kShortcutsURL));
}

void ShowBraveTalk(Browser* browser) {
  ShowSingletonTabOverwritingNTP(browser, GURL(sidebar::kBraveTalkURL));
}

void ShowWebcompatReporter(Browser* browser) {
  content::WebContents* web_contents =
      browser->tab_strip_model()->GetActiveWebContents();
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

void ShowBraveWallet(Browser* browser) {
  ShowSingletonTabOverwritingNTP(browser, GURL(kBraveUIWalletURL));
}

void ShowBraveWalletOnboarding(Browser* browser) {
  ShowSingletonTabOverwritingNTP(browser, GURL(kBraveUIWalletOnboardingURL));
}

void ShowBraveWalletAccountCreation(Browser* browser,
                                    brave_wallet::mojom::CoinType coin_type) {
  // Only solana is supported.
  if (coin_type == brave_wallet::mojom::CoinType::SOL) {
    ShowSingletonTabOverwritingNTP(
        browser,
        GURL(base::StrCat({kBraveUIWalletAccountCreationURL, "Solana"})));
  } else {
    NOTREACHED_IN_MIGRATION();
  }
}

void ShowExtensionSettings(Browser* browser) {
  ShowSingletonTabOverwritingNTP(browser, GURL(kExtensionSettingsURL));
}

void ShowWalletSettings(Browser* browser) {
  ShowSingletonTabOverwritingNTP(browser, GURL(kWalletSettingsURL));
}

}  // namespace brave

/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_BROWSER_UI_BRAVE_PAGES_H_
#define BRAVE_BROWSER_UI_BRAVE_PAGES_H_

class BrowserWindowInterface;

namespace content {
class WebContents;
}  // namespace content

namespace brave {

void ShowBraveAdblock(BrowserWindowInterface* browser);
void ShowWebcompatReporter(BrowserWindowInterface* browser);
// FlyWeb: opens flyweb.lamosquita.net's "report a problem" page in a new tab.
void OpenFlyWebReportProblemPage(content::WebContents* initiator);
void ShowBraveRewards(BrowserWindowInterface* browser);
void ShowBraveWallet(BrowserWindowInterface* browser);
void ShowBraveWalletOnboarding(BrowserWindowInterface* browser);
void ShowExtensionSettings(BrowserWindowInterface* browser);
void ShowWalletSettings(BrowserWindowInterface* browser);
void ShowSync(BrowserWindowInterface* browser);
void ShowShortcutsPage(BrowserWindowInterface* browser);
void ShowBraveTalk(BrowserWindowInterface* browser);

}  // namespace brave

#endif  // BRAVE_BROWSER_UI_BRAVE_PAGES_H_

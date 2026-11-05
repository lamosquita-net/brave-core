/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_BROWSER_UI_BRAVE_BROWSER_H_
#define BRAVE_BROWSER_UI_BRAVE_BROWSER_H_

#include <memory>

#include "base/memory/weak_ptr.h"
#include "chrome/browser/ui/browser.h"

class BraveBrowserWindow;

namespace content {
class WebContents;
}  // namespace content

class BraveBrowser : public Browser {
 public:
  explicit BraveBrowser(BrowserWindowCreateParams params);
  ~BraveBrowser() override;

  BraveBrowser(const BraveBrowser&) = delete;
  BraveBrowser& operator=(const BraveBrowser&) = delete;

  static bool ShouldUseBraveWebViewRoundedCorners(
      BrowserWindowInterface* browser);

  // Browser overrides:
  // 156 moved FinishWarnBeforeClosing(), TryToCloseWindow() and
  // ResetTryToCloseWindow() to UnloadController (confirmed_to_close lives there
  // now, as in Brave 1.98), and ShouldDisplayFavicon(), BeforeUnloadFired() and
  // UpdateTargetURL() out of Browser.
  void OnTabClosing(tabs::TabInterface* tab,
                    bool* had_active_modal_dialog) override;
  void TabStripEmpty() override;

  // Returns true when we should ask browser closing to users before handling
  // any warning/onbeforeunload handlers.
  bool ShouldAskForBrowserClosingBeforeHandlers();

  BraveBrowserWindow* brave_window();

  void set_confirmed_to_close(bool close);

  void set_ignore_enable_closing_last_tab_pref() {
    ignore_enable_closing_last_tab_pref_ = true;
  }

 private:
  friend class BraveTestLauncherDelegate;
  friend class WindowClosingConfirmBrowserTest;
  friend class InProcessBrowserTest;

  // static
  static void SuppressBrowserWindowClosingDialogForTesting(bool suppress);

  bool AreAllTabsSharedPinnedTabs();

  // When "kEnableClosingLastTab" is false, browser will try to add new tab in
  // TabStripEmpty() if there is no tab. But, in some cases, we should not add
  // new tab, like when user tries to "Bring all tabs" to other window.
  bool ignore_enable_closing_last_tab_pref_ = false;

  base::WeakPtrFactory<BraveBrowser> weak_ptr_factory_{this};
};

#endif  // BRAVE_BROWSER_UI_BRAVE_BROWSER_H_

/* Copyright (c) 2026 lamosquita. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

// FlyWeb (T8, Chromium 156): with JavaScript blocked everywhere in Shields,
// internal pages and extensions keep running their scripts.
//
// Up to 128 this relied on an exception for allowlisted schemes in
// BraveContentSettingsAgentImpl::AllowScriptFromSource. 156 decides
// JavaScript in the browser process: HostContentSettingsMap returns ALLOW for
// the JAVASCRIPT allowlisted primary schemes (chrome, chrome-untrusted,
// devtools, chrome-extension; content_settings_registry.cc), so the renderer
// exception is gone, as in Brave 1.98. This test pins that behaviour.

#include <string>
#include <string_view>

#include "base/strings/strcat.h"
#include "brave/components/brave_shields/content/browser/brave_shields_util.h"
#include "chrome/browser/content_settings/host_content_settings_map_factory.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/browser/ui/browser.h"
#include "chrome/browser/ui/tabs/tab_strip_model.h"
#include "chrome/test/base/in_process_browser_test.h"
#include "chrome/test/base/ui_test_utils.h"
#include "components/content_settings/core/browser/host_content_settings_map.h"
#include "components/content_settings/core/common/content_settings.h"
#include "components/content_settings/core/common/content_settings_types.h"
#include "content/public/test/browser_test.h"
#include "content/public/test/browser_test_utils.h"
#include "url/gurl.h"

namespace {

// Resolves to true once the page's own scripts have run (they define the
// custom element or render the React root); false after ~5 s otherwise.
std::string PageScriptsRan(std::string_view condition) {
  return base::StrCat({R"(
  new Promise(resolve => {
    let tries = 0;
    const check = () => {
      if ()",
                       condition, R"() { resolve(true); return; }
      if (++tries > 50) { resolve(false); return; }
      setTimeout(check, 100);
    };
    check();
  }))"});
}

}  // namespace

class FlyWebScriptBlockInternalPagesBrowserTest : public InProcessBrowserTest {
 public:
  void SetUpOnMainThread() override {
    InProcessBrowserTest::SetUpOnMainThread();
    // Shields: block scripts everywhere (global default).
    brave_shields::SetNoScriptControlType(map(), brave_shields::BLOCK, GURL());
    ASSERT_EQ(CONTENT_SETTING_BLOCK,
              map()->GetContentSetting(GURL("https://a.test/"), GURL(),
                                       ContentSettingsType::JAVASCRIPT));
  }

 protected:
  HostContentSettingsMap* map() {
    return HostContentSettingsMapFactory::GetForProfile(browser()->profile());
  }

  bool NavigateAndCheck(const GURL& url, const char* condition) {
    if (!ui_test_utils::NavigateToURL(browser(), url)) {
      return false;
    }
    content::WebContents* web_contents =
        browser()->tab_strip_model()->GetActiveWebContents();
    return content::EvalJs(web_contents, PageScriptsRan(condition))
        .ExtractBool();
  }
};

IN_PROC_BROWSER_TEST_F(FlyWebScriptBlockInternalPagesBrowserTest,
                       AllowlistedSchemes) {
  for (const char* url :
       {"chrome://settings/", "chrome://newtab/", "chrome://extensions/",
        "chrome-untrusted://terms/", "devtools://devtools/",
        "chrome-extension://aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa/page.html"}) {
    EXPECT_EQ(CONTENT_SETTING_ALLOW,
              map()->GetContentSetting(GURL(url), GURL(url),
                                       ContentSettingsType::JAVASCRIPT))
        << url;
  }
}

IN_PROC_BROWSER_TEST_F(FlyWebScriptBlockInternalPagesBrowserTest, Settings) {
  EXPECT_TRUE(NavigateAndCheck(GURL("chrome://settings/"),
                               "!!customElements.get('settings-ui')"));
}

IN_PROC_BROWSER_TEST_F(FlyWebScriptBlockInternalPagesBrowserTest, NewTab) {
  // FlyWeb's new tab page (brave_new_tab_ui) renders into #root.
  EXPECT_TRUE(NavigateAndCheck(
      GURL("chrome://newtab/"),
      "document.getElementById('root') && "
      "document.getElementById('root').childElementCount > 0"));
}

IN_PROC_BROWSER_TEST_F(FlyWebScriptBlockInternalPagesBrowserTest, Extensions) {
  EXPECT_TRUE(NavigateAndCheck(GURL("chrome://extensions/"),
                               "!!customElements.get('extensions-manager')"));
}

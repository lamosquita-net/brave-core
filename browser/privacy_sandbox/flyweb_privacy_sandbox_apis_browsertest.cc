/* Copyright (c) 2026 lamosquita. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

// FlyWeb (T8, Chromium 156): the Privacy Sandbox web APIs must not exist.
// Up to 128 they were turned off with blink::features overrides; in 156 most
// of those features moved to runtime_enabled_features.json5 (patched) or to
// network/content/attribution features (chromium_src overrides). If an
// upstream change re-enables one of them, this test says which.
// Manual twin for LOCAL: softmac FlyWeb/tools/privacy-sandbox.html.

#include "chrome/browser/ui/browser.h"
#include "chrome/browser/ui/tabs/tab_strip_model.h"
#include "chrome/test/base/in_process_browser_test.h"
#include "chrome/test/base/ui_test_utils.h"
#include "content/public/test/browser_test.h"
#include "content/public/test/browser_test_utils.h"
#include "net/dns/mock_host_resolver.h"
#include "net/test/embedded_test_server/embedded_test_server.h"

namespace {

// Each expression is true if the API is exposed. All must be false.
constexpr const char* kExposed[] = {
    // Shared Storage.
    "'sharedStorage' in window",
    "typeof SharedStorage !== 'undefined'",
    // Fenced frames.
    "'fence' in window",
    "typeof HTMLFencedFrameElement !== 'undefined'",
    "typeof FencedFrameConfig !== 'undefined'",
    // Protected Audience (FLEDGE, interest groups, Parakeet).
    "'runAdAuction' in navigator",
    "'joinAdInterestGroup' in navigator",
    "'leaveAdInterestGroup' in navigator",
    "'clearOriginJoinedAdInterestGroups' in navigator",
    "'createAuctionNonce' in navigator",
    "'adAuctionComponents' in navigator",
    "'deprecatedURNToURL' in navigator",
    "'deprecatedReplaceInURN' in navigator",
    // Topics.
    "'browsingTopics' in document",
    // Private Aggregation (worklet-only global; checked anyway).
    "'privateAggregation' in window",
    // Attribution Reporting.
    "'attributionSrc' in HTMLAnchorElement.prototype",
    "'attributionSrc' in HTMLImageElement.prototype",
    "'attributionSrc' in HTMLScriptElement.prototype",
    // Private State Tokens (Trust Tokens).
    "'hasPrivateToken' in document",
    "'hasRedemptionRecord' in document",
};

}  // namespace

class FlyWebPrivacySandboxApisBrowserTest : public InProcessBrowserTest {
 public:
  void SetUpOnMainThread() override {
    InProcessBrowserTest::SetUpOnMainThread();
    host_resolver()->AddRule("*", "127.0.0.1");
    https_server_.ServeFilesFromSourceDirectory("chrome/test/data");
    ASSERT_TRUE(https_server_.Start());
  }

 protected:
  net::EmbeddedTestServer https_server_{
      net::EmbeddedTestServer::TYPE_HTTPS};
};

IN_PROC_BROWSER_TEST_F(FlyWebPrivacySandboxApisBrowserTest, ApisDoNotExist) {
  ASSERT_TRUE(ui_test_utils::NavigateToURL(
      browser(), https_server_.GetURL("a.test", "/simple.html")));
  content::WebContents* web_contents =
      browser()->tab_strip_model()->GetActiveWebContents();
  for (const char* expression : kExposed) {
    EXPECT_EQ(false, content::EvalJs(web_contents, expression)) << expression;
  }
}

/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/browser/brave_content_browser_client.h"

#include <memory>
#include <vector>

#include "base/path_service.h"
#include "base/strings/strcat.h"
#include "base/strings/string_util.h"
#include "base/strings/utf_string_conversions.h"
#include "base/test/bind.h"
#include "brave/components/brave_shields/core/common/brave_shield_constants.h"
#include "brave/components/constants/brave_paths.h"
#include "brave/components/constants/pref_names.h"
#include "brave/components/constants/webui_url_constants.h"
#include "chrome/browser/content_settings/host_content_settings_map_factory.h"
#include "chrome/browser/extensions/component_loader.h"
#include "chrome/browser/extensions/extension_service.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/browser/ui/browser_window.h"
#include "chrome/browser/ui/browser_window/public/browser_window_features.h"
#include "chrome/common/buildflags.h"
#include "chrome/common/webui_url_constants.h"
#include "chrome/test/base/in_process_browser_test.h"
#include "chrome/test/base/ui_test_utils.h"
#include "components/content_settings/core/browser/host_content_settings_map.h"
#include "components/content_settings/core/common/content_settings_types.h"
#include "components/omnibox/browser/location_bar_model.h"
#include "components/prefs/pref_service.h"
#include "content/public/browser/navigation_entry.h"
#include "content/public/common/content_client.h"
#include "content/public/test/browser_test.h"
#include "content/public/test/browser_test_utils.h"
#include "extensions/browser/extension_registry.h"
#include "extensions/browser/extension_system.h"
#include "extensions/common/constants.h"
#include "net/dns/mock_host_resolver.h"
#include "url/origin.h"

class BraveContentBrowserClientTest : public InProcessBrowserTest {
 public:
  void SetUp() override {
    // This is needed because component extensions are not added by default
    // without it.  Theyu are found to interfere with tests otherwise. It's
    // needed for loading the hangouts extension of which there are tests for
    // below.
    extensions::ComponentLoader::EnableBackgroundExtensionsForTesting();
    InProcessBrowserTest::SetUp();
  }

  void SetUpOnMainThread() override {
    InProcessBrowserTest::SetUpOnMainThread();

    browser_content_client_ = std::make_unique<BraveContentBrowserClient>();
    content::SetBrowserClientForTesting(browser_content_client_.get());

    host_resolver()->AddRule("*", "127.0.0.1");
    content::SetupCrossSiteRedirector(embedded_test_server());

    base::FilePath test_data_dir;
    base::PathService::Get(brave::DIR_TEST_DATA, &test_data_dir);
    embedded_test_server()->ServeFilesFromDirectory(test_data_dir);

    ASSERT_TRUE(embedded_test_server()->Start());
  }

  void TearDown() override { browser_content_client_.reset(); }

  void NavigateToURLAndWaitForRewrites(content::WebContents* contents,
                                       const GURL& original_url,
                                       const GURL& final_url) {
    ui_test_utils::UrlLoadObserver load_complete(final_url);
    browser()->OpenURL(
        content::OpenURLParams(original_url, content::Referrer(),
                               WindowOpenDisposition::CURRENT_TAB,
                               ui::PAGE_TRANSITION_TYPED, false),
        /*navigation_handle_callback=*/{});
    load_complete.Wait();
    EXPECT_EQ(contents->GetLastCommittedURL(), final_url);
  }

  content::ContentBrowserClient* client() {
    return browser_content_client_.get();
  }

 private:
  std::unique_ptr<BraveContentBrowserClient> browser_content_client_;
};

IN_PROC_BROWSER_TEST_F(BraveContentBrowserClientTest, CanLoadChromeURL) {
  std::vector<std::string> pages{
      kWelcomeHost,
  };

  std::vector<std::string> schemes{
      "brave://",
      "chrome://",
  };

  for (const std::string& page : pages) {
    for (const std::string& scheme : schemes) {
      content::WebContents* contents =
          browser()->tab_strip_model()->GetActiveWebContents();
      ASSERT_TRUE(
          ui_test_utils::NavigateToURL(browser(), GURL(scheme + page + "/")));
      ASSERT_TRUE(WaitForLoadStop(contents));

      EXPECT_STREQ(base::UTF16ToUTF8(
                       LocationBarModel::From(browser())->GetFormattedFullURL())
                       .c_str(),
                   ("brave://" + page).c_str());
      EXPECT_STREQ(contents->GetController()
                       .GetLastCommittedEntry()
                       ->GetVirtualURL()
                       .spec()
                       .c_str(),
                   ("chrome://" + page + "/").c_str());
      EXPECT_STREQ(contents->GetController()
                       .GetLastCommittedEntry()
                       ->GetURL()
                       .spec()
                       .c_str(),
                   ("chrome://" + page + "/").c_str());
    }
  }
}

IN_PROC_BROWSER_TEST_F(BraveContentBrowserClientTest, CanLoadAboutHost) {
  std::vector<std::string> schemes{
      "chrome://",
      "brave://",
  };

  for (const std::string& scheme : schemes) {
    content::WebContents* contents =
        browser()->tab_strip_model()->GetActiveWebContents();
    ASSERT_TRUE(
        ui_test_utils::NavigateToURL(browser(), GURL(scheme + "about/")));
    ASSERT_TRUE(WaitForLoadStop(contents));

    EXPECT_STREQ(base::UTF16ToUTF8(
                     LocationBarModel::From(browser())->GetFormattedFullURL())
                     .c_str(),
                 "brave://about");
    EXPECT_STREQ(contents->GetController()
                     .GetLastCommittedEntry()
                     ->GetVirtualURL()
                     .spec()
                     .c_str(),
                 "chrome://about/");
    EXPECT_STREQ(contents->GetController()
                     .GetLastCommittedEntry()
                     ->GetURL()
                     .spec()
                     .c_str(),
                 "chrome://chrome-urls/");
  }
}
IN_PROC_BROWSER_TEST_F(BraveContentBrowserClientTest, RewriteChromeSync) {
  std::vector<std::string> schemes{
      "brave://",
      "chrome://",
  };

  for (const std::string& scheme : schemes) {
    content::WebContents* contents =
        browser()->tab_strip_model()->GetActiveWebContents();
    NavigateToURLAndWaitForRewrites(contents,
                                    GURL(scheme + chrome::kBraveUISyncHost),
                                    GURL("chrome://sync"));

    EXPECT_STREQ(base::UTF16ToUTF8(
                     LocationBarModel::From(browser())->GetFormattedFullURL())
                     .c_str(),
                 "brave://sync");
    EXPECT_EQ(
        contents->GetController().GetLastCommittedEntry()->GetVirtualURL(),
        GURL("chrome://sync"));
    EXPECT_EQ(contents->GetController().GetLastCommittedEntry()->GetURL(),
              GURL("chrome://settings/braveSync"));
  }
}

IN_PROC_BROWSER_TEST_F(BraveContentBrowserClientTest, RewriteAdblock) {
  std::vector<std::string> schemes{
      "brave://",
      "chrome://",
  };

  for (const std::string& scheme : schemes) {
    content::WebContents* contents =
        browser()->tab_strip_model()->GetActiveWebContents();
    NavigateToURLAndWaitForRewrites(contents, GURL(scheme + "adblock"),
                                    GURL("chrome://settings/shields/filters"));
    EXPECT_STREQ(base::UTF16ToUTF8(
                     LocationBarModel::From(browser())->GetFormattedFullURL())
                     .c_str(),
                 "brave://settings/shields/filters");
    EXPECT_EQ(LocationBarModel::From(browser())->GetURL(),
              GURL("chrome://settings/shields/filters"));
    EXPECT_EQ(
        contents->GetController().GetLastCommittedEntry()->GetVirtualURL(),
        GURL("chrome://settings/shields/filters"));
  }
}

class BraveContentBrowserClientReferrerTest
    : public BraveContentBrowserClientTest {
 public:
  HostContentSettingsMap* content_settings() {
    return HostContentSettingsMapFactory::GetForProfile(browser()->GetProfile());
  }
};

IN_PROC_BROWSER_TEST_F(BraveContentBrowserClientReferrerTest,
                       DefaultBehaviour) {
  const GURL kRequestUrl("http://request.com/path?query");
  const GURL kDocumentUrl("http://document.com/path?query");
  const GURL kSameSiteRequestUrl("http://sub.document.com/sub/path");
  const GURL kSameOriginRequestUrl("http://document.com/different/path");

  blink::mojom::ReferrerPtr kReferrer = blink::mojom::Referrer::New(
      kDocumentUrl, network::mojom::ReferrerPolicy::kDefault);

  // Cross-origin navigations get an origin.
  blink::mojom::ReferrerPtr referrer = kReferrer.Clone();
  client()->MaybeHideReferrer(browser()->GetProfile(), kRequestUrl, kDocumentUrl,
                              &referrer);

  // Creating an Origin off a GURL should generally be avoided, but it's ok in
  // this particular case where we're just testing and using the http protocol.
  GURL document_url_origin = url::Origin::Create(kDocumentUrl).GetURL();
  EXPECT_EQ(referrer->url, document_url_origin);

  // Same-origin navigations get full referrers.
  referrer = kReferrer.Clone();
  client()->MaybeHideReferrer(browser()->GetProfile(), kSameOriginRequestUrl,
                              kDocumentUrl, &referrer);
  EXPECT_EQ(referrer->url, kDocumentUrl);

  // Same-site navigations get truncated referrers.
  referrer = kReferrer.Clone();
  client()->MaybeHideReferrer(browser()->GetProfile(), kSameSiteRequestUrl,
                              kDocumentUrl, &referrer);
  EXPECT_EQ(referrer->url, document_url_origin);

  // Cross-origin iframe navigations get origins.
  referrer = kReferrer.Clone();
  client()->MaybeHideReferrer(browser()->GetProfile(), kRequestUrl, kDocumentUrl,
                              &referrer);
  EXPECT_EQ(referrer->url, document_url_origin);

  // Same-origin iframe navigations get full referrers.
  referrer = kReferrer.Clone();
  client()->MaybeHideReferrer(browser()->GetProfile(), kSameOriginRequestUrl,
                              kDocumentUrl, &referrer);
  EXPECT_EQ(referrer->url, kDocumentUrl);

  // Special rule for extensions.
  const GURL kExtensionUrl("chrome-extension://abc/path?query");
  referrer = kReferrer.Clone();
  referrer->url = kExtensionUrl;
  client()->MaybeHideReferrer(browser()->GetProfile(), kRequestUrl, kExtensionUrl,
                              &referrer);
  EXPECT_EQ(referrer->url, kExtensionUrl);

  // Special rule for Onion services.
  const GURL kOnionUrl("http://lwkjglkejslkgjel.onion/index.html");
  referrer = kReferrer.Clone();
  referrer->url = kOnionUrl;
  client()->MaybeHideReferrer(browser()->GetProfile(), kRequestUrl, kOnionUrl,
                              &referrer);
  EXPECT_EQ(referrer->url, GURL());  // .onion -> normal
  referrer = kReferrer.Clone();
  client()->MaybeHideReferrer(browser()->GetProfile(), kOnionUrl, kDocumentUrl,
                              &referrer);
  EXPECT_EQ(referrer->url, document_url_origin);  // normal -> .onion

  // Allow referrers for certain URL.
  content_settings()->SetContentSettingCustomScope(
      ContentSettingsPattern::FromString(document_url_origin.spec() + "*"),
      ContentSettingsPattern::Wildcard(), ContentSettingsType::BRAVE_REFERRERS,
      CONTENT_SETTING_ALLOW);
  referrer = kReferrer.Clone();
  client()->MaybeHideReferrer(browser()->GetProfile(), kRequestUrl, kDocumentUrl,
                              &referrer);
  EXPECT_EQ(referrer->url, kDocumentUrl);
}

// Confirm only expected extension has been installed.
IN_PROC_BROWSER_TEST_F(BraveContentBrowserClientTest, CheckExpectedExtensions) {
  // Info: This checkup will not cover on-demand component extension
  // installation.
  std::set<std::string> expected_extensions = {
      brave_extension_id,
      extension_misc::kPdfExtensionId,
  };

  extensions::ExtensionRegistry* registry =
      extensions::ExtensionRegistry::Get(browser()->GetProfile());
  std::set<std::string> installed_extensions =
      registry->GenerateInstalledExtensionsSet().GetIDs();

  EXPECT_EQ(expected_extensions, installed_extensions);
}

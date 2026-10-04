/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/browser/net/brave_common_static_redirect_network_delegate_helper.h"

#include <memory>
#include <string>

#include "base/command_line.h"
#include "brave/browser/net/url_context.h"
#include "brave/components/constants/network_constants.h"
#include "brave/components/constants/url_constants.h"
#include "net/base/net_errors.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "url/gurl.h"
#include "url/url_constants.h"

using brave::ResponseCallback;

TEST(BraveCommonStaticRedirectNetworkDelegateHelperTest,
     RedirectChromecastDownload) {
  const GURL url(
      "http://redirector.gvt1.com/edgedl/chromewebstore/"
      "random_hash/random_version_pkedcjkdefgpdelpbcmbmeomcjbeemfm.crx");
  auto request_info = std::make_shared<brave::BraveRequestInfo>(url);

  int rc = OnBeforeURLRequest_CommonStaticRedirectWork(ResponseCallback(),
                                                       request_info);
  const GURL redirect = GURL(request_info->new_url_spec);
  EXPECT_EQ(redirect.host(), kBraveRedirectorProxy);
  EXPECT_TRUE(redirect.SchemeIs(url::kHttpsScheme));
  EXPECT_EQ(redirect.path(), url.path());
  EXPECT_EQ(rc, net::OK);
}

TEST(BraveCommonStaticRedirectNetworkDelegateHelperTest,
     RedirectGoogleClients4) {
  const GURL url("https://clients4.google.com/chrome-sync/dev");
  auto request_info = std::make_shared<brave::BraveRequestInfo>(url);

  int rc = OnBeforeURLRequest_CommonStaticRedirectWork(ResponseCallback(),
                                                       request_info);
  const GURL redirect = GURL(request_info->new_url_spec);
  EXPECT_EQ(redirect.host(), kBraveClients4Proxy);
  EXPECT_TRUE(redirect.SchemeIs(url::kHttpsScheme));
  EXPECT_EQ(redirect.path(), url.path());
  EXPECT_EQ(rc, net::OK);
}

TEST(BraveCommonStaticRedirectNetworkDelegateHelperTest,
     RedirectBugsChromium) {
  // FlyWeb: Chromium's bug report links go to our "report a problem" page,
  // whatever the query.
  for (const char* spec :
       {"https://bugs.chromium.org/p/chromium/issues/"
        "entry?template=Crash%20Report&comment=IMPORTANT%20Chrome&labels="
        "Restrict-View-EditIssue%2CStability-Crash%2CUser-Submitted",
        "https://bugs.chromium.org/p/chromium/issues/entry?template=A"}) {
    auto request_info = std::make_shared<brave::BraveRequestInfo>(GURL(spec));
    int rc = OnBeforeURLRequest_CommonStaticRedirectWork(ResponseCallback(),
                                                         request_info);
    EXPECT_EQ(request_info->new_url_spec, kFlyWebReportProblemURL);
    EXPECT_EQ(rc, net::OK);
  }
}

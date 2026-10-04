/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/browser/net/brave_common_static_redirect_network_delegate_helper.h"

#include <memory>
#include <string>

#include "brave/components/constants/network_constants.h"
#include "brave/components/constants/url_constants.h"
#include "extensions/common/url_pattern.h"
#include "net/base/net_errors.h"
#include "url/gurl.h"

namespace brave {

int OnBeforeURLRequest_CommonStaticRedirectWork(
    const ResponseCallback& next_callback,
    std::shared_ptr<BraveRequestInfo> ctx) {
  GURL new_url;
  int rc = OnBeforeURLRequest_CommonStaticRedirectWorkForGURL(ctx->request_url,
                                                              &new_url);
  if (!new_url.is_empty()) {
    ctx->new_url_spec = new_url.spec();
  }
  return rc;
}

int OnBeforeURLRequest_CommonStaticRedirectWorkForGURL(
    const GURL& request_url,
    GURL* new_url) {
  DCHECK(new_url);

  GURL::Replacements replacements;
  static URLPattern chromecast_pattern(
      URLPattern::SCHEME_HTTP | URLPattern::SCHEME_HTTPS, kChromeCastPrefix);
  static URLPattern clients4_pattern(
      URLPattern::SCHEME_HTTP | URLPattern::SCHEME_HTTPS, kClients4Prefix);
  static URLPattern bugsChromium_pattern(
      URLPattern::SCHEME_HTTP | URLPattern::SCHEME_HTTPS,
      "*://bugs.chromium.org/p/chromium/issues/entry?*");

  if (chromecast_pattern.MatchesURL(request_url)) {
    replacements.SetSchemeStr("https");
    replacements.SetHostStr(kBraveRedirectorProxy);
    *new_url = request_url.ReplaceComponents(replacements);
    return net::OK;
  }

  // FlyWeb: clients4.google.com (Chrome sync, which FlyWeb disables) is
  // blocked instead of going through Brave's clients4.brave.com.
  // (new_url too: system requests ignore the error; .invalid never resolves.)
  if (clients4_pattern.MatchesHost(request_url)) {
    *new_url = GURL(kFlyWebBlockedURL);
    return net::ERR_BLOCKED_BY_CLIENT;
  }

  // FlyWeb: Chromium's "report a bug" links (crash pages) go to our page on
  // how to report a problem, not to Brave's GitHub.
  if (bugsChromium_pattern.MatchesURL(request_url)) {
    *new_url = GURL(kFlyWebReportProblemURL);
    return net::OK;
  }

  return net::OK;
}


}  // namespace brave

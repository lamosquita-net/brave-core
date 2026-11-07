/* Copyright (c) 2026 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "content/public/browser/browser_url_handler.h"
#include "content/public/common/url_constants.h"
#include "url/gurl.h"

namespace {

GURL* RewriteExpectedURLBraveToChrome(GURL* url) {
  if (url && url->SchemeIs(content::kBraveUIScheme)) {
    GURL::Replacements replacements;
    replacements.SetSchemeStr(content::kChromeUIScheme);
    *url = url->ReplaceComponents(replacements);
  }
  return url;
}

}  // namespace

// flyweb:// (content::kBraveUIScheme) is a display-only scheme that maps to
// chrome:// internally. When tests navigate to flyweb:// URLs using
// NavigateToURLBlockUntilNavigationsComplete or similar, the actual committed
// URL will be chrome://. We need to fix up the expected URL before comparing so
// that navigation observers correctly match flyweb:// URLs with their chrome://
// equivalents.
#define RewriteURLIfNecessary(URL, BROWSER_CONTEXT) \
  RewriteURLIfNecessary(RewriteExpectedURLBraveToChrome(URL), BROWSER_CONTEXT)

#include "src/content/public/test/test_navigation_observer.cc"

#undef RewriteURLIfNecessary

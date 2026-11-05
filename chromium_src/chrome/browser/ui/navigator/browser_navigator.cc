/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "chrome/browser/ui/navigator/browser_navigator_params.h"
#include "content/public/common/url_constants.h"
#include "url/gurl.h"

namespace {

void UpdateBraveScheme(NavigateParams* params) {
  if (params->url.SchemeIs(content::kBraveUIScheme)) {
    GURL::Replacements replacements;
    replacements.SetSchemeStr(content::kChromeUIScheme);
    params->url = params->url.ReplaceComponents(replacements);
  }
}

}  // namespace

// The incognito URL check moved to incognito_allowed_url.cc and the HTTPS
// upgrade of typed http:// URLs to browser_navigator_params_utils.cc in 156.
#define BRAVE_ADJUST_NAVIGATE_PARAMS_FOR_URL UpdateBraveScheme(params);
#include "src/chrome/browser/ui/navigator/browser_navigator.cc"
#undef BRAVE_ADJUST_NAVIGATE_PARAMS_FOR_URL

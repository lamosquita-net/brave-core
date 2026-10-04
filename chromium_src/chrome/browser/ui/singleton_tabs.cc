/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/components/constants/url_constants.h"

#define ShowSingletonTab ShowSingletonTab_ChromiumImpl
#include "src/chrome/browser/ui/singleton_tabs.cc"
#undef ShowSingletonTab

void ShowSingletonTab(Browser* browser, const GURL& url) {
  // FlyWeb: Google help pages (support.google.com...) go to our help page.
  GURL new_url = url.DomainIs("google.com") ? GURL(kFlyWebHelpURL) : url;

  ShowSingletonTab_ChromiumImpl(browser, new_url);
}

void ShowSingletonTab(Profile* profile, const GURL& url) {
  GURL new_url = url.DomainIs("google.com") ? GURL(kFlyWebHelpURL) : url;

  ShowSingletonTab_ChromiumImpl(profile, new_url);
}

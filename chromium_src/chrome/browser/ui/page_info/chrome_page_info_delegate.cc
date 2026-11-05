/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "chrome/browser/ui/page_info/chrome_page_info_delegate.h"

#include "chrome/common/url_constants.h"

#include "src/chrome/browser/ui/page_info/chrome_page_info_delegate.cc"

// FlyWeb: the only permissions hidden here were the Shields and geolocation
// ones in Tor windows, and Tor is gone, so every permission is shown.
bool ChromePageInfoDelegate::BraveShouldShowPermission(
    ContentSettingsType type) {
  return true;
}

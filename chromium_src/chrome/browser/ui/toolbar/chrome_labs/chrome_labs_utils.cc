/* Copyright (c) 2024 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

// Chrome Labs used to be gated by features::kChromeLabs; on 156 it is
// IsChromeLabsEnabled(). Keep it off.
#define IsChromeLabsEnabled IsChromeLabsEnabled_ChromiumImpl
#include "src/chrome/browser/ui/toolbar/chrome_labs/chrome_labs_utils.cc"
#undef IsChromeLabsEnabled

bool IsChromeLabsEnabled() {
  return false;
}

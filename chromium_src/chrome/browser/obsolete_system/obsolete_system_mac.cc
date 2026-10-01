/* Copyright (c) 2026 lamosquita. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

// FlyWeb targets macOS 10.13/10.14 on purpose (it is the reason it exists), so
// never show the "you'll need macOS 10.15 or later to get future updates"
// infobar that Chromium 116 shows there. The rest of the file (strings, URL)
// is left as is.
#define IsObsoleteNowOrSoon IsObsoleteNowOrSoon_ChromiumImpl
#include "src/chrome/browser/obsolete_system/obsolete_system_mac.cc"
#undef IsObsoleteNowOrSoon

namespace ObsoleteSystem {

bool IsObsoleteNowOrSoon() {
  return false;
}

}  // namespace ObsoleteSystem

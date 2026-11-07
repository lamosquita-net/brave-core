/* Copyright (c) 2026 lamosquita. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

// FlyWeb targets macOS 10.14 on purpose (it is the reason it exists), so never
// warn that this macOS is obsolete: no infobar (Chromium 116 meant 10.15, 128
// macOS 11, 156 macOS 12) and no text for the About page either, whose
// hidden element would otherwise carry IDS_MACOS_OBSOLETE with a wrong macOS
// version (COORDINACIÓN-27, 04-11). The link URL is left as is.
#define IsObsoleteNowOrSoon IsObsoleteNowOrSoon_ChromiumImpl
#define LocalizedObsoleteString LocalizedObsoleteString_ChromiumImpl
#define IsEndOfTheLine IsEndOfTheLine_ChromiumImpl
#include "src/chrome/browser/obsolete_system/obsolete_system_mac.cc"
#undef IsEndOfTheLine
#undef LocalizedObsoleteString
#undef IsObsoleteNowOrSoon

namespace ObsoleteSystem {

bool IsObsoleteNowOrSoon() {
  return false;
}

std::u16string LocalizedObsoleteString() {
  return std::u16string();
}

bool IsEndOfTheLine() {
  return false;
}

}  // namespace ObsoleteSystem

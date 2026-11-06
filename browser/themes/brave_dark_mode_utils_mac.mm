/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/browser/themes/brave_dark_mode_utils.h"

#import <Cocoa/Cocoa.h>

namespace dark_mode {

void SetSystemDarkMode(BraveDarkModeType type) {
  // macOS supports a per-application appearance, which NativeThemeMac follows.
  if (type == BraveDarkModeType::BRAVE_DARK_MODE_TYPE_DEFAULT) {
    [NSApp setAppearance:nil];
    return;
  }

  NSAppearanceName new_appearance_name =
      type == BraveDarkModeType::BRAVE_DARK_MODE_TYPE_DARK
          ? NSAppearanceNameDarkAqua
          : NSAppearanceNameAqua;
  [NSApp setAppearance:[NSAppearance appearanceNamed:new_appearance_name]];
}

}  // namespace dark_mode

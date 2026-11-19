/* Copyright (c) 2022 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_BROWSER_UI_COLOR_COLOR_PALETTE_H_
#define BRAVE_BROWSER_UI_COLOR_COLOR_PALETTE_H_

#include "third_party/skia/include/core/SkColor.h"

// Although most of colors are only used at brave_color_mixer.cc,
// test also needs some colors. So, defined all here.
constexpr SkColor kPrivateColorForTest = SkColorSetRGB(0xFF, 0x00, 0x00);
constexpr SkColor kLightColorForTest = SkColorSetRGB(0xFF, 0xFF, 0xFF);
constexpr SkColor kDarkColorForTest = SkColorSetRGB(0x00, 0x00, 0x00);

// FlyWeb 3.2.0 interface: colors from the design in
// softmac FlyWeb/branding/interfaz-3.2.0/README.md.
//
// Frame: the top bar that holds the inactive tabs.
constexpr SkColor kLightFrame = SkColorSetRGB(0xE3, 0xE3, 0xE2);
constexpr SkColor kLightOmniboxText = SkColorSetRGB(0x42, 0x42, 0x42);

constexpr SkColor kDarkFrame = SkColorSetRGB(0x4D, 0x4D, 0x4C);
constexpr SkColor kDarkOmniboxText = SkColorSetRGB(0xff, 0xff, 0xff);

// Active tab and toolbar: one piece, the exact same color.
constexpr SkColor kFlyWebLightToolbar = SkColorSetRGB(0xF4, 0xF5, 0xF6);
constexpr SkColor kFlyWebDarkToolbar = SkColorSetRGB(0x65, 0x66, 0x66);

// Toolbar icons and tab text (Titín, 19-11).
constexpr SkColor kFlyWebLightToolbarIcon = SkColorSetRGB(0x65, 0x66, 0x66);
constexpr SkColor kFlyWebDarkToolbarIcon = SkColorSetRGB(0xED, 0xEE, 0xF1);
constexpr SkColor kFlyWebLightTabText = SK_ColorBLACK;
constexpr SkColor kFlyWebDarkTabText = SK_ColorWHITE;
// Inactive tabs: the active tab text at this opacity, composited over the tab
// strip so the result is opaque (Chromium's tab code expects opaque colors).
constexpr SkAlpha kFlyWebInactiveTabTextAlpha = 0x80;  // 50%

// URL field background, in light and dark mode.
constexpr SkColor kFlyWebLocationBarBackground = SK_ColorWHITE;

// Dark mode URL field and suggestion popup.
// true: both white with dark text, as in light mode. Chromium paints the
// focused URL field with the popup background (kColorOmniboxResultsBackground)
// and both share the text color ids, so a white field needs a white popup.
// false: Brave's dark omnibox (kBraveDarkLocationBarBackground).
constexpr bool kFlyWebLightOmniboxInDarkMode = true;
constexpr SkColor kBraveDarkLocationBarBackground =
    SkColorSetRGB(0x0D, 0x0F, 0x14);

// Text selection in a white URL field while the window is in dark mode
// (Chromium's selection color for dark mode is meant for a dark field).
constexpr SkColor kFlyWebOmniboxSelectionOnLight =
    SkColorSetRGB(0xB3, 0xD7, 0xFF);

constexpr SkColor kPrivateFrame = SkColorSetRGB(0x13, 0x05, 0x2A);

constexpr SkColor kBraveNewTabBackgroundDark = SkColorSetRGB(0x33, 0x36, 0x39);
constexpr SkColor kBraveNewTabBackgroundLight = SkColorSetRGB(0x6B, 0x70, 0x84);

constexpr SkAlpha kBraveDisabledControlAlpha = 0x66;  // 40%

#endif  // BRAVE_BROWSER_UI_COLOR_COLOR_PALETTE_H_

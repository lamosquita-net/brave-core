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
// Inactive tab under the pointer (Titín, 20-11).
constexpr SkColor kFlyWebLightInactiveTabHover = SkColorSetRGB(0xEB, 0xEB, 0xEB);
constexpr SkColor kFlyWebDarkInactiveTabHover = SkColorSetRGB(0x58, 0x59, 0x59);

// URL field background, in light and dark mode.
constexpr SkColor kFlyWebLocationBarBackground = SK_ColorWHITE;

// Dark mode URL field and suggestion popup.
// true: both white with dark text, as in light mode. Chromium paints the
// focused URL field with the popup background (kColorOmniboxResultsBackground)
// and both share the text color ids, so a white field needs a white popup.
// false: Brave's dark omnibox (kBraveDarkLocationBarBackground).
// Titín, 20-11 (maqueta-oscuro-desplegable-hd.webp): replaced by a dark URL
// field and popup in dark mode, «en la gama del entorno» (the kFlyWebDark*
// omnibox colors below). Kept false so the white-on-dark path is not used.
constexpr bool kFlyWebLightOmniboxInDarkMode = false;
constexpr SkColor kBraveDarkLocationBarBackground =
    SkColorSetRGB(0x0D, 0x0F, 0x14);

// FlyWeb 3.2.0, dark mode omnibox (Titín, 20-11), measured on
// maqueta-oscuro-desplegable-hd.webp. Regular windows only: private windows
// use their own theme. White text on the field and the popup.
constexpr SkColor kFlyWebDarkLocationBarBackground =
    SkColorSetRGB(0x4D, 0x4D, 0x4D);
constexpr SkColor kFlyWebDarkOmniboxResultsBackground =
    SkColorSetRGB(0x90, 0x92, 0x94);
constexpr SkColor kFlyWebDarkOmniboxResultsSelected =
    SkColorSetRGB(0x54, 0x26, 0x71);
// The light mark at the left of the selected row (about #AFA8D3 on the
// mockup, Titín's #AFA6D2).
constexpr SkColor kFlyWebDarkOmniboxFocusIndicator =
    SkColorSetRGB(0xAF, 0xA6, 0xD2);
constexpr SkColor kFlyWebDarkOmniboxText = SK_ColorWHITE;

// Text selection in a white URL field while the window is in dark mode
// (Chromium's selection color for dark mode is meant for a dark field).
constexpr SkColor kFlyWebOmniboxSelectionOnLight =
    SkColorSetRGB(0xB3, 0xD7, 0xFF);

constexpr SkColor kPrivateFrame = SkColorSetRGB(0x13, 0x05, 0x2A);

// FlyWeb 3.2.0: private window active tab and toolbar (Titín, 19-11), one
// piece of the same color as in regular windows. Private windows are always in
// dark mode (ThemeService forces it for incognito profiles); Titín's light
// value, #AFA6D2, waits for the decision on whether they follow the system.
constexpr SkColor kFlyWebPrivateDarkToolbar = SkColorSetRGB(0x57, 0x1E, 0x73);
// Private window frame (Titín, 19-11), so the private window is not the same
// as a regular window in dark mode. Only the frame: the URL field and the new
// tab page keep kPrivateFrame.
constexpr SkColor kFlyWebPrivateFrame = SkColorSetRGB(0x27, 0x27, 0x26);

constexpr SkColor kBraveNewTabBackgroundDark = SkColorSetRGB(0x33, 0x36, 0x39);
constexpr SkColor kBraveNewTabBackgroundLight = SkColorSetRGB(0x6B, 0x70, 0x84);

constexpr SkAlpha kBraveDisabledControlAlpha = 0x66;  // 40%

#endif  // BRAVE_BROWSER_UI_COLOR_COLOR_PALETTE_H_

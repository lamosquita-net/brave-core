/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_BROWSER_UI_BRAVE_LAYOUT_CONSTANTS_H_
#define BRAVE_BROWSER_UI_BRAVE_LAYOUT_CONSTANTS_H_

#include <optional>

#include "chrome/browser/ui/layout_constants.h"

std::optional<gfx::Insets> GetBraveLayoutInsets(LayoutInset inset);
std::optional<int> GetBraveLayoutConstant(LayoutConstant constant);

// FlyWeb 3.2.0 interface (Titín, 20-11): the URL field is this many dip
// shorter, taken from the top, so it sits a bit further from the tabs. The
// toolbar keeps its height and the field keeps its bottom edge. Provisional,
// adjusted with Titín on screenshots.
inline constexpr int kFlyWebLocationBarTopTrim = 3;

// FlyWeb 3.2.0 interface (Titín, 20-11, maqueta-oscuro-desplegable-hd.webp and
// maqueta-claro-url-sutil.png): the URL field and the suggestions popup have a
// very subtle corner radius, not a pill (about 1-2 px on the mockups, where
// 1 px is about 1 dip), and form one piece of the same width. In dip.
inline constexpr int kFlyWebLocationBarCornerRadius = 2;

#endif  // BRAVE_BROWSER_UI_BRAVE_LAYOUT_CONSTANTS_H_

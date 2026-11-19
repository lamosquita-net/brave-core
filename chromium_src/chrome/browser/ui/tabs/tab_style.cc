/* Copyright (c) 2026 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

// FlyWeb 3.2.0 interface (softmac FlyWeb/branding/interfaz-3.2.0): a smaller
// top corner radius than Chromium's 10 dip, taken from Titín's mockup in
// proportion to the tab height (about 1-1.5 px of corner on a ~16 px tab that
// is 34 dip high). Adjusted in tests with Titín. Every user of
// TabStyle::GetTopCornerRadius() follows: the tab shape, the hover highlight
// and the focus ring. The tab size does not depend on it.
#define BRAVE_TAB_STYLE_TOP_CORNER_RADIUS 3

#include "src/chrome/browser/ui/tabs/tab_style.cc"

#undef BRAVE_TAB_STYLE_TOP_CORNER_RADIUS

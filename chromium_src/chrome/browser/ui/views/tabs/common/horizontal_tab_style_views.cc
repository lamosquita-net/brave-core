/* Copyright (c) 2026 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include <algorithm>

namespace {

// FlyWeb 3.2.0 interface (softmac FlyWeb/branding/interfaz-3.2.0): the tab
// joins the toolbar with a straight edge, without Chromium's outward curves at
// the base. Only the drawn radius changes; TabStyle::GetBottomCornerRadius()
// still sets the tab width and the overlap between tabs, so the tab size stays
// the same. In dip; 0 = straight join.
constexpr float kFlyWebTabBottomCornerRadius = 0.0f;

}  // namespace

#define BRAVE_HORIZONTAL_TAB_STYLE_VIEWS_GET_PATH_BOTTOM_CORNER_RADIUS \
  extension_corner_radius = std::clamp(extension_corner_radius, 0.0f,  \
                                       kFlyWebTabBottomCornerRadius * scale);

#include "src/chrome/browser/ui/views/tabs/common/horizontal_tab_style_views.cc"

#undef BRAVE_HORIZONTAL_TAB_STYLE_VIEWS_GET_PATH_BOTTOM_CORNER_RADIUS

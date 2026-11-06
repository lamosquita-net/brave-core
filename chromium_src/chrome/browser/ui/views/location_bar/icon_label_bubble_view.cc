/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

// 156: LayoutConstant is an enum class and SkPath is built with RRect().
#define BRAVE_ICON_LABEL_BUBBLE_VIEW_GET_HIGHLIGHT_PATH                 \
  const int layout_radius =                                             \
      GetLayoutConstant(LayoutConstant::kLocationBarChildCornerRadius); \
  return SkPath::RRect(gfx::RectToSkRect(highlight_bounds), layout_radius, \
                       layout_radius);

#include "src/chrome/browser/ui/views/location_bar/icon_label_bubble_view.cc"
#undef BRAVE_ICON_LABEL_BUBBLE_VIEW_GET_HIGHLIGHT_PATH

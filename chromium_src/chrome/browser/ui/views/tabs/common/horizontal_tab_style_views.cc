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

// FlyWeb 3.2.0 interface (Titín, 20-11): the hover background of an inactive
// tab (and the fill of a selected one) is a tab silhouette, like the active
// tab but behind it, instead of Chromium's detached pill. Chromium's
// kHighlight path ends kTabStripPadding above the base, since the tab view is
// kTabStripHeight high and the pill is kTabHeight minus the padding and the
// toolbar overlap, and rounds its bottom corners. Here it reaches the same
// base as the active tab (the view bottom minus the toolbar overlap) and its
// bottom corners are square. The hit test path is not changed. In dip.
constexpr float kFlyWebHighlightBottomCornerRadius = 0.0f;

// FlyWeb 3.2.0 interface (Titín, 20-11, bt-320e: «la pestaña en roll over
// debía ser más baja, como en mi maqueta»): on maqueta-claro-hd.webp the hover
// of the first inactive tab starts about 5 px below the top of the active tab
// (y 50.5 against 45.5) and reaches the same base. The active tab is 38.5 px
// high there and 34 dip in the browser, so that is about 4.4 dip. Only for
// inactive tabs (hover and selected fills); the active tab keeps its full
// height (kHighlight is also its child clip path). In dip.
constexpr float kFlyWebHoverTabTopInset = 4.0f;

}  // namespace

#define BRAVE_HORIZONTAL_TAB_STYLE_VIEWS_GET_PATH_HIGHLIGHT                 \
  if (path_type == TabStyle::PathType::kHighlight) {                        \
    tab_height =                                                            \
        aligned_bounds.height() -                                           \
        (GetLayoutConstant(LayoutConstant::kTabStripPadding) +              \
         GetLayoutConstant(LayoutConstant::kTabstripToolbarOverlap)) *      \
            scale;                                                          \
    bottom_left_corner_radius = kFlyWebHighlightBottomCornerRadius * scale; \
    bottom_right_corner_radius = kFlyWebHighlightBottomCornerRadius * scale; \
  }

#define BRAVE_HORIZONTAL_TAB_STYLE_VIEWS_GET_PATH_HIGHLIGHT_TOP       \
  if (path_type == TabStyle::PathType::kHighlight &&                  \
      !delegate_->IsActive()) {                                       \
    top += kFlyWebHoverTabTopInset * scale;                           \
    tab_height -= kFlyWebHoverTabTopInset * scale;                    \
  }

#define BRAVE_HORIZONTAL_TAB_STYLE_VIEWS_GET_PATH_BOTTOM_CORNER_RADIUS \
  extension_corner_radius = std::clamp(extension_corner_radius, 0.0f,  \
                                       kFlyWebTabBottomCornerRadius * scale);

#include "src/chrome/browser/ui/views/tabs/common/horizontal_tab_style_views.cc"

#undef BRAVE_HORIZONTAL_TAB_STYLE_VIEWS_GET_PATH_BOTTOM_CORNER_RADIUS
#undef BRAVE_HORIZONTAL_TAB_STYLE_VIEWS_GET_PATH_HIGHLIGHT
#undef BRAVE_HORIZONTAL_TAB_STYLE_VIEWS_GET_PATH_HIGHLIGHT_TOP

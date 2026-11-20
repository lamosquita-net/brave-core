/* Copyright (c) 2022 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "chrome/browser/ui/views/omnibox/rounded_omnibox_results_frame.h"

#include "brave/browser/ui/brave_layout_constants.h"
#include "chrome/browser/ui/layout_constants.h"
#include "ui/views/layout/layout_provider.h"

// FlyWeb 3.2.0 interface (Titín, 20-11, maqueta-oscuro-desplegable-hd.webp):
// the suggestions popup and the URL field form one piece. In every window
// (also private ones, Titín):
// - Same width as the field and no frame above it: no alignment insets
//   (Chromium: 6 dip on each side and 5 dip above and below).
// - Results right below the field, with no gap: the cutout is as high as the
//   field, which is kFlyWebLocationBarTopTrim shorter than kLocationBarHeight
//   since 0009. Before, the 3 dip difference left a transparent band under the
//   field through which the toolbar showed.
// - No outer shadow: elevation 0 (the header was included above, so the
//   constant's declaration is not renamed).
// - The field's subtle corner radius (kOmniboxExpandedRadius was 4 in Brave).
#define BRAVE_ROUNDED_OMNIBOX_RESULTS_FRAME_GET_LOCATION_BAR_ALIGNMENT_INSETS \
  return gfx::Insets();
#define BRAVE_ROUNDED_OMNIBOX_RESULTS_FRAME_GET_NON_RESULT_SECTION_HEIGHT \
  return include_cutout                                                    \
             ? GetLayoutConstant(LayoutConstant::kLocationBarHeight) -     \
                   kFlyWebLocationBarTopTrim                               \
             : 0;
#define kDefaultElevation 0

// Set our radius value directly as kOmniboxExpandedRadius is mapped to
// more general token ShapeSysTokens::kMedium. In 156 `corner_radius` is a
// const local, so fold our value into its initializer.
#define kOmniboxExpandedRadius \
  kOmniboxExpandedRadius) * 0 + (kFlyWebLocationBarCornerRadius

#include "src/chrome/browser/ui/views/omnibox/rounded_omnibox_results_frame.cc"

#undef kOmniboxExpandedRadius
#undef kDefaultElevation
#undef BRAVE_ROUNDED_OMNIBOX_RESULTS_FRAME_GET_NON_RESULT_SECTION_HEIGHT
#undef BRAVE_ROUNDED_OMNIBOX_RESULTS_FRAME_GET_LOCATION_BAR_ALIGNMENT_INSETS

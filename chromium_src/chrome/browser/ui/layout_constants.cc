/* Copyright (c) 2018 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <optional>

#include "brave/browser/ui/brave_layout_constants.h"

// Forward declaration
gfx::Insets GetLayoutInsets_ChromiumImpl(LayoutInset inset);

#define LayoutInset LayoutInset inset) {           \
    const std::optional<gfx::Insets> braveOption = \
        GetBraveLayoutInsets(inset);               \
    if (braveOption) {                             \
      return braveOption.value();                  \
    }                                              \
                                                   \
    return GetLayoutInsets_ChromiumImpl(inset);    \
  }                                                \
                                                   \
  gfx::Insets GetLayoutInsets_ChromiumImpl(LayoutInset

// LayoutConstant is an enum class in 156, so the upstream function is renamed
// instead and wrapped below. Upstream's internal GetLayoutConstant() calls go
// to the Chromium implementation.
#define GetLayoutConstant GetLayoutConstant_ChromiumImpl
#include "src/chrome/browser/ui/layout_constants.cc"
#undef GetLayoutConstant
#undef LayoutInset

int GetLayoutConstant(LayoutConstant constant) {
  const std::optional<int> brave_option = GetBraveLayoutConstant(constant);
  if (brave_option) {
    return brave_option.value();
  }
  return GetLayoutConstant_ChromiumImpl(constant);
}

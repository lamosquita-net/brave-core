/* Copyright (c) 2022 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "brave/browser/ui/views/omnibox/brave_omnibox_popup_view_views.h"

#include "chrome/browser/ui/views/location_bar/location_bar_view.h"
#include "ui/base/metadata/metadata_impl_macros.h"

BraveOmniboxPopupViewViews::~BraveOmniboxPopupViewViews() = default;

int BraveOmniboxPopupViewViews::GetLocationBarViewWidth() const {
  return location_bar_view()->width();
}

BEGIN_METADATA(BraveOmniboxPopupViewViews)
END_METADATA

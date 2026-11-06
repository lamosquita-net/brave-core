/* Copyright (c) 2022 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/browser/ui/views/tabs/brave_tab_strip_layout_helper.h"

#include "brave/browser/ui/tabs/brave_tab_layout_constants.h"
#include "brave/browser/ui/tabs/features.h"
#include "chrome/browser/ui/views/tabs/tab.h"

namespace tabs {

int GetTabCornerRadius(const Tab& tab) {
  if (!tabs::features::HorizontalTabsUpdateEnabled()) {
    return tab.data().pinned ? 8 : 4;
  }

  return brave_tabs::kTabBorderRadius;
}

}  // namespace tabs

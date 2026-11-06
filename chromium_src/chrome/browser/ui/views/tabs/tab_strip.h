/* Copyright (c) 2022 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_CHROMIUM_SRC_CHROME_BROWSER_UI_VIEWS_TABS_TAB_STRIP_H_
#define BRAVE_CHROMIUM_SRC_CHROME_BROWSER_UI_VIEWS_TABS_TAB_STRIP_H_

#include "brave/browser/ui/views/tabs/brave_tab_container.h"
#include "chrome/browser/ui/views/tabs/tab_container.h"
#include "chrome/browser/ui/views/tabs/tab_slot_controller.h"

class BraveTabHoverCardController;

#define UpdateHoverCard           \
  UpdateHoverCard_Unused();       \
  friend class BraveTabHoverTest; \
  friend class BraveTabStrip;     \
  void UpdateHoverCard

#define TabHoverCardController BraveTabHoverCardController
#include "src/chrome/browser/ui/views/tabs/tab_strip.h"  // IWYU pragma: export
#undef TabHoverCardController
#undef UpdateHoverCard

#endif  // BRAVE_CHROMIUM_SRC_CHROME_BROWSER_UI_VIEWS_TABS_TAB_STRIP_H_

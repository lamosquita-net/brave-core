/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "chrome/browser/ui/views/frame/horizontal_tab_strip_region_view.h"

// 156 creates the tab strip, its controller and the hover card controller here
// (CreateTabStrip()) instead of in browser_view.cc; the patch swaps in Brave's.
#include "brave/browser/ui/views/tabs/brave_browser_tab_strip_controller.h"
#include "brave/browser/ui/views/tabs/brave_tab_hover_card_controller.h"
#include "brave/browser/ui/views/tabs/brave_tab_strip.h"

// FlyWeb 3.2.0 interface: Titín's "+" for the new tab button only (Chromium's
// add_old.icon is a generic "+" used across the UI).
#include "brave/app/vector_icons/vector_icons.h"
#define BRAVE_NEW_TAB_BUTTON_OLD_ICON kFlywebNewTabIcon

#include "src/chrome/browser/ui/views/frame/horizontal_tab_strip_region_view.cc"

#undef BRAVE_NEW_TAB_BUTTON_OLD_ICON

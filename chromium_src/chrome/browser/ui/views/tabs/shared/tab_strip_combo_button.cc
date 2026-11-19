/* Copyright (c) 2026 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

// FlyWeb 3.2.0 interface: Titín's chevron for the tab search button of the
// horizontal tab strip only (Chromium's expand_more_old.icon is a generic
// chevron used across the UI).
#include "brave/app/vector_icons/vector_icons.h"

#define BRAVE_TAB_STRIP_COMBO_BUTTON_TAB_SEARCH_OLD_ICON kFlywebTabSearchIcon

#include "src/chrome/browser/ui/views/tabs/shared/tab_strip_combo_button.cc"

#undef BRAVE_TAB_STRIP_COMBO_BUTTON_TAB_SEARCH_OLD_ICON

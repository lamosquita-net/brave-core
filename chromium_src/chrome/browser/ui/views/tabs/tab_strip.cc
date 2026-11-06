/* Copyright (c) 2022 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "chrome/browser/ui/views/tabs/tab_strip.h"

#include <cmath>

#include "brave/browser/ui/views/tabs/brave_tab.h"
#include "brave/browser/ui/views/tabs/brave_tab_container.h"
#include "brave/browser/ui/views/tabs/brave_tab_hover_card_controller.h"
#include "chrome/browser/ui/ui_features.h"
#include "chrome/browser/ui/views/tabs/browser_tab_strip_controller.h"
#include "chrome/browser/ui/views/tabs/tab_container.h"
#include "chrome/browser/ui/views/tabs/tab_container_impl.h"

#if BUILDFLAG(IS_WIN)
#include "ui/gfx/win/hwnd_util.h"
#endif

// Overrides TabContainer::TabInsertionParams construction in
// TabStrip::AddTabsAt.
#define param(TAB, MODEL_INDEX, PINNED) \
  param(std::make_unique<BraveTab>(tab_data.handle, this), MODEL_INDEX, PINNED)
#define TabContainerImpl BraveTabContainer
#define TabHoverCardController BraveTabHoverCardController

#include "src/chrome/browser/ui/views/tabs/tab_strip.cc"

#undef TabHoverCardController
#undef TabContainerImpl
#undef param

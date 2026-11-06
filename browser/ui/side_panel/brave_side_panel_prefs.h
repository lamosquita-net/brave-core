/* Copyright (c) 2026 lamosquita. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_BROWSER_UI_SIDE_PANEL_BRAVE_SIDE_PANEL_PREFS_H_
#define BRAVE_BROWSER_UI_SIDE_PANEL_BRAVE_SIDE_PANEL_PREFS_H_

namespace brave {

// Width of BraveSidePanel. It kept the name it had while the Brave Sidebar
// (removed in FlyWeb) owned it, so stored widths carry over.
inline constexpr char kSidePanelWidth[] = "brave.sidebar.side_panel_width";
inline constexpr int kDefaultSidePanelWidth = 320;

}  // namespace brave

#endif  // BRAVE_BROWSER_UI_SIDE_PANEL_BRAVE_SIDE_PANEL_PREFS_H_

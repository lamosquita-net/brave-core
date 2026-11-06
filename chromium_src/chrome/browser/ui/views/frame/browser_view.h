/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_CHROMIUM_SRC_CHROME_BROWSER_UI_VIEWS_FRAME_BROWSER_VIEW_H_
#define BRAVE_CHROMIUM_SRC_CHROME_BROWSER_UI_VIEWS_FRAME_BROWSER_VIEW_H_

#include "brave/browser/ui/brave_browser_window.h"
#include "brave/browser/ui/views/bookmarks/brave_bookmark_bar_view.h"
#include "build/build_config.h"
#include "chrome/browser/ui/exclusive_access/exclusive_access_context.h"

#define BrowserViewLayoutDelegateImpl \
  BrowserViewLayoutDelegateImpl;      \
  friend class BraveBrowserView
#define BrowserWindow BraveBrowserWindow
#define BookmarkBarView BraveBookmarkBarView

#define MaybeShowReadingListInSidePanelIPH \
  virtual MaybeShowReadingListInSidePanelIPH

#if BUILDFLAG(IS_WIN)
// On Windows <winuser.h> defines LoadAccelerators
// Using push_macro seems to be causing #undef not to work in Chromium 125.
// Unclear what causes this.
// #pragma push_macro("LoadAccelerators")
#undef LoadAccelerators
#endif
#define LoadAccelerators virtual LoadAccelerators

#include "src/chrome/browser/ui/views/frame/browser_view.h"  // IWYU pragma: export

#undef LoadAccelerators
#if BUILDFLAG(IS_WIN)
// #pragma pop_macro("LoadAccelerators")
#endif

#undef MaybeShowReadingListInSidePanelIPH
#undef BookmarkBarView
#undef BrowserWindow
#undef BrowserViewLayoutDelegateImpl

#endif  // BRAVE_CHROMIUM_SRC_CHROME_BROWSER_UI_VIEWS_FRAME_BROWSER_VIEW_H_

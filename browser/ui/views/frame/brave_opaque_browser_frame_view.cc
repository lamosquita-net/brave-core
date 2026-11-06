/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "brave/browser/ui/views/frame/brave_opaque_browser_frame_view.h"

#include "brave/browser/ui/views/frame/brave_non_client_hit_test_helper.h"
#include "chrome/browser/ui/views/frame/browser_view.h"
#include "ui/base/hit_test.h"
#include "ui/base/metadata/metadata_impl_macros.h"

BraveOpaqueBrowserFrameView::BraveOpaqueBrowserFrameView(
    BrowserWidget* widget,
    BrowserView* browser_view,
    OpaqueBrowserFrameViewLayout* layout)
    : OpaqueBrowserFrameView(widget, browser_view, layout) {}

BraveOpaqueBrowserFrameView::~BraveOpaqueBrowserFrameView() = default;

int BraveOpaqueBrowserFrameView::NonClientHitTest(const gfx::Point& point) {
  if (auto res = brave::NonClientHitTest(browser_view(), point);
      res != HTNOWHERE) {
    return res;
  }

  return OpaqueBrowserFrameView::NonClientHitTest(point);
}

BEGIN_METADATA(BraveOpaqueBrowserFrameView)
END_METADATA

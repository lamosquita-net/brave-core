/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "brave/browser/ui/views/frame/brave_browser_frame_view_mac.h"

#include "brave/browser/ui/views/frame/brave_non_client_hit_test_helper.h"
#include "chrome/browser/ui/views/frame/browser_view.h"
#include "ui/base/hit_test.h"

BraveBrowserFrameViewMac::BraveBrowserFrameViewMac(
    BrowserWidget* browser_widget,
    BrowserView* browser_view)
    : BrowserFrameViewMac(browser_widget, browser_view) {}

BraveBrowserFrameViewMac::~BraveBrowserFrameViewMac() = default;

int BraveBrowserFrameViewMac::NonClientHitTest(const gfx::Point& point) {
  if (auto res = brave::NonClientHitTest(GetBrowserView(), point);
      res != HTNOWHERE) {
    return res;
  }

  return BrowserFrameViewMac::NonClientHitTest(point);
}

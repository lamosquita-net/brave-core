/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "brave/browser/ui/views/frame/brave_browser_frame_view_win.h"

#include "brave/browser/ui/views/frame/brave_non_client_hit_test_helper.h"
#include "chrome/browser/ui/views/frame/browser_caption_button_container_win.h"
#include "chrome/browser/ui/views/frame/browser_view.h"
#include "ui/base/hit_test.h"
#include "ui/gfx/geometry/rect.h"

BraveBrowserFrameViewWin::BraveBrowserFrameViewWin(
    BrowserWidget* browser_widget,
    BrowserView* browser_view)
    : BrowserFrameViewWin(browser_widget, browser_view) {}

BraveBrowserFrameViewWin::~BraveBrowserFrameViewWin() = default;

int BraveBrowserFrameViewWin::NonClientHitTest(const gfx::Point& point) {
  auto result = BrowserFrameViewWin::NonClientHitTest(point);
  if (result != HTCLIENT) {
    return result;
  }

  if (caption_button_container_) {
    // When we use custom caption button container, it could return HTCLIENT.
    // We shouldn't override it.
    gfx::Point local_point = point;
    ConvertPointToTarget(parent(), caption_button_container_, &local_point);
    if (caption_button_container_->HitTestPoint(local_point)) {
      const int hit_test_result =
          caption_button_container_->NonClientHitTest(local_point);
      if (hit_test_result != HTNOWHERE) {
        return hit_test_result;
      }
    }
  }

  if (auto overridden_result = brave::NonClientHitTest(browser_view(), point);
      overridden_result != HTNOWHERE) {
    return overridden_result;
  }

  return result;
}

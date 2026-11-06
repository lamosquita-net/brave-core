/* Copyright 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_BROWSER_UI_VIEWS_FRAME_BRAVE_BROWSER_FRAME_VIEW_MAC_H_
#define BRAVE_BROWSER_UI_VIEWS_FRAME_BRAVE_BROWSER_FRAME_VIEW_MAC_H_

#include "chrome/browser/ui/views/frame/browser_frame_view_mac.h"

class BraveBrowserFrameViewMac : public BrowserFrameViewMac {
 public:
  BraveBrowserFrameViewMac(BrowserWidget* browser_widget,
                           BrowserView* browser_view);
  ~BraveBrowserFrameViewMac() override;

  BraveBrowserFrameViewMac(const BraveBrowserFrameViewMac&) = delete;
  BraveBrowserFrameViewMac& operator=(const BraveBrowserFrameViewMac&) = delete;

 private:
  // BrowserFrameViewMac overrides:
  int GetTopInset(bool restored) const override;
  int NonClientHitTest(const gfx::Point& point) override;
};

#endif  // BRAVE_BROWSER_UI_VIEWS_FRAME_BRAVE_BROWSER_FRAME_VIEW_MAC_H_

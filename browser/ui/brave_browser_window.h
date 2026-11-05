/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_BROWSER_UI_BRAVE_BROWSER_WINDOW_H_
#define BRAVE_BROWSER_UI_BRAVE_BROWSER_WINDOW_H_

#include <string>

#include "chrome/browser/ui/browser_window.h"

class BrowserWindowInterface;

namespace content {
class WebContents;
}  // namespace content

class BraveBrowserWindow : public BrowserWindow {
 public:
  ~BraveBrowserWindow() override {}

  static BraveBrowserWindow* From(BrowserWindow*);
  static BraveBrowserWindow* FromBrowser(BrowserWindowInterface* browser);

  virtual void StartTabCycling() {}

  // Returns the rectangle info of the Shield's panel.
  // Renderers will call this to check if the bottom of the panel exceeds
  // the overall screen's height
  virtual gfx::Rect GetShieldsBubbleRect();

#if defined(TOOLKIT_VIEWS)
  virtual bool HasSelectedURL() const;
  virtual void CleanAndCopySelectedURL();

  // Returns true when bubble is shown.
  virtual bool ShowBraveHelpBubbleView(const std::string& text);
#endif

  // Returns true if all tabs in this window is being dragged.
  virtual bool IsInTabDragging() const;
};

#endif  // BRAVE_BROWSER_UI_BRAVE_BROWSER_WINDOW_H_

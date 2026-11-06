/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_CHROMIUM_SRC_CHROME_BROWSER_UI_VIEWS_TABS_TAB_ALERT_INDICATOR_BUTTON_H_
#define BRAVE_CHROMIUM_SRC_CHROME_BROWSER_UI_VIEWS_TABS_TAB_ALERT_INDICATOR_BUTTON_H_

#include "ui/base/metadata/metadata_header_macros.h"

class AlertIndicatorButton;
using AlertIndicatorButton_BraveImpl = AlertIndicatorButton;
#define AlertIndicatorButton AlertIndicatorButtonBase

// Make our AlertIndicatorButton a friend of the base one so that it can
// access `delegate_`.
#define TabContentsTest \
  TabContentsTest;      \
  friend AlertIndicatorButton_BraveImpl

// Let the delegate (Tab) tell whether kTabMuteIndicatorNotClickable is set.
#define IsApparentlyActive                                \
  IsTabMuteIndicatorNotClickable() { return false; }      \
  virtual bool IsApparentlyActive

#define UpdateEnabledForMuteToggle virtual UpdateEnabledForMuteToggle
#include "src/chrome/browser/ui/views/tabs/tab/alert_indicator_button.h"  // IWYU pragma: export
#undef UpdateEnabledForMuteToggle
#undef IsApparentlyActive
#undef TabContentsTest
#undef AlertIndicatorButton

class AlertIndicatorButton : public AlertIndicatorButtonBase {
  METADATA_HEADER(AlertIndicatorButton, AlertIndicatorButtonBase)
 public:
  using AlertIndicatorButtonBase::AlertIndicatorButtonBase;
  void UpdateEnabledForMuteToggle() override;
};

#endif  // BRAVE_CHROMIUM_SRC_CHROME_BROWSER_UI_VIEWS_TABS_TAB_ALERT_INDICATOR_BUTTON_H_

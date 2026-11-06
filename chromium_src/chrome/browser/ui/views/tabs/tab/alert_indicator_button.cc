/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "chrome/browser/ui/views/tabs/tab/alert_indicator_button.h"

#include "ui/base/metadata/metadata_impl_macros.h"

#define AlertIndicatorButton AlertIndicatorButtonBase

#include "src/chrome/browser/ui/views/tabs/tab/alert_indicator_button.cc"

#undef AlertIndicatorButton

void AlertIndicatorButton::UpdateEnabledForMuteToggle() {
  // We have clickable mute indicators enabled by default. Thus, if our pref is
  // enabled we can force the indicator off.
  if (delegate_->IsTabMuteIndicatorNotClickable()) {
    if (GetEnabled()) {
      SetEnabled(false);
    }
    return;
  }
  AlertIndicatorButtonBase::UpdateEnabledForMuteToggle();
}

BEGIN_METADATA(AlertIndicatorButton)
END_METADATA

/* Copyright (c) 2026 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "chrome/browser/ui/page_action/action_ids.h"
#include "chrome/browser/ui/page_action/page_action_controller.h"
#include "chrome/browser/ui/page_action/page_action_model.h"

// Never show these page actions in the omnibox:
// - the bookmark star: we have our own bookmark button in the toolbar;
// - cookie controls: Shields handle cookies.
// SetShowRequested() is only called from Show() and Hide(), where `action_id`
// is the action being toggled; hiding is left untouched.
#define SetShowRequested(pass_key, requested)                                 \
  SetShowRequested(pass_key, (requested) &&                                   \
                                 action_id != kActionBookmarkThisTab &&       \
                                 action_id != kActionShowCookieControls)

#include "src/chrome/browser/ui/page_action/page_action_controller.cc"

#undef SetShowRequested

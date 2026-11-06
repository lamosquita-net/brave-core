/* Copyright (c) 2024 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_CHROMIUM_SRC_UI_VIEWS_WIDGET_WIDGET_DELEGATE_H_
#define BRAVE_CHROMIUM_SRC_UI_VIEWS_WIDGET_WIDGET_DELEGATE_H_

// Public GetWeakPtr() for WidgetDelegate (used by the permission prompt
// bubble override), as in Brave 1.98. FlyWeb has none of the CreatePassKey
// friends 1.98 adds here (ads popup, tooltips, MenuButtonDelegate).
#define set_desired_bounds_delegate                                 \
  set_desired_bounds_delegate_unused();                             \
  base::WeakPtr<WidgetDelegate> GetWeakPtr() {                      \
    return weak_ptr_factory_.GetWeakPtr();                          \
  }                                                                 \
  void set_desired_bounds_delegate

#include "src/ui/views/widget/widget_delegate.h"  // IWYU pragma: export

#undef set_desired_bounds_delegate

#endif  // BRAVE_CHROMIUM_SRC_UI_VIEWS_WIDGET_WIDGET_DELEGATE_H_

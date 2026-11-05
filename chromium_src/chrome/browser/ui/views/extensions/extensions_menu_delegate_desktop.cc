/* Copyright (c) 2024 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

// 156 renamed ExtensionsMenuViewController to ExtensionsMenuDelegateDesktop;
// the patch opens BraveExtensionsMenuMainPageView in OpenMainPage() (as in
// Brave 1.98).
#include "brave/browser/ui/views/extensions/brave_extensions_menu_main_page_view.h"

#include "src/chrome/browser/ui/views/extensions/extensions_menu_delegate_desktop.cc"

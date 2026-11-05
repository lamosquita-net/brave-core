/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/browser/ui/brave_browser.h"
#include "brave/browser/ui/brave_tab_strip_model_delegate.h"
#include "brave/browser/ui/tabs/brave_tab_strip_model.h"

// 156 creates the command controller, the content setting bubble delegate and
// the location bar model delegate in BrowserWindowFeatures and
// BrowserLocationBarModelDelegate, and the bookmark tab helper in
// TabFeatures; only the Browser, its tab strip model and its delegate are
// created here (the patch, as in Brave 1.98).
#include "src/chrome/browser/ui/browser.cc"

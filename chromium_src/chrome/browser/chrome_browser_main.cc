/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "chrome/browser/chrome_browser_main.h"
#include "brave/browser/brave_browser_process_impl.h"
#include "build/build_config.h"

// FlyWeb: on 156 ChromeBrowserMainParts::Create() builds the Mac main parts
// here, no longer in chrome_content_browser_client.cc (as Brave 1.98).
// BraveBrowserMainPartsMac registers Sparkle and backs up a 2.x profile
// before its first run.
#if BUILDFLAG(IS_MAC)
#include "brave/browser/brave_browser_main_parts_mac.h"
#undef ChromeBrowserMainPartsMac
#define ChromeBrowserMainPartsMac BraveBrowserMainPartsMac
#endif  // BUILDFLAG(IS_MAC)

#define BrowserProcessImpl BraveBrowserProcessImpl
#include "src/chrome/browser/chrome_browser_main.cc"
#undef BrowserProcessImpl

#if BUILDFLAG(IS_MAC)
#undef ChromeBrowserMainPartsMac
#endif  // BUILDFLAG(IS_MAC)

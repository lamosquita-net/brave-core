/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "ui/base/clipboard/clipboard.h"

// Lets the embedder's BrowserMainParts run its PreShutdown() step (clear
// browsing data on exit) at the start of the shutdown sequence.
#define OnPreShutdownForCurrentThread() \
  OnPreShutdownForCurrentThread();      \
  if (parts_) {                         \
    parts_->PreShutdown();              \
  }

#include "src/content/browser/browser_main_loop.cc"
#undef OnPreShutdownForCurrentThread

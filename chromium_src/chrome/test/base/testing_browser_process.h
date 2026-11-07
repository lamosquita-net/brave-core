/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_CHROMIUM_SRC_CHROME_TEST_BASE_TESTING_BROWSER_PROCESS_H_
#define BRAVE_CHROMIUM_SRC_CHROME_TEST_BASE_TESTING_BROWSER_PROCESS_H_

// No class rename here: in 156, testing_browser_process.cc includes
// runtime_init.h and testing_profile_manager.h, and renaming the class
// would mangle their declarations (undefined symbols at link time).
// Brave's lifetime hooks live in the .cc override plus a small patch.
#include "src/chrome/test/base/testing_browser_process.h"  // IWYU pragma: export

#include "brave/test/base/testing_brave_browser_process.h"  // IWYU pragma: export

#endif  // BRAVE_CHROMIUM_SRC_CHROME_TEST_BASE_TESTING_BROWSER_PROCESS_H_

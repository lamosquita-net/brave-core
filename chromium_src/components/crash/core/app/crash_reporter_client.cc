/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

// FlyWeb: crash reports stay on disk (Crashpad database in the profile
// directory) and are never uploaded. With an empty URL the Crashpad handler
// does not start its upload thread (handler_main.cc).
#define BRAVE_CRASH_REPORTER_CLIENT_GET_UPLOAD_URL \
  return std::string();

#include "src/components/crash/core/app/crash_reporter_client.cc"
#undef BRAVE_CRASH_REPORTER_CLIENT_GET_UPLOAD_URL

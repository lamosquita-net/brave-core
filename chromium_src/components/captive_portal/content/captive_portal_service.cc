/* Copyright (c) 2023 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "components/captive_portal/content/captive_portal_service.h"

// FlyWeb: captive portal detection is always off. Brave forced it on
// (ignoring the "resolve navigation errors" pref) and pointed it at its own
// plain-HTTP host (detectportal.brave-http-only.com), so every check told
// Brave the user's IP. macOS already detects captive portals on its own (the
// Captive Network Assistant opens the Wi-Fi login page), so nothing is lost.
// Only use of DISABLED_FOR_TESTING in the .cc is UpdateEnabledState():
//   enabled_ = testing_state_ != DISABLED_FOR_TESTING && <pref>;
#define DISABLED_FOR_TESTING DISABLED_FOR_TESTING && false
#include "src/components/captive_portal/content/captive_portal_service.cc"
#undef DISABLED_FOR_TESTING

/* Copyright (c) 2023 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "components/captive_portal/core/captive_portal_detector.h"

// FlyWeb: detection is off (captive_portal_service.cc). If anything still
// called the detector, it would get a host that never resolves instead of
// Brave's detectportal.brave-http-only.com or Google's.
#define kDefaultURL                                     \
  kDefaultURL[] = "http://detectportal.flyweb.invalid/"; \
  const char kEmpty
#include "src/components/captive_portal/core/captive_portal_detector.cc"
#undef kDefaultURL

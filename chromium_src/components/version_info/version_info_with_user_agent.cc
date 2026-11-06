/* Copyright (c) 2025 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

// The User-Agent carries "Chrome/<real Chromium version>", not the product
// version (patch, as in 1.98). browser/flyweb/declared_version.h then declares
// the engine level on top of it.
#include "brave/components/version_info/version_info_values.h"

#include "src/components/version_info/version_info_with_user_agent.cc"

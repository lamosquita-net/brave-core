/* Copyright (c) 2026 lamosquita. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include <string_view>

#include "components/version_info/version_info.h"

// FlyWeb: the Safe Browsing client (requests to our proxy, via
// GetV4ProtocolConfig) declares the Chrome version of the declared engine
// level, as the User-Agent does, not Brave's "128.1.69.x".
namespace version_info {
constexpr std::string_view FlyWebDeclaredVersionNumber() {
  return constants::kFlyWebEngineFullVersion;
}
}  // namespace version_info

#define GetVersionNumber FlyWebDeclaredVersionNumber
#include "src/components/safe_browsing/core/browser/db/v4_protocol_manager_util.cc"
#undef GetVersionNumber

/* Copyright (c) 2026 lamosquita. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_BROWSER_FLYWEB_DECLARED_VERSION_H_
#define BRAVE_BROWSER_FLYWEB_DECLARED_VERSION_H_

#include <string>

namespace flyweb {

// Chrome major version FlyWeb declares to websites (User-Agent and client
// hints): the last complete engine level, not the real 116. Raised only when
// every Baseline feature Chrome shipped up to that version is in FlyWeb
// (softmac FlyWeb/docs/motor.md). Extensions, components and the updater keep
// using the real version.
inline constexpr int kDeclaredEngineLevel = 117;

// "117"
std::string DeclaredMajor();

// Real major -> declared major: "116" -> "117", "116.0.5845.188" ->
// "117.0.5845.188". Anything else (e.g. GREASE brand versions) is unchanged.
std::string DeclareVersion(const std::string& version);

// "... Chrome/116.0.0.0 Safari/537.36" -> "... Chrome/117.0.0.0 Safari/537.36".
std::string DeclareUserAgent(std::string user_agent);

}  // namespace flyweb

#endif  // BRAVE_BROWSER_FLYWEB_DECLARED_VERSION_H_

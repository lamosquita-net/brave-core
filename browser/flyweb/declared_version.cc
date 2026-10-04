/* Copyright (c) 2026 lamosquita. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "brave/browser/flyweb/declared_version.h"

#include "base/strings/strcat.h"
#include "base/strings/string_number_conversions.h"
#include "base/strings/string_util.h"
#include "components/version_info/version_info.h"

namespace flyweb {

std::string DeclaredMajor() {
  return base::NumberToString(kDeclaredEngineLevel);
}

std::string DeclareVersion(const std::string& version) {
  const std::string real = version_info::GetMajorVersionNumber();
  if (version == real) {
    return DeclaredMajor();
  }
  if (base::StartsWith(version, base::StrCat({real, "."}))) {
    return base::StrCat({DeclaredMajor(), version.substr(real.size())});
  }
  return version;
}

std::string DeclareUserAgent(std::string user_agent) {
  base::ReplaceFirstSubstringAfterOffset(
      &user_agent, 0,
      base::StrCat({"Chrome/", version_info::GetMajorVersionNumber(), "."}),
      base::StrCat({"Chrome/", DeclaredMajor(), "."}));
  return user_agent;
}

}  // namespace flyweb

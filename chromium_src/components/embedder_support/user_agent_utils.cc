/* Copyright (c) 2022 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "components/embedder_support/user_agent_utils.h"

#include <optional>
#include <string>

#include "base/strings/string_number_conversions.h"
#include "base/version.h"
#include "build/build_config.h"
#include "components/version_info/version_info.h"

// Chromium uses `version_info::GetProductName()` to get the browser's "brand"
// name, but on MacOS we use different names for different channels (adding Beta
// or Nightly, for example). In the UA client hint, though, we want a consistent
// name regardless of the channel, so we just hard-code it. Note, that we use
// IDS_PRODUCT_NAME from app/chromium_strings.grd (brave_strings.grd) in
// constructing the UA in brave/browser/brave_content_browser_client.cc, but we
// can't use it here in the //components.
// FlyWeb: no own brand in the client hints at all (only "Chromium" plus the
// GREASE brand, like an unbranded Chromium). Sending "Brave" would be Brave
// branding, and a rare "FlyWeb" brand would make users easier to fingerprint.
#define BRAVE_GET_USER_AGENT_BRAND_LIST brand = std::nullopt;

// FlyWeb (F4.8): the full brand versions are the declared real full version,
// not Brave's zeroed "major.0.0.0", which no Chrome sends.
#define BRAVE_BRAND_VERSION_OVERRIDE_FOR_FULL_BRAND_VERSION_TYPE full_version

// No device model in the User-Agent or the client hints (it was
// content/common/user_agent.cc before 156; as 1.98). BuildModelInfo() is
// already empty on desktop; this keeps it empty everywhere.
#define BRAVE_GET_ANDROID_OS_INFO \
  include_android_model = IncludeAndroidModel::Exclude;

namespace embedder_support {
std::string BuildModelInfo_ChromiumImpl();
std::string BuildModelInfo() {
  return std::string();
}
}  // namespace embedder_support

// The original's own calls go to the renamed one: declare it, with the
// header's default argument, before the include.
namespace embedder_support {
blink::UserAgentMetadata GetUserAgentMetadata_ChromiumImpl(
    bool only_low_entropy_ch = false);
}  // namespace embedder_support

#define GetUserAgentMetadata GetUserAgentMetadata_ChromiumImpl
// Inside the original, BuildModelInfo is the renamed one; the empty override
// above wins, and HardwareModelName() is not used for the model.
#define HardwareModelName() HardwareModelName() == "" ? "" : ""
#define BuildModelInfo BuildModelInfo_ChromiumImpl
#include "src/components/embedder_support/user_agent_utils.cc"
#undef BuildModelInfo
#undef HardwareModelName
#undef GetUserAgentMetadata
#undef BRAVE_GET_ANDROID_OS_INFO
#undef BRAVE_BRAND_VERSION_OVERRIDE_FOR_FULL_BRAND_VERSION_TYPE
#undef BRAVE_GET_USER_AGENT_BRAND_LIST

namespace embedder_support {

blink::UserAgentMetadata GetUserAgentMetadata(bool only_low_entropy_ch) {
  blink::UserAgentMetadata metadata =
      GetUserAgentMetadata_ChromiumImpl(only_low_entropy_ch);
  // A user-agent override on the command line keeps Chromium's answer (low
  // entropy only, or blank with kUACHOverrideBlank).
  if (GetUserAgentFromCommandLine().has_value()) {
    return metadata;
  }
  // FlyWeb (F4.8): the client hints declare the engine level, built the way
  // Chromium builds them for that version (the GREASE brand is seeded by the
  // major version), so Sec-CH-UA, navigator.userAgentData and the User-Agent
  // agree. With level 156 on Chromium 156 this equals Chromium's own answer,
  // apart from the full version (the last Mac Stable of the level). Every
  // caller goes through here: request headers and the renderer.
  const std::string declared_major =
      base::NumberToString(version_info::constants::kFlyWebEngineLevel);
  const std::string declared_full =
      version_info::constants::kFlyWebEngineFullVersion;
  metadata.brand_version_list = GetUserAgentBrandList(
      declared_major, declared_full,
      blink::UserAgentBrandVersionType::kMajorVersion, std::nullopt);
  if (only_low_entropy_ch) {
    return metadata;
  }
  metadata.brand_full_version_list = GetUserAgentBrandList(
      declared_major, declared_full,
      blink::UserAgentBrandVersionType::kFullVersion, std::nullopt);
  metadata.full_version = declared_full;
#if BUILDFLAG(IS_MAC)
  // FlyWeb (F4.8): never a macOS older than the declared Chrome supports.
  const base::Version platform_version(metadata.platform_version);
  const base::Version min_macos(
      version_info::constants::kFlyWebMinMacOSVersion);
  if (!platform_version.IsValid() || platform_version < min_macos) {
    metadata.platform_version = min_macos.GetString();
  }
#endif
  return metadata;
}

}  // namespace embedder_support

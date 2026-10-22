/* Copyright (c) 2022 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "components/embedder_support/user_agent_utils.h"

#include "base/command_line.h"
#include "base/strings/string_number_conversions.h"
#include "base/version.h"
#include "build/build_config.h"
#include "components/embedder_support/switches.h"

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

// The one-argument overload of the original calls the two-argument one, which
// the define renames too: declare the renamed one before it is used.
namespace embedder_support {
blink::UserAgentMetadata GetUserAgentMetadata_ChromiumImpl(
    const PrefService* pref_service,
    bool only_low_entropy_ch);
}  // namespace embedder_support

#define GetUserAgentMetadata GetUserAgentMetadata_ChromiumImpl
#include "src/components/embedder_support/user_agent_utils.cc"
#undef GetUserAgentMetadata
#undef BRAVE_BRAND_VERSION_OVERRIDE_FOR_FULL_BRAND_VERSION_TYPE
#undef BRAVE_GET_USER_AGENT_BRAND_LIST

namespace embedder_support {

blink::UserAgentMetadata GetUserAgentMetadata(bool only_low_entropy_ch) {
  return GetUserAgentMetadata(nullptr, only_low_entropy_ch);
}

blink::UserAgentMetadata GetUserAgentMetadata(const PrefService* pref_service,
                                              bool only_low_entropy_ch) {
  blink::UserAgentMetadata metadata =
      GetUserAgentMetadata_ChromiumImpl(pref_service, only_low_entropy_ch);
  base::CommandLine* command_line = base::CommandLine::ForCurrentProcess();
  if (command_line->HasSwitch(kUserAgent)) {
    return metadata;
  }
  // FlyWeb (F4.8): the client hints declare the engine level, built the way
  // Chromium builds them for that version (the GREASE brand is seeded by the
  // major version), so Sec-CH-UA, navigator.userAgentData and the User-Agent
  // agree. Every caller goes through here: the request headers
  // (client_hints::ClientHints) and the renderer (ContentBrowserClient).
  const std::string declared_major =
      base::NumberToString(version_info::constants::kFlyWebEngineLevel);
  const std::string declared_full =
      version_info::constants::kFlyWebEngineFullVersion;
  bool enable_updated_grease_by_policy = true;
  if (pref_service &&
      pref_service->HasPrefPath(
          policy::policy_prefs::kUserAgentClientHintsGREASEUpdateEnabled)) {
    enable_updated_grease_by_policy = pref_service->GetBoolean(
        policy::policy_prefs::kUserAgentClientHintsGREASEUpdateEnabled);
  }
  metadata.brand_version_list = GetUserAgentBrandList(
      declared_major, enable_updated_grease_by_policy, declared_full,
      blink::UserAgentBrandVersionType::kMajorVersion);
  if (only_low_entropy_ch) {
    return metadata;
  }
  metadata.brand_full_version_list = GetUserAgentBrandList(
      declared_major, enable_updated_grease_by_policy, declared_full,
      blink::UserAgentBrandVersionType::kFullVersion);
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

/* Copyright (c) 2026 lamosquita. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "brave/browser/flyweb/user_agent_override_tab_helper.h"

#include "base/strings/strcat.h"
#include "base/strings/string_number_conversions.h"
#include "base/strings/string_split.h"
#include "base/strings/string_util.h"
#include "chrome/browser/browser_process.h"
#include "components/embedder_support/user_agent_utils.h"
#include "components/version_info/version_info.h"
#include "content/public/browser/navigation_handle.h"
#include "content/public/browser/web_contents.h"
#include "third_party/blink/public/common/user_agent/user_agent_metadata.h"
#include "url/gurl.h"

namespace flyweb {

// FlyWeb: off by default (decision of 04-10-2026). FlyWeb declares its real
// engine level everywhere (declared_version.h) and raises it as the engine
// grows, so sites that warn about an old browser show where it stands. Kept as
// an escape hatch: --enable-features=FlyWebUserAgentOverride turns it back on.
BASE_FEATURE(kFlyWebUserAgentOverride,
             "FlyWebUserAgentOverride",
             base::FEATURE_DISABLED_BY_DEFAULT);

// Current stable Chrome when this was written (01-10-2026). Raise it in each
// maintenance cycle (FlyWeb/docs/compatibilidad.md).
const base::FeatureParam<int> kDeclaredChromeMajor{&kFlyWebUserAgentOverride,
                                                   "chrome_major", 153};
const base::FeatureParam<std::string> kOverrideSites{
    &kFlyWebUserAgentOverride, "sites",
    "drive.google.com,docs.google.com,mail.google.com"};
const base::FeatureParam<std::string> kDeclaredPlatformVersion{
    &kFlyWebUserAgentOverride, "platform_version", "14.7.0"};

bool HostMatchesSites(const std::string& host, const std::string& sites) {
  for (const auto& site : base::SplitStringPiece(
           sites, ",", base::TRIM_WHITESPACE, base::SPLIT_WANT_NONEMPTY)) {
    if (host == site || base::EndsWith(host, base::StrCat({".", site}))) {
      return true;
    }
  }
  return false;
}

namespace {

// "116.0.5845.188" -> "153.0.5845.188"; anything else is left untouched.
std::string ReplaceMajor(const std::string& version,
                         const std::string& current,
                         const std::string& declared) {
  if (version == current) {
    return declared;
  }
  if (base::StartsWith(version, base::StrCat({current, "."}))) {
    return base::StrCat({declared, version.substr(current.size())});
  }
  return version;
}

// "153.1.57.64" -> "153.0.0.0", as BraveContentBrowserClient does for the
// normal metadata: the full versions would otherwise carry Brave's version.
std::string ReduceToMajor(const std::string& version) {
  return base::StrCat({version.substr(0, version.find('.')), ".0.0.0"});
}

blink::UserAgentOverride BuildOverride() {
  const std::string current = version_info::GetMajorVersionNumber();
  const std::string declared =
      base::NumberToString(kDeclaredChromeMajor.Get());

  blink::UserAgentOverride ua_override;
  ua_override.ua_string_override = embedder_support::GetUserAgent();
  base::ReplaceFirstSubstringAfterOffset(
      &ua_override.ua_string_override, 0, base::StrCat({"Chrome/", current, "."}),
      base::StrCat({"Chrome/", declared, "."}));

  blink::UserAgentMetadata metadata =
      embedder_support::GetUserAgentMetadata(g_browser_process->local_state());
  // GREASE brands ("Not=A?Brand") carry other version numbers and stay as is.
  for (auto& brand_version : metadata.brand_version_list) {
    brand_version.version =
        ReplaceMajor(brand_version.version, current, declared);
  }
  for (auto& brand_version : metadata.brand_full_version_list) {
    brand_version.version = ReduceToMajor(
        ReplaceMajor(brand_version.version, current, declared));
  }
  metadata.full_version =
      ReduceToMajor(ReplaceMajor(metadata.full_version, current, declared));
  metadata.platform_version = kDeclaredPlatformVersion.Get();
  ua_override.ua_metadata_override = metadata;
  return ua_override;
}

}  // namespace

UserAgentOverrideTabHelper::UserAgentOverrideTabHelper(
    content::WebContents* web_contents)
    : content::WebContentsObserver(web_contents),
      content::WebContentsUserData<UserAgentOverrideTabHelper>(*web_contents) {}

UserAgentOverrideTabHelper::~UserAgentOverrideTabHelper() = default;

void UserAgentOverrideTabHelper::DidStartNavigation(
    content::NavigationHandle* navigation_handle) {
  Decide(navigation_handle);
}

void UserAgentOverrideTabHelper::DidRedirectNavigation(
    content::NavigationHandle* navigation_handle) {
  Decide(navigation_handle);
}

void UserAgentOverrideTabHelper::Decide(
    content::NavigationHandle* navigation_handle) {
  if (!navigation_handle->IsInPrimaryMainFrame() ||
      navigation_handle->IsSameDocument()) {
    return;
  }
  const GURL& url = navigation_handle->GetURL();
  const bool override_ua =
      url.SchemeIsHTTPOrHTTPS() &&
      HostMatchesSites(url.host(), kOverrideSites.Get());
  if (override_ua) {
    web_contents()->SetUserAgentOverride(BuildOverride(),
                                         /*override_in_new_tabs=*/false);
  }
  // Subframes and subresources of this page follow the main frame's choice.
  navigation_handle->SetIsOverridingUserAgent(override_ua);
}

WEB_CONTENTS_USER_DATA_KEY_IMPL(UserAgentOverrideTabHelper);

}  // namespace flyweb

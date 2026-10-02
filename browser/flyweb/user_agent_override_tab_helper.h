/* Copyright (c) 2026 lamosquita. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_BROWSER_FLYWEB_USER_AGENT_OVERRIDE_TAB_HELPER_H_
#define BRAVE_BROWSER_FLYWEB_USER_AGENT_OVERRIDE_TAB_HELPER_H_

#include <string>

#include "base/feature_list.h"
#include "base/metrics/field_trial_params.h"
#include "content/public/browser/web_contents_observer.h"
#include "content/public/browser/web_contents_user_data.h"

namespace flyweb {

// Declares a newer Chrome major version, in the User-Agent string and in the
// User-Agent Client Hints at the same time, only on the listed sites. Some
// sites (Google Drive) show an "unsupported browser" banner to Chrome 116
// even though everything works. Per site and not global because a site that
// believes it talks to a newer Chrome may use features 116 lacks.
//
// Tune without rebuilding (the commas inside a parameter must be written as
// %2C, because --enable-features itself is comma-separated):
//   --enable-features=FlyWebUserAgentOverride:chrome_major/154/sites/drive.google.com%2Cdocs.google.com
// Turn off: --disable-features=FlyWebUserAgentOverride
BASE_DECLARE_FEATURE(kFlyWebUserAgentOverride);
extern const base::FeatureParam<int> kDeclaredChromeMajor;
// Comma-separated hosts; each also matches its subdomains.
extern const base::FeatureParam<std::string> kOverrideSites;
// macOS version declared in the platformVersion client hint on those sites.
// The real one (10.14) is older than any macOS the declared Chrome supports.
extern const base::FeatureParam<std::string> kDeclaredPlatformVersion;

// Exposed for tests.
bool HostMatchesSites(const std::string& host, const std::string& sites);

class UserAgentOverrideTabHelper
    : public content::WebContentsObserver,
      public content::WebContentsUserData<UserAgentOverrideTabHelper> {
 public:
  UserAgentOverrideTabHelper(const UserAgentOverrideTabHelper&) = delete;
  UserAgentOverrideTabHelper& operator=(const UserAgentOverrideTabHelper&) =
      delete;
  ~UserAgentOverrideTabHelper() override;

  // content::WebContentsObserver:
  void DidStartNavigation(
      content::NavigationHandle* navigation_handle) override;

 private:
  explicit UserAgentOverrideTabHelper(content::WebContents* web_contents);
  friend class content::WebContentsUserData<UserAgentOverrideTabHelper>;

  WEB_CONTENTS_USER_DATA_KEY_DECL();
};

}  // namespace flyweb

#endif  // BRAVE_BROWSER_FLYWEB_USER_AGENT_OVERRIDE_TAB_HELPER_H_

/* Copyright (c) 2025 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "chrome/browser/ssl/https_upgrades_util.h"

#include "brave/components/brave_shields/content/browser/brave_shields_util.h"
#include "chrome/browser/content_settings/host_content_settings_map_factory.h"
#include "chrome/browser/profiles/profile.h"

namespace {

// In 156 the HTTPS-Only interstitial state is computed here instead of in
// HttpsUpgradesNavigationThrottle::MaybeCreateThrottleFor(). Keep Brave's
// "strict" HTTPS upgrade setting (Shields) enabling the interstitial, in every
// profile as before.
bool BraveShouldForceHttps(const GURL& url, Profile* profile) {
  HostContentSettingsMap* map =
      HostContentSettingsMapFactory::GetForProfile(profile);
  return map && brave_shields::ShouldForceHttps(map, url);
}

}  // namespace

#include "src/chrome/browser/ssl/https_upgrades_util.cc"

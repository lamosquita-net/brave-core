/* Copyright (c) 2026 lamosquita. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "chrome/browser/sync/device_info_sync_client_impl.h"

#include "base/feature_list.h"
#include "components/personal_context/core/personal_context_features.h"
#include "components/personal_context/core/personal_context_key_manager.h"

// FlyWeb: Brave ships kPersonalContext off
// (chromium_src/components/personal_context/core/personal_context_features.cc),
// so PersonalContextEligibilityServiceFactory builds no service, and 156's
// GetLocalPersonalContextInfo() CHECKs that it has one. Every profile with
// Sync set up (a 2.x FlyWeb Sync chain, say) hit it on the first DeviceInfo
// reconcile, a few seconds after startup. Without the feature the device is
// simply not eligible; with it, Chromium's code runs unchanged.
//
// The rename also reaches the key manager's static of the same name, called
// from there; a shim gives it the renamed name and forwards to the real one.
namespace personal_context {
class BravePersonalContextKeyManagerShim : public PersonalContextKeyManager {
 public:
  static syncer::DeviceInfo::PersonalContextInfo::StatusOrInfo
  GetLocalPersonalContextInfo_ChromiumImpl(
      PrefService* prefs,
      const PersonalContextEligibilityService* eligibility_service) {
    return PersonalContextKeyManager::GetLocalPersonalContextInfo(
        prefs, eligibility_service);
  }
};
}  // namespace personal_context

#define PersonalContextKeyManager BravePersonalContextKeyManagerShim
#define GetLocalPersonalContextInfo GetLocalPersonalContextInfo_ChromiumImpl
#include "src/chrome/browser/sync/device_info_sync_client_impl.cc"
#undef GetLocalPersonalContextInfo
#undef PersonalContextKeyManager

namespace browser_sync {

syncer::DeviceInfo::PersonalContextInfo::StatusOrInfo
DeviceInfoSyncClientImpl::GetLocalPersonalContextInfo() const {
  if (!base::FeatureList::IsEnabled(
          personal_context::features::kPersonalContext)) {
    return syncer::DeviceInfo::PersonalContextInfo::NotEligible();
  }
  return GetLocalPersonalContextInfo_ChromiumImpl();
}

}  // namespace browser_sync

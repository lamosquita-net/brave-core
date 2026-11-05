// Copyright (c) 2020 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// you can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef BRAVE_COMPONENTS_NTP_BACKGROUND_IMAGES_BROWSER_VIEW_COUNTER_SERVICE_H_
#define BRAVE_COMPONENTS_NTP_BACKGROUND_IMAGES_BROWSER_VIEW_COUNTER_SERVICE_H_

#include <optional>

#include "base/memory/raw_ptr.h"
#include "base/values.h"
#include "components/keyed_service/core/keyed_service.h"

class PrefRegistrySimple;
class PrefService;

namespace user_prefs {
class PrefRegistrySyncable;
}  // namespace user_prefs

namespace ntp_background_images {

class BraveNTPCustomBackgroundService;

// FlyWeb: the new tab page ships its own backgrounds (brave_new_tab_ui
// components/default/flyweb), so there are no Brave photo wallpapers, no
// sponsored images and no super referral wallpapers. This service only tells
// the page whether the user chose a background of their own (image or colour).
class ViewCounterService : public KeyedService {
 public:
  ViewCounterService(BraveNTPCustomBackgroundService* custom_service,
                     PrefService* prefs);
  ~ViewCounterService() override;

  ViewCounterService(const ViewCounterService&) = delete;
  ViewCounterService& operator=(const ViewCounterService&) = delete;

  static void RegisterProfilePrefs(user_prefs::PrefRegistrySyncable* registry);
  static void RegisterProfilePrefsForMigration(
      user_prefs::PrefRegistrySyncable* registry);
  static void MigrateObsoleteProfilePrefs(PrefService* prefs);
  static void RegisterLocalStatePrefsForMigration(PrefRegistrySimple* registry);
  static void MigrateObsoleteLocalStatePrefs(PrefService* local_state);

  // The user's own background, or nullopt when the page should pick one of
  // its bundled backgrounds (or show none, when backgrounds are turned off).
  std::optional<base::DictValue> GetCurrentWallpaper() const;

 private:
  // Should we show custom background that user uploaded or fill background
  // with solid color user selected.
  bool ShouldShowCustomBackground() const;

  raw_ptr<PrefService> prefs_ = nullptr;

  // Can be null if custom background is not supported.
  raw_ptr<BraveNTPCustomBackgroundService> custom_bi_service_ = nullptr;
};

}  // namespace ntp_background_images

#endif  // BRAVE_COMPONENTS_NTP_BACKGROUND_IMAGES_BROWSER_VIEW_COUNTER_SERVICE_H_

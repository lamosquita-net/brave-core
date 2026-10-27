/* Copyright (c) 2024 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "brave/browser/ui/geolocation/geolocation_utils.h"

#import <CoreLocation/CoreLocation.h>

#include <utility>

#include "base/functional/callback.h"

namespace geolocation {

namespace {

bool GetSystemLocationSettingEnabled() {
  // Service is off globally.
  if (![CLLocationManager locationServicesEnabled]) {
    return false;
  }

  CLAuthorizationStatus status;
  if (@available(macOS 11.0, *)) {
    CLLocationManager* manager = [[CLLocationManager alloc] init];
    status = [manager authorizationStatus];
  } else {
    // FlyWeb (FM.42 b5): on 10.14/10.15 the class method (10.7, deprecated in
    // 11) gives the same app authorization status.
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
    status = [CLLocationManager authorizationStatus];
#pragma clang diagnostic pop
  }

  return status == kCLAuthorizationStatusAuthorizedAlways;
}

}  // namespace

void IsSystemLocationSettingEnabled(base::OnceCallback<void(bool)> callback) {
  std::move(callback).Run(GetSystemLocationSettingEnabled());
}

bool CanGiveDetailedGeolocationRequestInfo() {
  // FlyWeb (FM.42 b5): the system status is known on every supported macOS.
  return true;
}

}  // namespace geolocation

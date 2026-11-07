/* Copyright (c) 2026 lamosquita. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_CHROMIUM_SRC_CHROME_BROWSER_SYNC_DEVICE_INFO_SYNC_CLIENT_IMPL_H_
#define BRAVE_CHROMIUM_SRC_CHROME_BROWSER_SYNC_DEVICE_INFO_SYNC_CLIENT_IMPL_H_

// The interface first, so the rename below only reaches the Impl class.
#include "components/sync_device_info/device_info_sync_client.h"

// FlyWeb: see device_info_sync_client_impl.cc.
#define GetLocalPersonalContextInfo                     \
  GetLocalPersonalContextInfo_ChromiumImpl() const;     \
  syncer::DeviceInfo::PersonalContextInfo::StatusOrInfo \
      GetLocalPersonalContextInfo

#include "src/chrome/browser/sync/device_info_sync_client_impl.h"  // IWYU pragma: export

#undef GetLocalPersonalContextInfo

#endif  // BRAVE_CHROMIUM_SRC_CHROME_BROWSER_SYNC_DEVICE_INFO_SYNC_CLIENT_IMPL_H_

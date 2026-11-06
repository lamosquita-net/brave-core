/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "components/sync/service/sync_service_impl.h"

#include "brave/components/sync/service/brave_sync_auth_manager.h"
#include "brave/components/sync/service/brave_sync_stopped_reporter.h"

namespace syncer {

// Reporting of sync errors is disabled in Brave to prevent the profile avatar
// button from providing visual feedback to the user (it replaces the old
// GetAvatarSyncErrorType override in chrome/browser/sync/sync_ui_util.cc).
// Brave Sync works differently in that there is no sign-in and nothing to
// prompt the user to manage once their sync chain is setup.
SyncService::UserActionableError SyncServiceImpl::GetUserActionableError()
    const {
  return SyncService::UserActionableError::kNone;
}

}  // namespace syncer

#define GetUserActionableError GetUserActionableError_ChromiumImpl
#define SyncAuthManager BraveSyncAuthManager
#define SyncStoppedReporter BraveSyncStoppedReporter

#include "src/components/sync/service/sync_service_impl.cc"

#undef GetUserActionableError
#undef SyncAuthManager
#undef SyncStoppedReporter

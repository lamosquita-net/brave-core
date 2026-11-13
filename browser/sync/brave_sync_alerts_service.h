/* Copyright (c) 2022 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_BROWSER_SYNC_BRAVE_SYNC_ALERTS_SERVICE_H_
#define BRAVE_BROWSER_SYNC_BRAVE_SYNC_ALERTS_SERVICE_H_

#include "base/memory/raw_ptr.h"
#include "base/scoped_multi_source_observation.h"
#include "base/scoped_observation.h"
#include "brave/browser/sync/account/flyweb_sync_account_service.h"
#include "components/keyed_service/core/keyed_service.h"
#include "components/sync/service/sync_service.h"
#include "components/sync/service/sync_service_observer.h"

class Profile;

namespace syncer {
class SyncService;
class SyncServiceObserver;
}  // namespace syncer

class BraveSyncAlertsService : public KeyedService,
                               public syncer::SyncServiceObserver,
                               public flyweb_sync::AccountService::Observer {
 public:
  explicit BraveSyncAlertsService(Profile* profile);
  BraveSyncAlertsService(const BraveSyncAlertsService&) = delete;
  BraveSyncAlertsService& operator=(const BraveSyncAlertsService&) = delete;
  ~BraveSyncAlertsService() override;

 private:
  // syncer::SyncServiceObserver implementation.
  void OnStateChanged(syncer::SyncService* service) override;
  void OnSyncShutdown(syncer::SyncService* sync) override;

  // flyweb_sync::AccountService::Observer (FlyWeb 3.2, sync-cuenta.md §6.2):
  void OnAccountStatusChanged() override;
  void OnAccountWarnings(
      const std::vector<flyweb_sync::AccountEvent>& events) override;
  void ShowAccountInfobar(
      bool key_changed,
      const std::vector<flyweb_sync::AccountEvent>& warnings);

  void ShowInfobar();
  void ShowSyncCannotRunInfobar();

  raw_ptr<Profile> profile_ = nullptr;
  base::ScopedMultiSourceObservation<syncer::SyncService,
                                     syncer::SyncServiceObserver>
      sync_service_observer_{this};
  base::ScopedObservation<flyweb_sync::AccountService,
                          flyweb_sync::AccountService::Observer>
      account_observation_{this};
  bool key_changed_notice_shown_ = false;
  bool account_deleted_deferred_ = false;
  base::WeakPtrFactory<BraveSyncAlertsService> weak_ptr_factory_{this};
};

#endif  // BRAVE_BROWSER_SYNC_BRAVE_SYNC_ALERTS_SERVICE_H_

/* Copyright (c) 2026 The FlyWeb Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "brave/browser/sync/account/flyweb_sync_account_service_factory.h"

#include <memory>

#include "base/no_destructor.h"
#include "brave/browser/sync/account/flyweb_sync_account_service.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/browser/sync/device_info_sync_service_factory.h"
#include "chrome/browser/sync/sync_service_factory.h"
#include "components/keyed_service/content/browser_context_dependency_manager.h"

namespace flyweb_sync {

// static
AccountService* AccountServiceFactory::GetForBrowserContext(
    content::BrowserContext* context) {
  return static_cast<AccountService*>(
      GetInstance()->GetServiceForBrowserContext(context, true));
}

// static
AccountServiceFactory* AccountServiceFactory::GetInstance() {
  static base::NoDestructor<AccountServiceFactory> instance;
  return instance.get();
}

AccountServiceFactory::AccountServiceFactory()
    : BrowserContextKeyedServiceFactory(
          "FlyWebSyncAccountService",
          BrowserContextDependencyManager::GetInstance()) {
  DependsOn(SyncServiceFactory::GetInstance());
  DependsOn(DeviceInfoSyncServiceFactory::GetInstance());
}

AccountServiceFactory::~AccountServiceFactory() = default;

std::unique_ptr<KeyedService>
AccountServiceFactory::BuildServiceInstanceForBrowserContext(
    content::BrowserContext* context) const {
  Profile* profile = Profile::FromBrowserContext(context);
  if (!SyncServiceFactory::IsSyncAllowed(profile)) {
    return nullptr;
  }
  return std::make_unique<AccountService>(profile);
}

content::BrowserContext* AccountServiceFactory::GetBrowserContextToUse(
    content::BrowserContext* context) const {
  Profile* profile = Profile::FromBrowserContext(context);
  if (profile->IsOffTheRecord() || profile->IsGuestSession()) {
    return nullptr;
  }
  return context;
}

// Created with the profile: it polls the account events (§11) and notices a
// chain disabled from another Mac even with the settings page closed.
bool AccountServiceFactory::ServiceIsCreatedWithBrowserContext() const {
  return true;
}

}  // namespace flyweb_sync

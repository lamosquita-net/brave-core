/* Copyright (c) 2026 The FlyWeb Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_BROWSER_SYNC_ACCOUNT_FLYWEB_SYNC_ACCOUNT_SERVICE_FACTORY_H_
#define BRAVE_BROWSER_SYNC_ACCOUNT_FLYWEB_SYNC_ACCOUNT_SERVICE_FACTORY_H_

#include <memory>

#include "components/keyed_service/content/browser_context_keyed_service_factory.h"

namespace base {
template <typename T>
class NoDestructor;
}  // namespace base

namespace flyweb_sync {

class AccountService;

// The sync account client (FlyWeb 3.2). Regular profiles only: incognito and
// guest have no sync.
class AccountServiceFactory : public BrowserContextKeyedServiceFactory {
 public:
  AccountServiceFactory(const AccountServiceFactory&) = delete;
  AccountServiceFactory& operator=(const AccountServiceFactory&) = delete;

  static AccountService* GetForBrowserContext(
      content::BrowserContext* context);
  static AccountServiceFactory* GetInstance();

 private:
  friend base::NoDestructor<AccountServiceFactory>;

  AccountServiceFactory();
  ~AccountServiceFactory() override;

  // BrowserContextKeyedServiceFactory:
  std::unique_ptr<KeyedService> BuildServiceInstanceForBrowserContext(
      content::BrowserContext* context) const override;
  content::BrowserContext* GetBrowserContextToUse(
      content::BrowserContext* context) const override;
  bool ServiceIsCreatedWithBrowserContext() const override;
};

}  // namespace flyweb_sync

#endif  // BRAVE_BROWSER_SYNC_ACCOUNT_FLYWEB_SYNC_ACCOUNT_SERVICE_FACTORY_H_

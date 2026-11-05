/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_CHROMIUM_SRC_COMPONENTS_PERMISSIONS_PERMISSION_CONTEXT_BASE_H_
#define BRAVE_CHROMIUM_SRC_COMPONENTS_PERMISSIONS_PERMISSION_CONTEXT_BASE_H_

#include "base/functional/callback.h"

namespace permissions {
class PermissionContextBase;
using PermissionContextBase_BraveImpl = PermissionContextBase;
class PermissionLifetimeManager;
}  // namespace permissions

#define PermissionContextBase PermissionContextBase_ChromiumImpl
#define PermissionDecided virtual PermissionDecided
#define BRAVE_PERMISSION_CONTEXT_BASE_H_ friend PermissionContextBase_BraveImpl;

#include "src/components/permissions/permission_context_base.h"  // IWYU pragma: export

#undef BRAVE_PERMISSION_CONTEXT_BASE_H_
#undef PermissionDecided
#undef PermissionContextBase

namespace permissions {

class PermissionContextBase : public PermissionContextBase_ChromiumImpl {
 public:
  PermissionContextBase(
      content::BrowserContext* browser_context,
      ContentSettingsType content_settings_type,
      network::mojom::PermissionsPolicyFeature permissions_policy_feature);

  ~PermissionContextBase() override;

  void SetPermissionLifetimeManagerFactory(
      const base::RepeatingCallback<
          PermissionLifetimeManager*(content::BrowserContext*)>& factory);

 private:
  void PermissionDecided(const permissions::PermissionPromptDecision& decision,
                         const PermissionRequestData& request_data) override;

  base::RepeatingCallback<PermissionLifetimeManager*(content::BrowserContext*)>
      permission_lifetime_manager_factory_;
};

}  // namespace permissions

#endif  // BRAVE_CHROMIUM_SRC_COMPONENTS_PERMISSIONS_PERMISSION_CONTEXT_BASE_H_

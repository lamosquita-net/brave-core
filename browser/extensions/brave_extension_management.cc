/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/browser/extensions/brave_extension_management.h"

#include <memory>

#include "brave/browser/brave_browser_process.h"
#include "brave/browser/extensions/brave_extension_provider.h"
#include "brave/components/constants/pref_names.h"
#include "chrome/browser/browser_process.h"
#include "chrome/browser/extensions/extension_management_internal.h"
#include "chrome/browser/extensions/external_policy_loader.h"
#include "chrome/browser/profiles/profile.h"
#include "components/prefs/pref_service.h"
#include "extensions/browser/extension_registry.h"
#include "extensions/common/constants.h"
#include "extensions/common/extension.h"
#include "extensions/common/extension_urls.h"

namespace extensions {

BraveExtensionManagement::BraveExtensionManagement(Profile* profile)
    : ExtensionManagement(profile) {
  extension_registry_observer_.Observe(
      ExtensionRegistry::Get(static_cast<content::BrowserContext*>(profile)));
  providers_.push_back(std::make_unique<BraveExtensionProvider>());
}

BraveExtensionManagement::~BraveExtensionManagement() = default;

}  // namespace extensions

/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "chrome/browser/component_updater/registration.h"

#define RegisterComponentsForUpdate RegisterComponentsForUpdate_ChromiumImpl
#include "src/chrome/browser/component_updater/registration.cc"
#undef RegisterComponentsForUpdate

namespace component_updater {

void RegisterComponentsForUpdate() {
  RegisterComponentsForUpdate_ChromiumImpl();
  // FlyWeb: no "Brave Wallet data files" component (token lists for the
  // wallet, which FlyWeb disables): it would be downloaded for nothing.
  // FlyWeb: no HTTPS Everywhere component either. FlyWeb enables HTTPS by
  // Default (net::features::kBraveHttpsByDefault), which uses Chromium's HTTPS
  // upgrades plus the exceptions list of the Local Data component, and skips
  // the HTTPS Everywhere rules altogether (BraveRequestHandler::SetupCallbacks).
}

}  // namespace component_updater

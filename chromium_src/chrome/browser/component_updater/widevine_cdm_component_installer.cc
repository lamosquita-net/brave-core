/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <optional>

#include "chrome/browser/component_updater/widevine_cdm_component_installer.h"

#define RegisterWidevineCdmComponent RegisterWidevineCdmComponent_ChromiumImpl
#include "src/chrome/browser/component_updater/widevine_cdm_component_installer.cc"
#undef RegisterWidevineCdmComponent

#include "brave/browser/widevine/widevine_utils.h"
#include "components/component_updater/component_updater_service.h"

namespace component_updater {

namespace {

// Upstream allows plaintext because the CRX payload is signature verified.
// Require TLS anyway, so the request doesn't announce on the wire that this
// user is installing Widevine (as Brave 1.98 does).
class BraveWidevineCdmComponentInstallerPolicy final
    : public WidevineCdmComponentInstallerPolicy {
 public:
  BraveWidevineCdmComponentInstallerPolicy() = default;
  BraveWidevineCdmComponentInstallerPolicy(
      const BraveWidevineCdmComponentInstallerPolicy&) = delete;
  BraveWidevineCdmComponentInstallerPolicy& operator=(
      const BraveWidevineCdmComponentInstallerPolicy&) = delete;
  ~BraveWidevineCdmComponentInstallerPolicy() override = default;

 private:
  // ComponentInstallerPolicy:
  bool RequiresNetworkEncryption() const override { return true; }
};

}  // namespace

void RegisterWidevineCdmComponent(ComponentUpdateService* cus,
                                  base::OnceClosure callback) {
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);
  if (!IsWidevineEnabled()) {
    return;
  }
  auto installer = base::MakeRefCounted<component_updater::ComponentInstaller>(
      std::make_unique<BraveWidevineCdmComponentInstallerPolicy>());
  installer->Register(cus, std::move(callback));
}

}  // namespace component_updater

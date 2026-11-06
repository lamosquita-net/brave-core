/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_BROWSER_BRAVE_BROWSER_PROCESS_IMPL_H_
#define BRAVE_BROWSER_BRAVE_BROWSER_PROCESS_IMPL_H_

#include <memory>

#include "base/memory/ref_counted.h"
#include "brave/browser/brave_browser_process.h"
#include "brave/components/brave_component_updater/browser/brave_component.h"
#include "brave/components/url_sanitizer/browser/url_sanitizer_component_installer.h"
#include "build/build_config.h"
#include "chrome/browser/browser_process_impl.h"
#include "extensions/buildflags/buildflags.h"

namespace brave {
class BraveFarblingService;
}  // namespace brave

namespace brave_component_updater {
class LocalDataFilesService;
}  // namespace brave_component_updater

namespace brave_shields {
class AdBlockService;
}  // namespace brave_shields

namespace https_upgrade_exceptions {
class HttpsUpgradeExceptionsService;
}  // namespace https_upgrade_exceptions

namespace localhost_permission {
class LocalhostPermissionComponent;
}  // namespace localhost_permission

namespace debounce {
class DebounceComponentInstaller;
}  // namespace debounce

namespace request_otr {
#if BUILDFLAG(ENABLE_REQUEST_OTR)
class RequestOTRComponentInstallerPolicy;
#endif
}  // namespace request_otr

class BraveBrowserProcessImpl : public BraveBrowserProcess,
                                public BrowserProcessImpl {
 public:
  explicit BraveBrowserProcessImpl(StartupData* startup_data);
  BraveBrowserProcessImpl(const BraveBrowserProcessImpl&) = delete;
  BraveBrowserProcessImpl& operator=(const BraveBrowserProcessImpl&) = delete;
  ~BraveBrowserProcessImpl() override;

  // BrowserProcess implementation.

  ProfileManager* profile_manager() override;
  NotificationPlatformBridge* notification_platform_bridge() override;

  // BraveBrowserProcess implementation.

  void StartBraveServices() override;
  brave_shields::AdBlockService* ad_block_service() override;
  https_upgrade_exceptions::HttpsUpgradeExceptionsService*
  https_upgrade_exceptions_service() override;
  localhost_permission::LocalhostPermissionComponent*
  localhost_permission_component() override;
  debounce::DebounceComponentInstaller* debounce_component_installer() override;
#if BUILDFLAG(ENABLE_REQUEST_OTR)
  request_otr::RequestOTRComponentInstallerPolicy*
  request_otr_component_installer() override;
#endif
  brave::URLSanitizerComponentInstaller* URLSanitizerComponentInstaller()
      override;
  brave_component_updater::LocalDataFilesService* local_data_files_service()
      override;
  brave::BraveFarblingService* brave_farbling_service() override;

 private:
  // BrowserProcessImpl overrides:
  void Init() override;
#if !BUILDFLAG(IS_ANDROID)
  void StartTearDown() override;
  void PostDestroyThreads() override;
#endif

  void CreateProfileManager();

  void UpdateBraveDarkMode();
  void OnBraveDarkModeChanged();

  void InitBraveStatsHelper();

  brave_component_updater::BraveComponent::Delegate*
  brave_component_updater_delegate();

  // Sequence checker must stay on top to avoid UaF issues when data members use
  // `g_browser_process->profile_manager()`.
  SEQUENCE_CHECKER(sequence_checker_);

  // local_data_files_service_ should always be first because it needs
  // to be destroyed last
  std::unique_ptr<brave_component_updater::LocalDataFilesService>
      local_data_files_service_;
  std::unique_ptr<brave_component_updater::BraveComponent::Delegate>
      brave_component_updater_delegate_;
  std::unique_ptr<brave_shields::AdBlockService> ad_block_service_;
  std::unique_ptr<https_upgrade_exceptions::HttpsUpgradeExceptionsService>
      https_upgrade_exceptions_service_;
  std::unique_ptr<localhost_permission::LocalhostPermissionComponent>
      localhost_permission_component_;
  std::unique_ptr<debounce::DebounceComponentInstaller>
      debounce_component_installer_;
#if BUILDFLAG(ENABLE_REQUEST_OTR)
  std::unique_ptr<request_otr::RequestOTRComponentInstallerPolicy>
      request_otr_component_installer_;
#endif
  std::unique_ptr<brave::URLSanitizerComponentInstaller>
      url_sanitizer_component_installer_;

  std::unique_ptr<brave::BraveFarblingService> brave_farbling_service_;
};

#endif  // BRAVE_BROWSER_BRAVE_BROWSER_PROCESS_IMPL_H_

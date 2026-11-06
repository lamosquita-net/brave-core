/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/browser/brave_browser_process_impl.h"

#include <memory>
#include <string>
#include <utility>

#include "base/functional/bind.h"
#include "base/path_service.h"
#include "base/task/single_thread_task_runner.h"
#include "base/task/thread_pool.h"
#include "brave/browser/brave_shields/ad_block_subscription_download_manager_getter.h"
#include "brave/browser/component_updater/brave_component_updater_configurator.h"
#include "brave/browser/net/brave_system_request_handler.h"
#include "brave/browser/profiles/brave_profile_manager.h"
#include "brave/browser/themes/brave_dark_mode_utils.h"
#include "brave/common/brave_channel_info.h"
#include "brave/components/brave_component_updater/browser/brave_component_updater_delegate.h"
#include "brave/components/brave_component_updater/browser/local_data_files_service.h"
#include "brave/components/brave_shields/content/browser/ad_block_service.h"
#include "brave/components/brave_shields/content/browser/ad_block_subscription_service_manager.h"
#include "brave/components/brave_shields/content/browser/brave_farbling_service.h"
#include "brave/components/brave_shields/core/common/features.h"
#include "brave/components/brave_sync/network_time_helper.h"
#include "brave/components/constants/pref_names.h"
#include "brave/components/debounce/core/browser/debounce_component_installer.h"
#include "brave/components/debounce/core/common/features.h"
#include "brave/components/https_upgrade_exceptions/browser/https_upgrade_exceptions_service.h"
#include "brave/components/localhost_permission/localhost_permission_component.h"
#include "brave/components/webcompat/features.h"
#include "brave/components/webcompat/webcompat_exceptions_service.h"
#include "brave/services/network/public/cpp/system_request_handler.h"
#include "build/build_config.h"
#include "chrome/browser/component_updater/component_updater_utils.h"
#include "chrome/browser/net/system_network_context_manager.h"
#include "chrome/browser/obsolete_system/obsolete_system.h"
#include "chrome/browser/ui/browser_window/public/browser_window_interface.h"
#include "chrome/common/buildflags.h"
#include "chrome/common/channel_info.h"
#include "chrome/common/chrome_paths.h"
#include "chrome/common/pref_names.h"
#include "components/component_updater/timer_update_scheduler.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/child_process_security_policy.h"
#include "net/base/features.h"
#include "services/network/public/cpp/resource_request.h"
#include "services/network/public/cpp/shared_url_loader_factory.h"
#include "url/gurl.h"

#if BUILDFLAG(IS_ANDROID)
#include "chrome/browser/flags/android/chrome_feature_list.h"
#else
#include "brave/browser/ui/brave_browser_command_controller.h"
#include "chrome/browser/ui/browser.h"
#endif

#if BUILDFLAG(ENABLE_REQUEST_OTR)
#include "brave/components/request_otr/browser/request_otr_component_installer.h"
#include "brave/components/request_otr/common/features.h"
#endif

using brave_component_updater::BraveComponent;

namespace {

// Initializes callback for SystemRequestHandler
void InitSystemRequestHandlerCallback() {
  network::SystemRequestHandler::OnBeforeSystemRequestCallback
      before_system_request_callback =
          base::BindRepeating(brave::OnBeforeSystemRequest);
  network::SystemRequestHandler::GetInstance()
      ->RegisterOnBeforeSystemRequestCallback(before_system_request_callback);
}

}  // namespace

using content::BrowserThread;

BraveBrowserProcessImpl::~BraveBrowserProcessImpl() = default;

BraveBrowserProcessImpl::BraveBrowserProcessImpl(StartupData* startup_data)
    : BrowserProcessImpl(startup_data) {
  g_browser_process = this;
  g_brave_browser_process = this;
}

void BraveBrowserProcessImpl::Init() {
  BrowserProcessImpl::Init();
  UpdateBraveDarkMode();
  pref_change_registrar_.Add(
      kBraveDarkMode,
      base::BindRepeating(&BraveBrowserProcessImpl::OnBraveDarkModeChanged,
                          base::Unretained(this)));

  InitSystemRequestHandlerCallback();

#if !BUILDFLAG(IS_ANDROID)
  if (!ObsoleteSystem::IsObsoleteNowOrSoon()) {
    // Clear to show unsupported warning infobar again even if user
    // suppressed it from previous os.
    local_state()->ClearPref(prefs::kSuppressUnsupportedOSWarning);
  }
#endif
}

#if !BUILDFLAG(IS_ANDROID)
void BraveBrowserProcessImpl::StartTearDown() {
  BrowserProcessImpl::StartTearDown();
}

void BraveBrowserProcessImpl::PostDestroyThreads() {
  BrowserProcessImpl::PostDestroyThreads();
  // AdBlockService should outlive its own worker thread.
  ad_block_service_.reset();
}
#endif

brave_component_updater::BraveComponent::Delegate*
BraveBrowserProcessImpl::brave_component_updater_delegate() {
  if (!brave_component_updater_delegate_) {
    brave_component_updater_delegate_ =
        std::make_unique<brave::BraveComponentUpdaterDelegate>(
            component_updater(), local_state(), GetApplicationLocale());
  }
  return brave_component_updater_delegate_.get();
}

ProfileManager* BraveBrowserProcessImpl::profile_manager() {
  DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
  if (!created_profile_manager_) {
    CreateProfileManager();
  }
  return profile_manager_.get();
}

void BraveBrowserProcessImpl::StartBraveServices() {
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);

  if (base::FeatureList::IsEnabled(net::features::kBraveHttpsByDefault)) {
    https_upgrade_exceptions_service();
  }

  if (base::FeatureList::IsEnabled(
          brave_shields::features::kBraveLocalhostAccessPermission)) {
    localhost_permission_component();
  }

  if (base::FeatureList::IsEnabled(
          webcompat::features::kBraveWebcompatExceptionsService)) {
    webcompat::WebcompatExceptionsService::CreateInstance(
        local_data_files_service());
  }

  debounce_component_installer();
#if BUILDFLAG(ENABLE_REQUEST_OTR)
  request_otr_component_installer();
#endif
  URLSanitizerComponentInstaller();
  // Now start the local data files service, which calls all observers.
  local_data_files_service()->Start();

  brave_sync::NetworkTimeHelper::GetInstance()->SetNetworkTimeTracker(
      g_browser_process->network_time_tracker(),
      base::SingleThreadTaskRunner::GetCurrentDefault());
}

brave_shields::AdBlockService* BraveBrowserProcessImpl::ad_block_service() {
  if (!ad_block_service_) {
    scoped_refptr<base::SequencedTaskRunner> task_runner(
        base::ThreadPool::CreateSequencedTaskRunner(
            {base::MayBlock(), base::TaskPriority::USER_BLOCKING,
             base::TaskShutdownBehavior::SKIP_ON_SHUTDOWN}));
    ad_block_service_ = std::make_unique<brave_shields::AdBlockService>(
        local_state(), GetApplicationLocale(), component_updater(), task_runner,
        AdBlockSubscriptionDownloadManagerGetter(),
        profile_manager()->user_data_dir().Append(
            profile_manager()->GetInitialProfileDir()));
  }
  return ad_block_service_.get();
}

https_upgrade_exceptions::HttpsUpgradeExceptionsService*
BraveBrowserProcessImpl::https_upgrade_exceptions_service() {
  if (!https_upgrade_exceptions_service_) {
    https_upgrade_exceptions_service_ =
        https_upgrade_exceptions::HttpsUpgradeExceptionsServiceFactory(
            local_data_files_service());
  }
  return https_upgrade_exceptions_service_.get();
}

localhost_permission::LocalhostPermissionComponent*
BraveBrowserProcessImpl::localhost_permission_component() {
  if (!base::FeatureList::IsEnabled(
          brave_shields::features::kBraveLocalhostAccessPermission)) {
    return nullptr;
  }

  if (!localhost_permission_component_) {
    localhost_permission_component_ =
        std::make_unique<localhost_permission::LocalhostPermissionComponent>(
            local_data_files_service());
  }
  return localhost_permission_component_.get();
}

debounce::DebounceComponentInstaller*
BraveBrowserProcessImpl::debounce_component_installer() {
  if (!base::FeatureList::IsEnabled(debounce::features::kBraveDebounce)) {
    return nullptr;
  }
  if (!debounce_component_installer_) {
    debounce_component_installer_ =
        std::make_unique<debounce::DebounceComponentInstaller>(
            local_data_files_service());
  }
  return debounce_component_installer_.get();
}

#if BUILDFLAG(ENABLE_REQUEST_OTR)
request_otr::RequestOTRComponentInstallerPolicy*
BraveBrowserProcessImpl::request_otr_component_installer() {
  if (!base::FeatureList::IsEnabled(
          request_otr::features::kBraveRequestOTRTab)) {
    return nullptr;
  }
  if (!request_otr_component_installer_) {
    request_otr_component_installer_ =
        std::make_unique<request_otr::RequestOTRComponentInstallerPolicy>(
            local_data_files_service());
  }
  return request_otr_component_installer_.get();
}
#endif

brave::URLSanitizerComponentInstaller*
BraveBrowserProcessImpl::URLSanitizerComponentInstaller() {
  if (!url_sanitizer_component_installer_) {
    url_sanitizer_component_installer_ =
        std::make_unique<brave::URLSanitizerComponentInstaller>(
            local_data_files_service());
  }
  return url_sanitizer_component_installer_.get();
}

brave_component_updater::LocalDataFilesService*
BraveBrowserProcessImpl::local_data_files_service() {
  if (!local_data_files_service_) {
    local_data_files_service_ =
        brave_component_updater::LocalDataFilesServiceFactory(
            brave_component_updater_delegate());
  }
  return local_data_files_service_.get();
}

void BraveBrowserProcessImpl::UpdateBraveDarkMode() {
  // Update with proper system theme to make brave theme and base ui components
  // theme use same theme.
  dark_mode::SetSystemDarkMode(dark_mode::GetBraveDarkModeType());
}

void BraveBrowserProcessImpl::OnBraveDarkModeChanged() {
  UpdateBraveDarkMode();
}

void BraveBrowserProcessImpl::CreateProfileManager() {
  DCHECK(!created_profile_manager_ && !profile_manager_);
  created_profile_manager_ = true;

  base::FilePath user_data_dir;
  base::PathService::Get(chrome::DIR_USER_DATA, &user_data_dir);
  profile_manager_ = std::make_unique<BraveProfileManager>(user_data_dir);
}

NotificationPlatformBridge*
BraveBrowserProcessImpl::notification_platform_bridge() {
  return BrowserProcessImpl::notification_platform_bridge();
}

brave::BraveFarblingService* BraveBrowserProcessImpl::brave_farbling_service() {
  if (!brave_farbling_service_) {
    brave_farbling_service_ = std::make_unique<brave::BraveFarblingService>();
  }
  return brave_farbling_service_.get();
}

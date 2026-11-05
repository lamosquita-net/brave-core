/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

// This interface is for managing the global services of the application. Each
// service is lazily created when requested the first time. The service getters
// will return NULL if the service is not available, so callers must check for
// this condition.

#ifndef BRAVE_BROWSER_BRAVE_BROWSER_PROCESS_H_
#define BRAVE_BROWSER_BRAVE_BROWSER_PROCESS_H_

#include "brave/components/request_otr/common/buildflags/buildflags.h"
#include "brave/components/speedreader/common/buildflags/buildflags.h"
#include "build/build_config.h"
#include "extensions/buildflags/buildflags.h"

namespace brave {
class BraveFarblingService;
class URLSanitizerComponentInstaller;
}  // namespace brave

namespace brave_component_updater {
class LocalDataFilesService;
}  // namespace brave_component_updater

namespace brave_shields {
class AdBlockService;
}  // namespace brave_shields

namespace debounce {
class DebounceComponentInstaller;
}  // namespace debounce

namespace https_upgrade_exceptions {
class HttpsUpgradeExceptionsService;
}  // namespace https_upgrade_exceptions

namespace localhost_permission {
class LocalhostPermissionComponent;
}  // namespace localhost_permission

namespace request_otr {
#if BUILDFLAG(ENABLE_REQUEST_OTR)
class RequestOTRComponentInstallerPolicy;
#endif
}  // namespace request_otr

namespace speedreader {
class SpeedreaderRewriterService;
}

class BraveBrowserProcess {
 public:
  BraveBrowserProcess();
  virtual ~BraveBrowserProcess();
  virtual void StartBraveServices() = 0;
  virtual brave_shields::AdBlockService* ad_block_service() = 0;
  virtual https_upgrade_exceptions::HttpsUpgradeExceptionsService*
  https_upgrade_exceptions_service() = 0;
  virtual localhost_permission::LocalhostPermissionComponent*
  localhost_permission_component() = 0;
  virtual debounce::DebounceComponentInstaller*
  debounce_component_installer() = 0;
#if BUILDFLAG(ENABLE_REQUEST_OTR)
  virtual request_otr::RequestOTRComponentInstallerPolicy*
  request_otr_component_installer() = 0;
#endif
  virtual brave::URLSanitizerComponentInstaller*
  URLSanitizerComponentInstaller() = 0;
  virtual brave_component_updater::LocalDataFilesService*
  local_data_files_service() = 0;
#if BUILDFLAG(ENABLE_SPEEDREADER)
  virtual speedreader::SpeedreaderRewriterService*
  speedreader_rewriter_service() = 0;
#endif
  virtual brave::BraveFarblingService* brave_farbling_service() = 0;
};

extern BraveBrowserProcess* g_brave_browser_process;

#endif  // BRAVE_BROWSER_BRAVE_BROWSER_PROCESS_H_

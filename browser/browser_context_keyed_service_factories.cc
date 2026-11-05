/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/browser/browser_context_keyed_service_factories.h"

#include "base/feature_list.h"
#include "brave/browser/brave_shields/ad_block_pref_service_factory.h"
#include "brave/browser/debounce/debounce_service_factory.h"
#include "brave/browser/ephemeral_storage/ephemeral_storage_service_factory.h"
#include "brave/browser/ntp_background/view_counter_service_factory.h"
#include "brave/browser/permissions/permission_lifetime_manager_factory.h"
#include "brave/browser/profiles/brave_renderer_updater_factory.h"
#include "brave/browser/search_engines/search_engine_provider_service_factory.h"
#include "brave/browser/search_engines/search_engine_tracker.h"
#include "brave/browser/sync/brave_sync_alerts_service_factory.h"
#include "brave/browser/url_sanitizer/url_sanitizer_service_factory.h"
#include "brave/components/brave_perf_predictor/browser/named_third_party_registry_factory.h"
#include "brave/components/request_otr/common/buildflags/buildflags.h"

#if !BUILDFLAG(IS_ANDROID)
#include "brave/browser/ui/bookmark/bookmark_prefs_service_factory.h"
#include "brave/browser/ui/commands/accelerator_service_factory.h"
#include "brave/browser/ui/tabs/features.h"
#include "brave/browser/ui/tabs/shared_pinned_tab_service_factory.h"
#include "brave/components/commands/common/features.h"
#else
#include "brave/browser/brave_shields/cookie_list_opt_in_service_factory.h"
#include "brave/browser/brave_shields/filter_list_service_factory.h"
#endif

#if BUILDFLAG(ENABLE_REQUEST_OTR)
#include "brave/browser/request_otr/request_otr_service_factory.h"
#endif

namespace brave {

void EnsureBrowserContextKeyedServiceFactoriesBuilt() {
  brave_perf_predictor::NamedThirdPartyRegistryFactory::GetInstance();
  brave_shields::AdBlockPrefServiceFactory::GetInstance();
  debounce::DebounceServiceFactory::GetInstance();
  brave::URLSanitizerServiceFactory::GetInstance();
  BraveRendererUpdaterFactory::GetInstance();
  SearchEngineProviderServiceFactory::GetInstance();
  SearchEngineTrackerFactory::GetInstance();
  ntp_background_images::ViewCounterServiceFactory::GetInstance();

#if !BUILDFLAG(IS_ANDROID)
  BookmarkPrefsServiceFactory::GetInstance();
#else
  brave_shields::CookieListOptInServiceFactory::GetInstance();
  brave_shields::FilterListServiceFactory::GetInstance();
#endif

#if !BUILDFLAG(IS_ANDROID)
  if (base::FeatureList::IsEnabled(commands::features::kBraveCommands)) {
    commands::AcceleratorServiceFactory::GetInstance();
  }
#endif

  EphemeralStorageServiceFactory::GetInstance();
  PermissionLifetimeManagerFactory::GetInstance();
#if BUILDFLAG(ENABLE_REQUEST_OTR)
  request_otr::RequestOTRServiceFactory::GetInstance();
#endif

  BraveSyncAlertsServiceFactory::GetInstance();

#if !BUILDFLAG(IS_ANDROID)
  if (base::FeatureList::IsEnabled(tabs::features::kBraveSharedPinnedTabs)) {
    SharedPinnedTabServiceFactory::GetInstance();
  }
#endif
}

}  // namespace brave

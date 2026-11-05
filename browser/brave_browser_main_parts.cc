/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/browser/brave_browser_main_parts.h"

#include <memory>
#include <utility>

#include "base/command_line.h"
#include "brave/browser/browsing_data/brave_clear_browsing_data.h"
#include "brave/components/brave_component_updater/browser/brave_on_demand_updater.h"
#include "brave/components/brave_sync/features.h"
#include "brave/components/constants/brave_constants.h"
#include "brave/components/constants/pref_names.h"
#include "brave/components/translate/core/common/brave_translate_features.h"
#include "build/build_config.h"
#include "chrome/browser/browser_process.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/common/chrome_features.h"
#include "components/component_updater/component_updater_service.h"
#include "components/prefs/pref_service.h"
#include "components/sync/base/command_line_switches.h"
#include "components/translate/core/browser/translate_language_list.h"
#include "content/public/browser/render_frame_host.h"
#include "extensions/buildflags/buildflags.h"
#include "media/base/media_switches.h"

#if !BUILDFLAG(IS_ANDROID)
#include "brave/browser/infobars/brave_sync_account_deleted_infobar_delegate.h"
#include "chrome/browser/ui/browser_window/public/browser_window_interface.h"
#include "chrome/browser/ui/browser_window/public/browser_window_interface_iterator.h"
#include "chrome/browser/ui/tabs/tab_strip_model.h"
#include "components/infobars/content/content_infobar_manager.h"
#include "components/sync/service/sync_service.h"
#include "components/sync/service/sync_user_settings.h"
#include "content/public/browser/web_contents.h"
#else
#include "brave/browser/android/preferences/features.h"
#endif

BraveBrowserMainParts::BraveBrowserMainParts(bool is_integration_test,
                                             StartupData* startup_data)
    : ChromeBrowserMainParts(is_integration_test, startup_data) {}

BraveBrowserMainParts::~BraveBrowserMainParts() = default;

int BraveBrowserMainParts::PreMainMessageLoopRun() {
  brave_component_updater::BraveOnDemandUpdater::GetInstance()
      ->RegisterOnDemandUpdater(
          &g_browser_process->component_updater()->GetOnDemandUpdater());

  return ChromeBrowserMainParts::PreMainMessageLoopRun();
}

void BraveBrowserMainParts::PostBrowserStart() {
  ChromeBrowserMainParts::PostBrowserStart();

#if !BUILDFLAG(IS_ANDROID)
  BrowserWindowInterface* browser =
      GetLastActiveBrowserWindowInterfaceWithAnyProfile();
  content::WebContents* active_web_contents = nullptr;

  if (browser) {
    active_web_contents = browser->GetTabStripModel()->GetActiveWebContents();

    if (active_web_contents) {
      Profile* profile = browser->GetProfile();
      infobars::ContentInfoBarManager* infobar_manager =
          infobars::ContentInfoBarManager::FromWebContents(active_web_contents);
      if (infobar_manager) {
        // The sync seed is decrypted asynchronously now (os_crypt_async):
        // SyncCannotRunInfoBarDelegate is shown by BraveSyncAlertsService once
        // the encryptor is ready.
        BraveSyncAccountDeletedInfoBarDelegate::Create(active_web_contents,
                                                       profile);
      }
    }
  }
#endif  // !BUILDFLAG(IS_ANDROID)
}

void BraveBrowserMainParts::PreShutdown() {
  content::BraveClearBrowsingData::ClearOnExit();
}

void BraveBrowserMainParts::PreProfileInit() {
  ChromeBrowserMainParts::PreProfileInit();
#if !BUILDFLAG(IS_ANDROID)
  auto* command_line = base::CommandLine::ForCurrentProcess();
  if (!base::FeatureList::IsEnabled(brave_sync::features::kBraveSync)) {
    // Disable sync temporarily
    if (!command_line->HasSwitch(syncer::kDisableSync))
      command_line->AppendSwitch(syncer::kDisableSync);
  } else {
    // Relaunch after flag changes will still have the switch
    // when switching from disabled to enabled
    command_line->RemoveSwitch(syncer::kDisableSync);
  }
#endif

  if (!translate::ShouldUpdateLanguagesList())
    translate::TranslateLanguageList::DisableUpdate();
}

void BraveBrowserMainParts::PostProfileInit(Profile* profile,
                                            bool is_initial_profile) {
  ChromeBrowserMainParts::PostProfileInit(profile, is_initial_profile);

#if BUILDFLAG(IS_ANDROID)
  if (base::FeatureList::IsEnabled(
          preferences::features::kBraveBackgroundVideoPlayback) ||
      profile->GetPrefs()->GetBoolean(kBackgroundVideoPlaybackEnabled)) {
    content::RenderFrameHost::AllowInjectingJavaScript();
    auto* command_line = base::CommandLine::ForCurrentProcess();
    command_line->AppendSwitch(switches::kDisableBackgroundMediaSuspend);
  }
#endif
}

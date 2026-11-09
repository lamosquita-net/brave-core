/* Copyright (c) 2024 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "chrome/browser/ui/webui/chrome_web_ui_configs.h"

#include "content/public/browser/webui_config_map.h"

#define RegisterChromeWebUIConfigs RegisterChromeWebUIConfigs_ChromiumImpl

#include "src/chrome/browser/ui/webui/chrome_web_ui_configs.cc"
#undef RegisterChromeWebUIConfigs

#if !BUILDFLAG(IS_ANDROID)
#include "base/strings/strcat.h"
#include "brave/browser/ui/webui/brave_settings_ui.h"
#include "brave/browser/ui/webui/brave_shields/shields_panel_ui.h"
#include "brave/browser/ui/webui/private_new_tab_page/brave_private_new_tab_ui.h"
#include "chrome/common/webui_url_constants.h"
#include "content/public/common/url_constants.h"
#include "url/gurl.h"
#include "url/url_constants.h"
#endif  // !BUILDFLAG(IS_ANDROID)

void RegisterChromeWebUIConfigs() {
  RegisterChromeWebUIConfigs_ChromiumImpl();

#if !BUILDFLAG(IS_ANDROID)
  auto& map = content::WebUIConfigMap::GetInstance();
  // FlyWeb: Chromium 156 registers settings::SettingsUIConfig, and the config
  // map wins over BraveWebUIControllerFactory, so chrome://settings ended up
  // as upstream's SettingsUI without the Brave handlers (sync page dead in
  // 3.0.0). Swap it for BraveSettingsUIConfig, as Brave 1.98 does.
  map.RemoveConfig(GURL(base::StrCat({content::kChromeUIScheme,
                                      url::kStandardSchemeSeparator,
                                      chrome::kChromeUISettingsHost})));
  // Same for Chromium's NewTabUIConfig (incognito/guest New Tab page):
  // private windows got Chromium's page instead of BravePrivateNewTabUI.
  map.RemoveConfig(GURL(base::StrCat({content::kChromeUIScheme,
                                      url::kStandardSchemeSeparator,
                                      chrome::kChromeUINewTabHost})));
  map.AddWebUIConfig(std::make_unique<BraveSettingsUIConfig>());
  map.AddWebUIConfig(std::make_unique<BravePrivateNewTabUIConfig>());
  map.AddWebUIConfig(std::make_unique<ShieldsPanelUIConfig>());
#endif  // !BUILDFLAG(IS_ANDROID)
}

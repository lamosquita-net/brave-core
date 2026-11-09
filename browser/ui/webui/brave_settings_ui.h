/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_BROWSER_UI_WEBUI_BRAVE_SETTINGS_UI_H_
#define BRAVE_BROWSER_UI_WEBUI_BRAVE_SETTINGS_UI_H_

#include <memory>
#include <string>

#include "brave/components/commands/common/commands.mojom.h"
#include "chrome/browser/ui/webui/settings/settings_ui.h"
#include "chrome/common/webui_url_constants.h"
#include "content/public/browser/webui_config.h"
#include "content/public/common/url_constants.h"

class BraveSettingsUI;

namespace content {
class WebUIDataSource;
}

class Profile;

// FlyWeb: since Chromium 156 the settings WebUI is created from the
// WebUIConfigMap, which is asked before BraveWebUIControllerFactory. Without
// this config chrome://settings got upstream's SettingsUI and none of the
// Brave handlers (sync, shields, appearance...) were attached. As in
// Brave 1.98, chrome_web_ui_configs.cc swaps SettingsUIConfig for this one.
class BraveSettingsUIConfig
    : public content::DefaultWebUIConfig<BraveSettingsUI> {
 public:
  BraveSettingsUIConfig()
      : DefaultWebUIConfig(content::kChromeUIScheme,
                           chrome::kChromeUISettingsHost) {}
};

class BraveSettingsUI : public settings::SettingsUI {
 public:
  explicit BraveSettingsUI(content::WebUI* web_ui);
  BraveSettingsUI(const BraveSettingsUI&) = delete;
  BraveSettingsUI& operator=(const BraveSettingsUI&) = delete;
  ~BraveSettingsUI() override;

  static void AddResources(content::WebUIDataSource* html_source,
                           Profile* profile);
  static bool& ShouldExposeElementsForTesting();

  void BindInterface(
      mojo::PendingReceiver<commands::mojom::CommandsService> pending_receiver);
};

#endif  // BRAVE_BROWSER_UI_WEBUI_BRAVE_SETTINGS_UI_H_

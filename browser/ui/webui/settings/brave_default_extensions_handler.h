/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_BROWSER_UI_WEBUI_SETTINGS_BRAVE_DEFAULT_EXTENSIONS_HANDLER_H_
#define BRAVE_BROWSER_UI_WEBUI_SETTINGS_BRAVE_DEFAULT_EXTENSIONS_HANDLER_H_

#include "base/values.h"
#include "chrome/browser/ui/webui/settings/settings_page_ui_handler.h"
#include "components/prefs/pref_change_registrar.h"
#include "third_party/widevine/cdm/buildflags.h"

class BraveDefaultExtensionsHandler : public settings::SettingsPageUIHandler
{
 public:
  BraveDefaultExtensionsHandler();
  BraveDefaultExtensionsHandler(const BraveDefaultExtensionsHandler&) = delete;
  BraveDefaultExtensionsHandler& operator=(
      const BraveDefaultExtensionsHandler&) = delete;
  ~BraveDefaultExtensionsHandler() override;

 private:
  // SettingsPageUIHandler overrides:
  void RegisterMessages() override;
  void OnJavascriptAllowed() override {}
  void OnJavascriptDisallowed() override {}

  void GetRestartNeeded(const base::ListValue& args);
  void SetWidevineEnabled(const base::ListValue& args);
  void IsWidevineEnabled(const base::ListValue& args);
  void OnWidevineEnabledChanged();

  void InitializePrefCallbacks();

  void OnRestartNeededChanged();
  bool IsRestartNeeded();

#if BUILDFLAG(ENABLE_WIDEVINE)
  PrefChangeRegistrar local_state_change_registrar_;
  bool was_widevine_enabled_ = false;
#endif
};

#endif  // BRAVE_BROWSER_UI_WEBUI_SETTINGS_BRAVE_DEFAULT_EXTENSIONS_HANDLER_H_

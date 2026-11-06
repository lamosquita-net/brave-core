/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/browser/ui/webui/settings/brave_default_extensions_handler.h"

#include <memory>
#include <string>

#include "base/functional/bind.h"
#include "base/strings/utf_string_conversions.h"
#include "base/values.h"
#include "brave/browser/extensions/brave_component_loader.h"
#include "brave/components/constants/pref_names.h"
#include "brave/components/l10n/common/localization_util.h"
#include "chrome/browser/about_flags.h"
#include "chrome/browser/browser_process.h"
#include "chrome/browser/extensions/component_loader.h"
#include "chrome/browser/extensions/extension_service.h"
#include "chrome/browser/extensions/webstore_install_with_prompt.h"
#include "chrome/browser/lifetime/application_lifetime.h"
#include "chrome/browser/media/router/media_router_feature.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/browser/ui/browser.h"
#include "chrome/browser/ui/browser_window.h"
#include "chrome/browser/ui/browser_window/public/browser_window_interface.h"
#include "chrome/browser/ui/chrome_select_file_policy.h"
#include "chrome/common/chrome_switches.h"
#include "chrome/common/pref_names.h"
#include "components/flags_ui/flags_ui_constants.h"
#include "components/flags_ui/pref_service_flags_storage.h"
#include "components/grit/brave_components_strings.h"
#include "components/prefs/pref_service.h"
#include "content/public/browser/web_ui.h"
#include "extensions/browser/extension_registry.h"
#include "extensions/browser/extension_system.h"
#include "extensions/common/constants.h"
#include "extensions/common/feature_switch.h"
#include "ui/shell_dialogs/selected_file_info.h"

#if BUILDFLAG(ENABLE_WIDEVINE)
#include "brave/browser/widevine/widevine_utils.h"
#endif

BraveDefaultExtensionsHandler::BraveDefaultExtensionsHandler()
    : weak_ptr_factory_(this) {
#if BUILDFLAG(ENABLE_WIDEVINE)
  was_widevine_enabled_ = ::IsWidevineEnabled();
#endif
}

BraveDefaultExtensionsHandler::~BraveDefaultExtensionsHandler() = default;

void BraveDefaultExtensionsHandler::RegisterMessages() {
  profile_ = Profile::FromWebUI(web_ui());
  // TODO(petemill): If anything outside this handler is responsible for causing
  // restart-neccessary actions, then this should be moved to a generic handler
  // and the flag should be moved to somewhere more static / singleton-like.
  web_ui()->RegisterMessageCallback(
      "getRestartNeeded",
      base::BindRepeating(&BraveDefaultExtensionsHandler::GetRestartNeeded,
                          base::Unretained(this)));
  web_ui()->RegisterMessageCallback(
      "setWidevineEnabled",
      base::BindRepeating(&BraveDefaultExtensionsHandler::SetWidevineEnabled,
                          base::Unretained(this)));
  web_ui()->RegisterMessageCallback(
      "isWidevineEnabled",
      base::BindRepeating(&BraveDefaultExtensionsHandler::IsWidevineEnabled,
                          base::Unretained(this)));

  // Can't call this in ctor because it needs to access web_ui().
  InitializePrefCallbacks();
}

void BraveDefaultExtensionsHandler::InitializePrefCallbacks() {
#if BUILDFLAG(ENABLE_WIDEVINE)
  local_state_change_registrar_.Init(g_browser_process->local_state());
  local_state_change_registrar_.Add(
      kWidevineEnabled,
      base::BindRepeating(
          &BraveDefaultExtensionsHandler::OnWidevineEnabledChanged,
          base::Unretained(this)));
#endif
}

bool BraveDefaultExtensionsHandler::IsRestartNeeded() {
#if BUILDFLAG(ENABLE_WIDEVINE)
  if (was_widevine_enabled_ != ::IsWidevineEnabled()) {
    return true;
  }
#endif

  return false;
}

void BraveDefaultExtensionsHandler::GetRestartNeeded(
    const base::ListValue& args) {
  CHECK_EQ(args.size(), 1U);

  AllowJavascript();
  ResolveJavascriptCallback(args[0], base::Value(IsRestartNeeded()));
}

bool BraveDefaultExtensionsHandler::IsExtensionInstalled(
    const std::string& extension_id) const {
  extensions::ExtensionRegistry* registry = extensions::ExtensionRegistry::Get(
      static_cast<content::BrowserContext*>(profile_));
  return registry && registry->GetInstalledExtension(extension_id);
}

void BraveDefaultExtensionsHandler::OnInstallResult(
    const std::string& pref_name,
    bool success,
    const std::string& error,
    extensions::webstore_install::Result result) {
  if (result != extensions::webstore_install::Result::SUCCESS &&
      result != extensions::webstore_install::Result::LAUNCH_IN_PROGRESS) {
    profile_->GetPrefs()->SetBoolean(pref_name, false);
  }
}

void BraveDefaultExtensionsHandler::OnRestartNeededChanged() {
  if (IsJavascriptAllowed()) {
    FireWebUIListener("brave-needs-restart-changed",
                      base::Value(IsRestartNeeded()));
  }
}

void BraveDefaultExtensionsHandler::SetWidevineEnabled(
    const base::ListValue& args) {
#if BUILDFLAG(ENABLE_WIDEVINE)
  CHECK_EQ(args.size(), 1U);
  bool enabled = args[0].GetBool();
  enabled ? EnableWidevineCdm() : DisableWidevineCdm();
  AllowJavascript();
#endif
}

void BraveDefaultExtensionsHandler::IsWidevineEnabled(
    const base::ListValue& args) {
  CHECK_EQ(args.size(), 1U);
  AllowJavascript();
  ResolveJavascriptCallback(args[0],
#if BUILDFLAG(ENABLE_WIDEVINE)
                            base::Value(::IsWidevineEnabled()));
#else
                            base::Value(false));
#endif
}

void BraveDefaultExtensionsHandler::OnWidevineEnabledChanged() {
  if (IsJavascriptAllowed()) {
    FireWebUIListener("widevine-enabled-changed",
#if BUILDFLAG(ENABLE_WIDEVINE)
                      base::Value(::IsWidevineEnabled()));
#else
                      base::Value(false));
#endif
    OnRestartNeededChanged();
  }
}

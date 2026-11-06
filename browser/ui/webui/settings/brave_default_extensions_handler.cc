/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/browser/ui/webui/settings/brave_default_extensions_handler.h"

#include "base/check_op.h"
#include "base/functional/bind.h"
#include "base/values.h"
#include "chrome/browser/browser_process.h"
#include "content/public/browser/web_ui.h"

#if BUILDFLAG(ENABLE_WIDEVINE)
#include "brave/browser/widevine/widevine_utils.h"
#include "brave/components/constants/pref_names.h"
#endif

BraveDefaultExtensionsHandler::BraveDefaultExtensionsHandler() {
#if BUILDFLAG(ENABLE_WIDEVINE)
  was_widevine_enabled_ = ::IsWidevineEnabled();
#endif
}

BraveDefaultExtensionsHandler::~BraveDefaultExtensionsHandler() = default;

void BraveDefaultExtensionsHandler::RegisterMessages() {
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

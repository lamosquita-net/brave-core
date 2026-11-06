// Copyright (c) 2021 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// you can obtain one at http://mozilla.org/MPL/2.0/.

#include "brave/browser/ui/webui/brave_shields/shields_panel_handler.h"

#include <utility>

#include "brave/browser/ui/brave_browser_window.h"
#include "brave/components/constants/pref_names.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/browser/ui/webui/top_chrome/top_chrome_web_ui_controller.h"
#include "chrome/browser/ui/webui/webui_embedding_context.h"
#include "components/prefs/pref_service.h"
#include "content/public/browser/web_ui.h"
#include "ui/gfx/geometry/vector2d.h"

ShieldsPanelHandler::ShieldsPanelHandler(
    mojo::PendingReceiver<brave_shields::mojom::PanelHandler> receiver,
    TopChromeWebUIController* webui_controller,
    Profile* profile)
    : receiver_(this, std::move(receiver)),
      webui_controller_(webui_controller),
      profile_(profile) {}

ShieldsPanelHandler::~ShieldsPanelHandler() = default;

void ShieldsPanelHandler::ShowUI() {
  auto embedder = webui_controller_->embedder();
  if (embedder) {
    embedder->ShowUI();
  }
}

void ShieldsPanelHandler::CloseUI() {
  auto embedder = webui_controller_->embedder();
  if (embedder) {
    embedder->CloseUI();
  }
}

void ShieldsPanelHandler::GetPosition(GetPositionCallback callback) {
  auto* browser = webui::GetBrowserWindowInterface(
      webui_controller_->web_ui()->GetWebContents());
  auto* brave_browser_window =
      browser ? BraveBrowserWindow::FromBrowser(browser) : nullptr;
  if (!brave_browser_window) {
    std::move(callback).Run(gfx::Vector2d());
    return;
  }
  const gfx::Rect rect = brave_browser_window->GetShieldsBubbleRect();
  std::move(callback).Run(gfx::Vector2d(rect.x(), rect.y()));
}

void ShieldsPanelHandler::SetAdvancedViewEnabled(bool is_enabled) {
  DCHECK(profile_);

  profile_->GetPrefs()->SetBoolean(kShieldsAdvancedViewEnabled, is_enabled);
}

void ShieldsPanelHandler::GetAdvancedViewEnabled(
    GetAdvancedViewEnabledCallback callback) {
  DCHECK(profile_);

  bool is_enabled =
      profile_->GetPrefs()->GetBoolean(kShieldsAdvancedViewEnabled);
  std::move(callback).Run(is_enabled);
}

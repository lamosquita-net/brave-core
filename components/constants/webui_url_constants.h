/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_COMPONENTS_CONSTANTS_WEBUI_URL_CONSTANTS_H_
#define BRAVE_COMPONENTS_CONSTANTS_WEBUI_URL_CONSTANTS_H_

#include "build/build_config.h"

inline constexpr char kAdblockHost[] = "adblock";
inline constexpr char kAdblockInternalsHost[] = "adblock-internals";
inline constexpr char kAdblockJS[] = "brave_adblock.js";
inline constexpr char kSkusInternalsHost[] = "skus-internals";
inline constexpr char kWebcompatReporterHost[] = "webcompat";
inline constexpr char kRewardsPageHost[] = "rewards";
inline constexpr char kRewardsPageURL[] = "chrome://rewards/";
inline constexpr char kRewardsPageTopHost[] = "rewards.top-chrome";
inline constexpr char kRewardsPageTopURL[] = "chrome://rewards.top-chrome";
inline constexpr char kRewardsInternalsHost[] = "rewards-internals";
inline constexpr char kWelcomeHost[] = "welcome";
inline constexpr char kWelcomeURL[] = "chrome://welcome/";
inline constexpr char kWelcomeJS[] = "brave_welcome.js";
inline constexpr char kBraveRewardsPanelURL[] =
    "chrome://rewards-panel.top-chrome";
inline constexpr char kBraveRewardsPanelHost[] = "rewards-panel.top-chrome";
inline constexpr char kBraveTipPanelURL[] = "chrome://tip-panel.top-chrome";
inline constexpr char kBraveTipPanelHost[] = "tip-panel.top-chrome";
inline constexpr char kBraveNewTabJS[] = "brave_new_tab.js";
inline constexpr char kBraveUIAdblockURL[] = "chrome://adblock/";
inline constexpr char kBraveUIWebcompatReporterURL[] = "chrome://webcompat/";
inline constexpr char kExtensionSettingsURL[] = "flyweb://settings/extensions";
inline constexpr char kBraveSyncPath[] = "braveSync";
inline constexpr char kBraveSyncSetupPath[] = "braveSync/setup";
inline constexpr char kShieldsPanelURL[] = "chrome://brave-shields.top-chrome";
inline constexpr char kShieldsPanelHost[] = "brave-shields.top-chrome";
inline constexpr char kCookieListOptInHost[] = "cookie-list-opt-in.top-chrome";
inline constexpr char kCookieListOptInURL[] =
    "chrome://cookie-list-opt-in.top-chrome";
inline constexpr char kFederatedInternalsURL[] = "flyweb://federated-internals";
inline constexpr char kFederatedInternalsHost[] = "federated-internals";
inline constexpr char kContentFiltersPath[] = "shields/filters";
inline constexpr char kShortcutsURL[] = "chrome://settings/system/shortcuts";
inline constexpr char kChatUIURL[] = "chrome-untrusted://chat/";
inline constexpr char kChatUIHost[] = "chat";

inline constexpr char kRewriterUIURL[] = "chrome://rewriter/";
inline constexpr char kRewriterUIHost[] = "rewriter";

#endif  // BRAVE_COMPONENTS_CONSTANTS_WEBUI_URL_CONSTANTS_H_

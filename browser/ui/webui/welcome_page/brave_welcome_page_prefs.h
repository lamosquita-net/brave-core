/* Copyright (c) 2025 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_BROWSER_UI_WEBUI_WELCOME_PAGE_BRAVE_WELCOME_PAGE_PREFS_H_
#define BRAVE_BROWSER_UI_WEBUI_WELCOME_PAGE_BRAVE_WELCOME_PAGE_PREFS_H_

class PrefRegistrySimple;
class PrefService;

// 156 removed Chromium's prefs::kHasSeenWelcomePage; the welcome page keeps
// its own (as Brave 1.98, without its new welcome page).
namespace brave_welcome_page::prefs {

inline constexpr char kHasSeenBraveWelcomePage[] =
    "brave.has_seen_brave_welcome_page";

void RegisterProfilePrefs(PrefRegistrySimple* registry);
// Registers Chromium's obsolete pref so that MigratePrefs() can read it.
void RegisterProfilePrefsForMigration(PrefRegistrySimple* registry);
// Copies Chromium's obsolete pref into the Brave one and clears it.
void MigratePrefs(PrefService* prefs);

}  // namespace brave_welcome_page::prefs

#endif  // BRAVE_BROWSER_UI_WEBUI_WELCOME_PAGE_BRAVE_WELCOME_PAGE_PREFS_H_

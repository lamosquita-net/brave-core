/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "brave/components/constants/pref_names.h"
#include "build/build_config.h"
#include "chrome/browser/browser_process.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/browser/profiles/profile_manager.h"
#include "chrome/common/pref_names.h"
#include "chrome/test/base/platform_browser_test.h"
#include "components/prefs/pref_service.h"
#include "content/public/test/browser_test.h"

class BraveProfileManagerTest : public PlatformBrowserTest {};

#if !BUILDFLAG(IS_ANDROID)
IN_PROC_BROWSER_TEST_F(BraveProfileManagerTest,
                       PRE_MediaRouterDisabledRestartTest) {
  Profile* profile = g_browser_process->profile_manager()->GetLastUsedProfile();
  {
    profile->GetPrefs()->SetBoolean(::prefs::kEnableMediaRouter, true);
    profile->GetPrefs()->SetBoolean(kEnableMediaRouterOnRestart, false);
    EXPECT_TRUE(profile->GetPrefs()->GetBoolean(::prefs::kEnableMediaRouter));
    EXPECT_FALSE(profile->GetPrefs()->GetBoolean(kEnableMediaRouterOnRestart));
  }
}

IN_PROC_BROWSER_TEST_F(BraveProfileManagerTest,
                       MediaRouterDisabledRestartTest) {
  Profile* profile = g_browser_process->profile_manager()->GetLastUsedProfile();
  {
    EXPECT_FALSE(profile->GetPrefs()->GetBoolean(::prefs::kEnableMediaRouter));
    EXPECT_FALSE(profile->GetPrefs()->GetBoolean(kEnableMediaRouterOnRestart));
  }
}

IN_PROC_BROWSER_TEST_F(BraveProfileManagerTest,
                       PRE_MediaRouterEnabledRestartTest) {
  Profile* profile = g_browser_process->profile_manager()->GetLastUsedProfile();
  {
    profile->GetPrefs()->SetBoolean(::prefs::kEnableMediaRouter, false);
    profile->GetPrefs()->SetBoolean(kEnableMediaRouterOnRestart, true);
    EXPECT_FALSE(profile->GetPrefs()->GetBoolean(::prefs::kEnableMediaRouter));
    EXPECT_TRUE(profile->GetPrefs()->GetBoolean(kEnableMediaRouterOnRestart));
  }
}

IN_PROC_BROWSER_TEST_F(BraveProfileManagerTest, MediaRouterEnabledRestartTest) {
  Profile* profile = g_browser_process->profile_manager()->GetLastUsedProfile();
  {
    EXPECT_TRUE(profile->GetPrefs()->GetBoolean(::prefs::kEnableMediaRouter));
    EXPECT_TRUE(profile->GetPrefs()->GetBoolean(kEnableMediaRouterOnRestart));
  }
}
#endif

// Copyright (c) 2021 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// you can obtain one at http://mozilla.org/MPL/2.0/.

#include "brave/browser/search/ntp_utils.h"

#include <memory>

#include "chrome/test/base/testing_profile.h"
#include "components/prefs/pref_registry_simple.h"
#include "components/prefs/testing_pref_service.h"
#include "components/sync_preferences/testing_pref_service_syncable.h"
#include "content/public/test/browser_task_environment.h"
#include "testing/gtest/include/gtest/gtest.h"

class NTPUtilsTest : public ::testing::Test {
 public:
  NTPUtilsTest() = default;

  void SetUp() override {
    profile_ = std::make_unique<TestingProfile>();
  }

  Profile* profile() { return profile_.get(); }

  PrefService* GetPrefs() { return profile_->GetPrefs(); }

 protected:
  // BrowserTaskEnvironment is needed before TestingProfile
  content::BrowserTaskEnvironment task_environment_;

  std::unique_ptr<TestingProfile> profile_;
};

TEST_F(NTPUtilsTest, ClearsObsoleteWidgetPrefs) {
  auto* prefs = GetPrefs();
  prefs->SetBoolean("brave.new_tab_page.show_rewards", false);
  prefs->SetBoolean("brave.new_tab_page.show_together", true);
  prefs->SetBoolean("brave.new_tab_page.hide_all_widgets", true);
  new_tab_page::MigrateNewTabPagePrefs(prefs);
  EXPECT_FALSE(prefs->HasPrefPath("brave.new_tab_page.show_rewards"));
  EXPECT_FALSE(prefs->HasPrefPath("brave.new_tab_page.show_together"));
  EXPECT_FALSE(prefs->HasPrefPath("brave.new_tab_page.hide_all_widgets"));
}

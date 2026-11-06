// Copyright (c) 2021 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

#include "brave/browser/profiles/brave_profile_manager.h"

#include <memory>

#include "base/files/file_util.h"
#include "base/files/scoped_temp_dir.h"
#include "brave/components/constants/pref_names.h"
#include "chrome/browser/browser_process.h"
#include "chrome/common/pref_names.h"
#include "chrome/test/base/testing_browser_process.h"
#include "chrome/test/base/testing_profile.h"
#include "components/sync_preferences/testing_pref_service_syncable.h"
#include "content/public/test/browser_task_environment.h"
#include "content/public/test/test_utils.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace {

// ProfileManager that provides TestingProfile so that
// creation doesn't crash due to prefs, etc.
// (ref. chrome/test/base/fake_profile_manager.cc)
class TestingBraveProfileManager : public BraveProfileManagerWithoutInit {
 public:
  explicit TestingBraveProfileManager(const base::FilePath& user_data_dir)
      : BraveProfileManagerWithoutInit(user_data_dir) {}

  ~TestingBraveProfileManager() override = default;

  std::unique_ptr<TestingProfile> BuildTestingProfile(
      const base::FilePath& path,
      Delegate* delegate,
      Profile::CreateMode create_mode) {
    return std::make_unique<TestingProfile>(path, delegate, create_mode);
  }

  std::unique_ptr<Profile> CreateProfileHelper(
      const base::FilePath& path) final {
    if (!base::PathExists(path) && !base::CreateDirectory(path)) {
      return nullptr;
    }
    return BuildTestingProfile(path, this, Profile::CreateMode::kSynchronous);
  }

  std::unique_ptr<Profile> CreateProfileAsyncHelper(
      const base::FilePath& path) final {
    // SingleThreadTaskRunner::GetCurrentDefault() is TestingProfile's "async"
    // IOTaskRunner (ref. TestingProfile::GetIOTaskRunner()).
    base::SingleThreadTaskRunner::GetCurrentDefault()->PostTask(
        FROM_HERE,
        base::BindOnce(base::IgnoreResult(&base::CreateDirectory), path));

    return BuildTestingProfile(path, this, Profile::CreateMode::kAsynchronous);
  }
};

}  // namespace

class BraveProfileManagerTest : public testing::Test {
 public:
  BraveProfileManagerTest() = default;

  void SetUp() override {
    ASSERT_TRUE(temp_dir_.CreateUniqueTempDir());
    TestingBrowserProcess::GetGlobal()->SetProfileManager(
        CreateProfileManagerForTest());
  }

  void TearDown() override {
    TestingBrowserProcess::GetGlobal()->SetProfileManager(nullptr);
    content::RunAllTasksUntilIdle();
  }

 protected:
  std::unique_ptr<BraveProfileManager> CreateProfileManagerForTest() {
    return std::make_unique<TestingBraveProfileManager>(temp_dir_.GetPath());
  }

  base::FilePath GetTempPath() { return temp_dir_.GetPath(); }

 private:
  base::ScopedTempDir temp_dir_;
  content::BrowserTaskEnvironment task_environment_;
};

TEST_F(BraveProfileManagerTest, EnableMediaRouterOnRestartDefaultValue) {
  ProfileManager* profile_manager = g_browser_process->profile_manager();
  ASSERT_TRUE(profile_manager);

  base::FilePath path = GetTempPath().AppendASCII("profile");
  TestingProfile::Builder builder;
  builder.SetPath(path);
  builder.SetIsNewProfile(true);
  std::unique_ptr<TestingProfile> profile = builder.Build();

  auto* pref_service = profile->GetTestingPrefService();

  pref_service->RemoveUserPref(kEnableMediaRouterOnRestart);
  EXPECT_TRUE(pref_service->FindPreference(kEnableMediaRouterOnRestart)
                  ->IsDefaultValue());
  pref_service->SetBoolean(::prefs::kEnableMediaRouter, true);
  profile_manager->InitProfileUserPrefs(profile.get());
  EXPECT_TRUE(pref_service->GetBoolean(kEnableMediaRouterOnRestart));

  pref_service->RemoveUserPref(kEnableMediaRouterOnRestart);
  EXPECT_TRUE(pref_service->FindPreference(kEnableMediaRouterOnRestart)
                  ->IsDefaultValue());
  pref_service->SetBoolean(::prefs::kEnableMediaRouter, false);
  profile_manager->InitProfileUserPrefs(profile.get());
  EXPECT_FALSE(pref_service->GetBoolean(kEnableMediaRouterOnRestart));
}

TEST_F(BraveProfileManagerTest, EnableMediaRouterOnRestartNonDefaultValue) {
  ProfileManager* profile_manager = g_browser_process->profile_manager();
  ASSERT_TRUE(profile_manager);

  base::FilePath path = GetTempPath().AppendASCII("profile");
  TestingProfile::Builder builder;
  builder.SetPath(path);
  builder.SetIsNewProfile(true);
  std::unique_ptr<TestingProfile> profile = builder.Build();

  auto* pref_service = profile->GetTestingPrefService();

  EXPECT_FALSE(pref_service->FindPreference(kEnableMediaRouterOnRestart)
                   ->IsDefaultValue());
  pref_service->SetBoolean(kEnableMediaRouterOnRestart, true);
  pref_service->SetBoolean(::prefs::kEnableMediaRouter, false);
  profile_manager->InitProfileUserPrefs(profile.get());
  EXPECT_TRUE(pref_service->GetBoolean(kEnableMediaRouterOnRestart));

  EXPECT_FALSE(pref_service->FindPreference(kEnableMediaRouterOnRestart)
                   ->IsDefaultValue());
  pref_service->SetBoolean(kEnableMediaRouterOnRestart, false);
  pref_service->SetBoolean(::prefs::kEnableMediaRouter, true);
  profile_manager->InitProfileUserPrefs(profile.get());
  EXPECT_FALSE(pref_service->GetBoolean(kEnableMediaRouterOnRestart));
}

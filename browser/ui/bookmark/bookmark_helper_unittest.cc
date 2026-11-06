/* Copyright (c) 2023 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "brave/browser/ui/bookmark/bookmark_helper.h"

#include "components/bookmarks/browser/bookmark_utils.h"
#include "components/bookmarks/common/bookmark_bar_visibility_state.h"
#include "components/bookmarks/common/bookmark_pref_names.h"
#include "components/sync_preferences/testing_pref_service_syncable.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace {

class BookmarkStateMigrationTest : public testing::Test {
 public:
  void SetUp() override {
    bookmarks::RegisterProfilePrefs(prefs_.registry());
    prefs_.registry()->RegisterBooleanPref(
        bookmarks::prefs::kAlwaysShowBookmarkBarOnNTP, true);
  }

  bookmarks::BookmarkBarVisibilityState GetVisibilityState() {
    return static_cast<bookmarks::BookmarkBarVisibilityState>(
        prefs_.GetInteger(bookmarks::prefs::kBookmarkBarVisibilityState));
  }

 protected:
  sync_preferences::TestingPrefServiceSyncable prefs_;
};

}  // namespace

TEST_F(BookmarkStateMigrationTest, DefaultStateIsNotMigrated) {
  brave::MigrateBookmarkState(&prefs_);
  EXPECT_TRUE(
      prefs_.FindPreference(bookmarks::prefs::kBookmarkBarVisibilityState)
          ->IsDefaultValue());
  EXPECT_EQ(GetVisibilityState(),
            bookmarks::BookmarkBarVisibilityState::kOnlyShowOnNtp);
}

TEST_F(BookmarkStateMigrationTest, MigrateAlways) {
  brave::SetBookmarkStateForTesting(brave::BookmarkBarState::kAlways, &prefs_);
  brave::MigrateBookmarkState(&prefs_);
  EXPECT_EQ(GetVisibilityState(),
            bookmarks::BookmarkBarVisibilityState::kAlwaysShow);
  EXPECT_TRUE(
      prefs_.FindPreference(bookmarks::prefs::kAlwaysShowBookmarkBarOnNTP)
          ->IsDefaultValue());
}

TEST_F(BookmarkStateMigrationTest, MigrateNtp) {
  brave::SetBookmarkStateForTesting(brave::BookmarkBarState::kNtp, &prefs_);
  brave::MigrateBookmarkState(&prefs_);
  EXPECT_EQ(GetVisibilityState(),
            bookmarks::BookmarkBarVisibilityState::kOnlyShowOnNtp);
  EXPECT_TRUE(
      prefs_.FindPreference(bookmarks::prefs::kAlwaysShowBookmarkBarOnNTP)
          ->IsDefaultValue());
}

TEST_F(BookmarkStateMigrationTest, MigrateNever) {
  brave::SetBookmarkStateForTesting(brave::BookmarkBarState::kNever, &prefs_);
  brave::MigrateBookmarkState(&prefs_);
  EXPECT_EQ(GetVisibilityState(),
            bookmarks::BookmarkBarVisibilityState::kAlwaysHide);
  EXPECT_TRUE(
      prefs_.FindPreference(bookmarks::prefs::kAlwaysShowBookmarkBarOnNTP)
          ->IsDefaultValue());
}

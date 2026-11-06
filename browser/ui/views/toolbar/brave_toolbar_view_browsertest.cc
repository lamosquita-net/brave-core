// Copyright (c) 2019 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

#include "brave/browser/ui/views/toolbar/brave_toolbar_view.h"

#include "base/check.h"
#include "base/functional/callback_helpers.h"
#include "base/memory/raw_ptr.h"
#include "base/test/scoped_feature_list.h"
#include "brave/browser/ui/views/frame/brave_browser_view.h"
#include "brave/browser/ui/views/toolbar/bookmark_button.h"
#include "brave/components/constants/pref_names.h"
#include "chrome/browser/browser_process.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/browser/profiles/profile_attributes_storage.h"
#include "chrome/browser/profiles/profile_manager.h"
#include "chrome/browser/profiles/profile_test_util.h"
#include "chrome/browser/profiles/profile_window.h"
#include "chrome/browser/search_engines/template_url_service_factory.h"
#include "chrome/browser/ui/browser_window/public/browser_window_interface.h"
#include "chrome/browser/ui/browser_window/public/global_browser_collection.h"
#include "chrome/browser/ui/layout_constants.h"
#include "chrome/browser/ui/views/frame/browser_view.h"
#include "chrome/browser/ui/views/frame/toolbar_button_provider.h"
#include "chrome/browser/ui/views/profiles/avatar_toolbar_button.h"
#include "chrome/browser/ui/views/toolbar/browser_app_menu_button.h"
#include "chrome/browser/ui/views/toolbar/toolbar_view.h"
#include "chrome/test/base/in_process_browser_test.h"
#include "chrome/test/base/search_test_utils.h"
#include "chrome/test/base/ui_test_utils.h"
#include "components/policy/core/browser/browser_policy_connector.h"
#include "components/policy/core/common/mock_configuration_policy_provider.h"
#include "components/policy/core/common/policy_map.h"
#include "components/policy/policy_constants.h"
#include "components/prefs/pref_service.h"
#include "content/public/test/browser_test.h"
#include "content/public/test/test_utils.h"
#include "ui/views/view.h"
#include "ui/views/view_utils.h"

class BraveToolbarViewTest : public InProcessBrowserTest {
 public:
  BraveToolbarViewTest() = default;
  BraveToolbarViewTest(const BraveToolbarViewTest&) = delete;
  BraveToolbarViewTest& operator=(const BraveToolbarViewTest&) = delete;
  ~BraveToolbarViewTest() override = default;

  // InProcessBrowserTest override
  void SetUpOnMainThread() override { Init(browser()); }

  void Init(BrowserWindowInterface* browser) {
    BrowserView* browser_view = BrowserView::GetBrowserViewForBrowser(browser);
    ASSERT_NE(browser_view, nullptr);

    toolbar_view_ = static_cast<BraveToolbarView*>(browser_view->toolbar());
    ASSERT_NE(toolbar_view_, nullptr);

    toolbar_button_provider_ = browser_view->toolbar_button_provider();
    ASSERT_NE(toolbar_button_provider_, nullptr);
  }

 protected:
  bool is_avatar_button_shown() {
    auto* button = static_cast<AvatarToolbarButton*>(
        toolbar_button_provider_->GetAvatarToolbarButtonInterface());
    DCHECK(button);
    return button->GetVisible();
  }

  bool is_bookmark_button_shown() {
    BraveBookmarkButton* bookmark_button = toolbar_view_->bookmark_button();
    DCHECK(bookmark_button);
    return bookmark_button->GetVisible();
  }

  raw_ptr<ToolbarButtonProvider> toolbar_button_provider_ = nullptr;
  raw_ptr<BraveToolbarView> toolbar_view_ = nullptr;
};

IN_PROC_BROWSER_TEST_F(BraveToolbarViewTest, ToolbarDividerNotShownTest) {
  // As we don't use divider in toolbar, it should be null always.
  EXPECT_TRUE(!toolbar_view_->toolbar_divider_for_testing());
}

IN_PROC_BROWSER_TEST_F(BraveToolbarViewTest,
                       AvatarButtonNotShownSingleProfile) {
  EXPECT_EQ(false, is_avatar_button_shown());
}

IN_PROC_BROWSER_TEST_F(BraveToolbarViewTest, AvatarButtonIsShownGuestProfile) {
  // Open a Guest window.
  EXPECT_EQ(1U, GlobalBrowserCollection::GetInstance()->GetSize());
  ui_test_utils::BrowserCreatedObserver browser_creation_observer;
  profiles::SwitchToGuestProfile(base::DoNothing());
  base::RunLoop().RunUntilIdle();
  browser_creation_observer.Wait();
  EXPECT_EQ(2U, GlobalBrowserCollection::GetInstance()->GetSize());

  // Retrieve the new Guest profile.
  Profile* guest = g_browser_process->profile_manager()->GetProfileByPath(
      ProfileManager::GetGuestProfilePath());

  // Access the browser with the Guest profile and re-init test for it.
  BrowserWindowInterface* browser = ui_test_utils::FindAnyBrowser(guest, true);
  EXPECT_TRUE(browser);
  Init(browser);
  EXPECT_EQ(true, is_avatar_button_shown());
}

IN_PROC_BROWSER_TEST_F(BraveToolbarViewTest,
                       AvatarButtonIsShownMultipleProfiles) {
  // Should not be shown in first profile, at first
  EXPECT_EQ(false, is_avatar_button_shown());

  // Create an additional profile.
  ProfileManager* profile_manager = g_browser_process->profile_manager();
  ProfileAttributesStorage& storage =
      profile_manager->GetProfileAttributesStorage();
  base::FilePath current_profile_path = browser()->GetProfile()->GetPath();
  base::FilePath new_path = profile_manager->GenerateNextProfileDirectoryPath();
  Profile& new_profile =
      profiles::testing::CreateProfileSync(profile_manager, new_path);
  ASSERT_EQ(2u, storage.GetNumberOfProfiles());

  // check it's now shown in first profile
  EXPECT_EQ(true, is_avatar_button_shown());

  // Open the new profile
  EXPECT_EQ(1U, GlobalBrowserCollection::GetInstance()->GetSize());
  ui_test_utils::BrowserCreatedObserver browser_creation_observer;
  profiles::OpenBrowserWindowForProfile(
      base::DoNothing(),
      /*always_create=*/false,
      /*is_new_profile=*/true, /*open_command_line_urls=*/false, &new_profile);
  base::RunLoop().RunUntilIdle();
  browser_creation_observer.Wait();
  EXPECT_EQ(2U, GlobalBrowserCollection::GetInstance()->GetSize());

  // Check it's shown in second profile
  BrowserWindowInterface* browser =
      ui_test_utils::FindAnyBrowser(&new_profile, true);
  EXPECT_TRUE(browser);
  Init(browser);
  EXPECT_EQ(true, is_avatar_button_shown());

  // Check avatar is positioned at the right before app menu button.
  // 156 has no toolbar container view: both are direct toolbar children.
  auto* avatar = static_cast<AvatarToolbarButton*>(
      toolbar_button_provider_->GetAvatarToolbarButtonInterface());
  ASSERT_TRUE(!!avatar);
  views::View* app_menu = nullptr;
  for (views::View* child : toolbar_view_->children()) {
    if (views::IsViewClass<BrowserAppMenuButton>(child)) {
      app_menu = child;
      break;
    }
  }
  ASSERT_TRUE(!!app_menu);
  EXPECT_EQ(toolbar_view_->GetIndexOf(avatar).value(),
            toolbar_view_->GetIndexOf(app_menu).value() - 1ul);

  // Check avatar button's size.
  const int avatar_size =
      GetLayoutConstant(LayoutConstant::kToolbarButtonHeight);
  EXPECT_EQ(gfx::Size(avatar_size, avatar_size), avatar->size());
}

IN_PROC_BROWSER_TEST_F(BraveToolbarViewTest,
                       BookmarkButtonCanBeToggledWithPref) {
  auto* prefs = browser()->GetProfile()->GetPrefs();

  // By default, the button should be shown.
  EXPECT_TRUE(prefs->GetBoolean(kShowBookmarksButton));
  EXPECT_TRUE(is_bookmark_button_shown());

  // Hide button.
  prefs->SetBoolean(kShowBookmarksButton, false);
  EXPECT_FALSE(is_bookmark_button_shown());

  // Reshowing the button should also work.
  prefs->SetBoolean(kShowBookmarksButton, true);
  EXPECT_TRUE(is_bookmark_button_shown());
}

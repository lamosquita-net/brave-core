/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/browser/ui/brave_browser_command_controller.h"

#include <memory>
#include <optional>

#include "base/functional/callback_helpers.h"
#include "base/test/scoped_feature_list.h"
#include "brave/app/brave_command_ids.h"
#include "brave/browser/ui/browser_commands.h"
#include "chrome/app/chrome_command_ids.h"
#include "chrome/browser/policy/configuration_policy_handler_list_factory.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/browser/profiles/profile_window.h"
#include "chrome/browser/ui/browser_command_controller.h"
#include "chrome/browser/ui/browser_commands.h"
#include "chrome/browser/ui/browser_list_observer.h"
#include "chrome/browser/ui/browser_tabstrip.h"
#include "chrome/browser/ui/browser_window/public/browser_window_interface.h"
#include "chrome/test/base/in_process_browser_test.h"
#include "chrome/test/base/ui_test_utils.h"
#include "components/policy/core/browser/browser_policy_connector.h"
#include "components/policy/core/common/mock_configuration_policy_provider.h"
#include "components/policy/core/common/policy_map.h"
#include "components/policy/policy_constants.h"
#include "components/prefs/pref_service.h"
#include "components/sync/base/command_line_switches.h"
#include "content/public/browser/notification_service.h"
#include "content/public/test/browser_test.h"
#include "content/public/test/test_utils.h"

#if defined(TOOLKIT_VIEWS)
#include "chrome/browser/ui/views/frame/browser_view.h"
#include "chrome/browser/ui/views/side_panel/side_panel_coordinator.h"
#include "chrome/browser/ui/views/side_panel/side_panel_entry.h"
#include "chrome/browser/ui/views/side_panel/side_panel_entry_id.h"
#include "chrome/browser/ui/views/side_panel/side_panel_entry_key.h"
#include "chrome/browser/ui/views/side_panel/side_panel_enums.h"
#include "chrome/browser/ui/views/side_panel/side_panel_test_utils.h"
#include "chrome/browser/ui/views/side_panel/side_panel_ui.h"
#endif

class BraveBrowserCommandControllerTest : public InProcessBrowserTest {
 public:
  BraveBrowserCommandControllerTest() {
  }
  void SetUpInProcessBrowserTestFixture() override {
    InProcessBrowserTest::SetUpInProcessBrowserTestFixture();
    provider_.SetDefaultReturns(
        /*is_initialization_complete_return=*/true,
        /*is_first_policy_load_complete_return=*/true);
    policy::BrowserPolicyConnector::SetPolicyProviderForTesting(&provider_);
  }

 private:
  policy::MockConfigurationPolicyProvider provider_;
  base::test::ScopedFeatureList scoped_feature_list_;
};

// Regular window
IN_PROC_BROWSER_TEST_F(BraveBrowserCommandControllerTest,
                       BraveCommandsEnableTest) {
  // Test normal browser's brave commands status.
  auto* command_controller = browser()->command_controller();
  EXPECT_TRUE(command_controller->IsCommandEnabled(IDC_SHOW_BRAVE_REWARDS));

  if (syncer::IsSyncAllowedByFlag()) {
    EXPECT_TRUE(command_controller->IsCommandEnabled(IDC_SHOW_BRAVE_SYNC));
  } else {
    EXPECT_FALSE(command_controller->IsCommandEnabled(IDC_SHOW_BRAVE_SYNC));
  }

  EXPECT_TRUE(command_controller->IsCommandEnabled(IDC_ADD_NEW_PROFILE));
  EXPECT_TRUE(command_controller->IsCommandEnabled(IDC_OPEN_GUEST_PROFILE));
  EXPECT_TRUE(
      command_controller->IsCommandEnabled(IDC_SHOW_BRAVE_WEBCOMPAT_REPORTER));
}

// Create private browser and test its brave commands status.
IN_PROC_BROWSER_TEST_F(BraveBrowserCommandControllerTest,
                       BraveCommandsEnableTestPrivateWindow) {
  auto* private_browser = CreateIncognitoBrowser();
  auto* command_controller = private_browser->command_controller();
  EXPECT_TRUE(command_controller->IsCommandEnabled(IDC_SHOW_BRAVE_REWARDS));

  if (syncer::IsSyncAllowedByFlag()) {
    EXPECT_TRUE(command_controller->IsCommandEnabled(IDC_SHOW_BRAVE_SYNC));
  } else {
    EXPECT_FALSE(command_controller->IsCommandEnabled(IDC_SHOW_BRAVE_SYNC));
  }

  EXPECT_TRUE(command_controller->IsCommandEnabled(IDC_ADD_NEW_PROFILE));
  EXPECT_TRUE(command_controller->IsCommandEnabled(IDC_OPEN_GUEST_PROFILE));
  EXPECT_TRUE(
      command_controller->IsCommandEnabled(IDC_SHOW_BRAVE_WEBCOMPAT_REPORTER));
}

// Create guest browser and test its brave commands status.
IN_PROC_BROWSER_TEST_F(BraveBrowserCommandControllerTest,
                       BraveCommandsEnableTestGuestWindow) {
  ui_test_utils::BrowserChangeObserver browser_creation_observer(
      nullptr, ui_test_utils::BrowserChangeObserver::ChangeType::kAdded);
  profiles::SwitchToGuestProfile(base::DoNothing());

  Browser* guest_browser = browser_creation_observer.Wait();
  DCHECK(guest_browser);
  EXPECT_TRUE(guest_browser->profile()->IsGuestSession());
  auto* command_controller = guest_browser->command_controller();
  EXPECT_FALSE(command_controller->IsCommandEnabled(IDC_SHOW_BRAVE_REWARDS));

  EXPECT_FALSE(command_controller->IsCommandEnabled(IDC_SHOW_BRAVE_SYNC));

  EXPECT_FALSE(command_controller->IsCommandEnabled(IDC_ADD_NEW_PROFILE));
  EXPECT_FALSE(command_controller->IsCommandEnabled(IDC_OPEN_GUEST_PROFILE));
  EXPECT_TRUE(
      command_controller->IsCommandEnabled(IDC_SHOW_BRAVE_WEBCOMPAT_REPORTER));
}

IN_PROC_BROWSER_TEST_F(BraveBrowserCommandControllerTest,
                       BraveCommandsCloseTabsToLeft) {
  auto* command_controller = browser()->command_controller();

  // Browser starts with a single about:blank page. Shouldn't be able to close
  // tabs to the left because there's nothing to the left.
  EXPECT_FALSE(
      command_controller->IsCommandEnabled(IDC_WINDOW_CLOSE_TABS_TO_LEFT));

  GURL t1 = GURL("https://example.com");
  chrome::AddTabAt(browser(), t1, 1, true);
  EXPECT_TRUE(
      command_controller->IsCommandEnabled(IDC_WINDOW_CLOSE_TABS_TO_LEFT));
  EXPECT_EQ(2, browser()->tab_strip_model()->count());

  // Open a 3rd tab in the background, so t1 is still focused
  GURL t2 = GURL("https://foo.com");
  chrome::AddTabAt(browser(), t2, 2, false);
  EXPECT_TRUE(
      command_controller->IsCommandEnabled(IDC_WINDOW_CLOSE_TABS_TO_LEFT));
  EXPECT_EQ(3, browser()->tab_strip_model()->count());

  command_controller->ExecuteCommand(IDC_WINDOW_CLOSE_TABS_TO_LEFT);
  EXPECT_EQ(2, browser()->tab_strip_model()->count());

  EXPECT_EQ(t1,
            browser()->tab_strip_model()->GetWebContentsAt(0)->GetVisibleURL());
  EXPECT_EQ(t2,
            browser()->tab_strip_model()->GetWebContentsAt(1)->GetVisibleURL());
}

IN_PROC_BROWSER_TEST_F(BraveBrowserCommandControllerTest,
                       BraveCommandsCloseUnpinnedTabs) {
  auto* tsm = browser()->tab_strip_model();
  auto* command_controller = browser()->command_controller();
  // Should start with one open tab which isn't pinned.
  EXPECT_TRUE(
      command_controller->IsCommandEnabled(IDC_WINDOW_CLOSE_UNPINNED_TABS));

  tsm->SetTabPinned(0, true);
  EXPECT_FALSE(
      command_controller->IsCommandEnabled(IDC_WINDOW_CLOSE_UNPINNED_TABS));

  GURL unpinned("https://example.com");
  chrome::AddTabAt(browser(), unpinned, 1, false);
  EXPECT_TRUE(
      command_controller->IsCommandEnabled(IDC_WINDOW_CLOSE_UNPINNED_TABS));

  GURL will_pin("https://will.pin");
  chrome::AddTabAt(browser(), will_pin, 2, true);
  EXPECT_TRUE(
      command_controller->IsCommandEnabled(IDC_WINDOW_CLOSE_UNPINNED_TABS));

  tsm->SetTabPinned(2, true);
  EXPECT_TRUE(
      command_controller->IsCommandEnabled(IDC_WINDOW_CLOSE_UNPINNED_TABS));

  EXPECT_EQ(3, tsm->count());

  command_controller->ExecuteCommand(IDC_WINDOW_CLOSE_UNPINNED_TABS);
  EXPECT_EQ(2, tsm->count());

  for (int i = 0; i < tsm->count(); ++i) {
    EXPECT_TRUE(tsm->IsTabPinned(i));
  }

  EXPECT_EQ(GURL("about:blank"), tsm->GetWebContentsAt(0)->GetVisibleURL());
  EXPECT_EQ(will_pin, tsm->GetWebContentsAt(1)->GetVisibleURL());
}

IN_PROC_BROWSER_TEST_F(BraveBrowserCommandControllerTest,
                       BraveCommandsAddAllToNewGroup) {
  auto* command_controller = browser()->command_controller();
  auto* tsm = browser()->tab_strip_model();

  GURL url("https://example.com");
  chrome::AddTabAt(browser(), url, 1, false);
  chrome::AddTabAt(browser(), url, 2, false);
  chrome::AddTabAt(browser(), url, 3, false);
  chrome::AddTabAt(browser(), url, 4, false);

  EXPECT_EQ(5, tsm->count());
  command_controller->ExecuteCommand(IDC_WINDOW_ADD_ALL_TABS_TO_NEW_GROUP);
  EXPECT_EQ(5, tsm->count());

  // All tabs should have the same group.
  auto group = tsm->GetTabGroupForTab(0);
  EXPECT_TRUE(group.has_value());

  for (int i = 1; i < tsm->count(); ++i) {
    EXPECT_EQ(group, tsm->GetTabGroupForTab(i));
  }
}

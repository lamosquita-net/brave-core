/* Copyright (c) 2023 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include <optional>

#include "base/test/scoped_feature_list.h"
#include "brave/app/brave_command_ids.h"
#include "brave/browser/themes/brave_dark_mode_utils.h"
#include "brave/browser/ui/brave_browser_command_controller.h"
#include "brave/browser/ui/views/toolbar/brave_browser_app_menu_button.h"
#include "chrome/browser/ui/toolbar/app_menu_icon_controller.h"
#include "chrome/browser/ui/views/frame/browser_view.h"
#include "chrome/browser/ui/views/toolbar/app_menu.h"
#include "chrome/browser/ui/views/toolbar/browser_app_menu_button.h"
#include "chrome/browser/ui/views/toolbar/toolbar_view.h"
#include "chrome/test/base/in_process_browser_test.h"
#include "chrome/test/base/ui_test_utils.h"
#include "content/public/test/browser_test.h"
#include "content/public/test/test_utils.h"
#include "ui/base/ui_base_features.h"
#include "ui/views/controls/button/toggle_button.h"
#include "ui/views/controls/menu/menu_item_view.h"
#include "ui/views/controls/menu/menu_runner.h"
#include "ui/views/view_utils.h"

class BraveAppMenuBrowserTest : public InProcessBrowserTest {
 public:
  BraveAppMenuBrowserTest() {
  }

  ~BraveAppMenuBrowserTest() override = default;

  BrowserAppMenuButton* menu_button() {
    return BrowserView::GetBrowserViewForBrowser(browser())
        ->toolbar()
        ->app_menu_button();
  }

};

IN_PROC_BROWSER_TEST_F(BraveAppMenuBrowserTest, AppMenuButtonUpgradeAlertTest) {
  // Check property for our style.
  auto* brave_menu_button =
      static_cast<BraveBrowserAppMenuButton*>(menu_button());
  EXPECT_TRUE(brave_menu_button->ShouldPaintBorder());
  EXPECT_TRUE(brave_menu_button->ShouldBlendHighlightColor());

  // Check our highlight color.
  dark_mode::SetBraveDarkModeType(
      dark_mode::BraveDarkModeType::BRAVE_DARK_MODE_TYPE_LIGHT);
  EXPECT_EQ(dark_mode::BraveDarkModeType::BRAVE_DARK_MODE_TYPE_LIGHT,
            dark_mode::GetActiveBraveDarkModeType());
  EXPECT_EQ(brave_menu_button->GetHighlightColor(), std::nullopt);
  brave_menu_button->SetTypeAndSeverity(
      {AppMenuIconController::IconType::UPGRADE_NOTIFICATION,
       AppMenuIconController::Severity::LOW});
  EXPECT_EQ(brave_menu_button->GetHighlightColor(),
            SkColorSetRGB(0x00, 0x46, 0x07));
  brave_menu_button->SetTypeAndSeverity(
      {AppMenuIconController::IconType::UPGRADE_NOTIFICATION,
       AppMenuIconController::Severity::MEDIUM});
  EXPECT_EQ(brave_menu_button->GetHighlightColor(),
            SkColorSetRGB(0x4A, 0x39, 0x00));
  brave_menu_button->SetTypeAndSeverity(
      {AppMenuIconController::IconType::UPGRADE_NOTIFICATION,
       AppMenuIconController::Severity::HIGH});
  EXPECT_EQ(brave_menu_button->GetHighlightColor(),
            SkColorSetRGB(0x7D, 0x00, 0x1A));

  EXPECT_EQ(GetLayoutConstant(TOOLBAR_BUTTON_HEIGHT),
            brave_menu_button->height());
}


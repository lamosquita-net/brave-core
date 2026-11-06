/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/browser/ui/views/toolbar/brave_app_menu.h"

#include <memory>

#include "base/debug/crash_logging.h"
#include "base/debug/dump_without_crashing.h"
#include "base/functional/bind.h"
#include "base/logging.h"
#include "base/scoped_observation.h"
#include "brave/app/brave_command_ids.h"
#include "brave/browser/brave_browser_process.h"
#include "brave/browser/ui/color/brave_color_id.h"
#include "brave/browser/ui/toolbar/brave_app_menu_model.h"
#include "brave/grit/brave_generated_resources.h"
#include "cc/paint/paint_flags.h"
#include "chrome/app/chrome_command_ids.h"
#include "chrome/browser/ui/toolbar/app_menu_model.h"
#include "chrome/browser/ui/views/toolbar/app_menu.h"
#include "ui/base/l10n/l10n_util.h"
#include "ui/base/metadata/metadata_header_macros.h"
#include "ui/base/metadata/metadata_impl_macros.h"
#include "ui/base/models/button_menu_item_model.h"
#include "ui/base/models/menu_model.h"
#include "ui/gfx/canvas.h"
#include "ui/views/background.h"
#include "ui/views/controls/button/label_button.h"
#include "ui/views/controls/menu/menu_item_view.h"
#include "ui/views/layout/box_layout_view.h"
#include "ui/views/metadata/view_factory.h"
#include "ui/views/view.h"

using views::MenuItemView;

namespace {

constexpr int kBookmarksCommandIdOffset =
    AppMenuModel::kMinBookmarksCommandId - IDC_FIRST_UNBOUNDED_MENU;
constexpr int kTabGroupsCommandIdOffset =
    AppMenuModel::kMinTabGroupsCommandId - IDC_FIRST_UNBOUNDED_MENU;

// Copied from app_menu.cc to dump w/o crashing
bool IsBookmarkCommand(int command_id) {
  return command_id >= IDC_FIRST_UNBOUNDED_MENU &&
         ((command_id - IDC_FIRST_UNBOUNDED_MENU) %
              AppMenuModel::kNumUnboundedMenuTypes ==
          kBookmarksCommandIdOffset);
}

// Copied from app_menu.cc to dump w/o crashing
bool IsTabGroupsCommand(int command_id) {
  return command_id >= IDC_FIRST_UNBOUNDED_MENU &&
         ((command_id - IDC_FIRST_UNBOUNDED_MENU) %
              AppMenuModel::kNumUnboundedMenuTypes ==
          kTabGroupsCommandIdOffset);
}

}  // namespace

BraveAppMenu::BraveAppMenu(BrowserWindowInterface* browser,
                           ui::MenuModel* model,
                           int run_types,
                           base::RepeatingClosure on_menu_closed_callback)
    : AppMenu(browser, model, run_types, std::move(on_menu_closed_callback)) {}

BraveAppMenu::~BraveAppMenu() = default;

void BraveAppMenu::RunMenu(views::MenuButtonController* host,
                           ui::mojom::MenuSourceType source_type) {
  AppMenu::RunMenu(host, source_type);
}

void BraveAppMenu::ExecuteCommand(int command_id, int mouse_event_flags) {
  // Suspect that the entry is null but can't imagine which command causes it.
  // See
  // https://github.com/brave/brave-browser/issues/37862#issuecomment-2078553575
  if (!IsBookmarkCommand(command_id) && !IsTabGroupsCommand(command_id) &&
      command_id != IDC_CREATE_NEW_TAB_GROUP &&
      command_id != AppMenuModel::kEditMenuPlaceholder &&
      command_id != AppMenuModel::kZoomMenuPlaceholder &&
      command_id_to_entry_.find(command_id) == command_id_to_entry_.end()) {
    LOG(ERROR) << __func__ << " entry should exist for " << command_id;
    SCOPED_CRASH_KEY_NUMBER("BraveAppMenu", "command_id", command_id);
    base::debug::DumpWithoutCrashing();
    return;
  }
  AppMenu::ExecuteCommand(command_id, mouse_event_flags);
}

void BraveAppMenu::OnMenuClosed(views::MenuItemView* menu) {
  // Don't add any code after this. AppMenu::OnMenuClosed() may execute a
  // deferred command (e.g. IDC_FULLSCREEN) that triggers re-entrant menu
  // destruction via AppMenuButton::CloseMenu(), destroying `this`.
  AppMenu::OnMenuClosed(menu);
}

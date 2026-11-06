/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/browser/ui/toolbar/brave_app_menu_model.h"

#include <memory>
#include <optional>
#include <string>

#include "base/check.h"
#include "base/check_op.h"
#include "base/memory/raw_ptr.h"
#include "base/notreached.h"
#include "brave/app/brave_command_ids.h"
#include "brave/browser/ui/toolbar/app_menu_icons.h"
#include "chrome/app/chrome_command_ids.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/browser/ui/browser_window/public/browser_window_interface.h"
#include "chrome/browser/ui/toolbar/app_menu_model.h"
#include "chrome/browser/ui/ui_features.h"
#include "chrome/grit/branded_strings.h"
#include "chrome/grit/generated_resources.h"
#include "components/grit/brave_components_strings.h"
#include "ui/base/l10n/l10n_util.h"
#include "ui/base/models/button_menu_item_model.h"
#include "ui/base/models/menu_separator_types.h"
#include "ui/base/ui_base_features.h"

namespace {

class BraveHelpMenuModel : public ui::SimpleMenuModel {
 public:
  explicit BraveHelpMenuModel(ui::SimpleMenuModel::Delegate* delegate)
      : SimpleMenuModel(delegate) {
    Build();
  }
  BraveHelpMenuModel(const BraveHelpMenuModel&) = delete;
  BraveHelpMenuModel& operator=(const BraveHelpMenuModel&) = delete;
  ~BraveHelpMenuModel() override = default;

 private:
  void Build() {
    AddItemWithStringId(IDC_ABOUT, IDS_ABOUT);
    AddItemWithStringId(IDC_HELP_PAGE_VIA_MENU, IDS_HELP_PAGE);
    AddItemWithStringId(IDC_SHOW_BRAVE_WEBCOMPAT_REPORTER,
                        IDS_SHOW_BRAVE_WEBCOMPAT_REPORTER);
  }
};
}  // namespace
BraveAppMenuModel::BraveAppMenuModel(
    ui::AcceleratorProvider* provider,
    BrowserWindowInterface* browser,
    AppMenuIconController* app_menu_icon_controller,
    AlertMenuItem alert_item)
    : AppMenuModel(provider, browser, app_menu_icon_controller, alert_item) {}

BraveAppMenuModel::~BraveAppMenuModel() = default;

void BraveAppMenuModel::Build() {
  // Customize items after build chromium items.
  // Insert & reorder brave menus based on corresponding commands enable status.
  // If we you want to add/remove from app menu, adjust commands enable status
  // at BraveBrowserCommandController.
  AppMenuModel::Build();

  RemoveUpstreamMenus();
  BuildBraveProductsSection();
  BuildBrowserSection();
  BuildMoreToolsSubMenu();
  BuildPasswordsAndAutofillSubmenu();
  BuildHelpSubMenu();

  ApplyLeoIcons(this);
  ApplyLeoIcons(bookmark_sub_menu_model());

  for (const auto& submenu : sub_menus()) {
    ApplyLeoIcons(submenu.get());
  }

  // TODO(simonhong): apply Nala icons recursively.
  // Can be null in tests.
  if (!bookmark_sub_menu_model()) {
    return;
  }

  if (const auto reading_list_submenu_index =
          bookmark_sub_menu_model()->GetIndexOfCommandId(
              kReadingListMenuPlaceholder)) {
    auto* reading_list_submenu = bookmark_sub_menu_model()->GetSubmenuModelAt(
        *reading_list_submenu_index);
    CHECK(reading_list_submenu);
    ApplyLeoIcons(static_cast<ui::SimpleMenuModel*>(reading_list_submenu));
  }
}

void BraveAppMenuModel::BuildPasswordsAndAutofillSubmenu() {
  if (!GetIndexOfCommandId(kPasswordsAndAutofillMenuPlaceholder)) {
    return;
  }

  auto* autofill_menu_model =
      static_cast<ui::SimpleMenuModel*>(GetSubmenuModelAt(
          GetIndexOfCommandId(kPasswordsAndAutofillMenuPlaceholder).value()));
  CHECK(autofill_menu_model);

  if (IsCommandIdEnabled(IDC_SHOW_IDENTITY_DOCS)) {
    autofill_menu_model->RemoveItemAt(
        autofill_menu_model->GetIndexOfCommandId(IDC_SHOW_IDENTITY_DOCS)
            .value());
  }

  if (IsCommandIdEnabled(IDC_SHOW_TRAVEL)) {
    autofill_menu_model->RemoveItemAt(
        autofill_menu_model->GetIndexOfCommandId(IDC_SHOW_TRAVEL).value());
  }
}

void BraveAppMenuModel::BuildBraveProductsSection() {
  // Needs to add separator as this section is brave specific section.
  bool need_separator = false;

  if (IsCommandIdEnabled(IDC_TOGGLE_AI_CHAT)) {
    InsertItemWithStringIdAt(GetNextIndexOfBraveProductsSection(),
                             IDC_TOGGLE_AI_CHAT, IDS_TOGGLE_AI_CHAT);
    need_separator = true;
  }

  if (need_separator) {
    InsertSeparatorAt(GetNextIndexOfBraveProductsSection(),
                      ui::NORMAL_SEPARATOR);
  }
}

void BraveAppMenuModel::BuildBrowserSection() {
  // The items are currently arranged as
  // History
  // Downloads
  // Bookmarks
  // ...
  // Extensions (from more tools sub menu)
  // and we want to rearrange to
  // History
  // Bookmarks
  // Downloads
  // Extensions
  std::optional<size_t> bookmark_item_index =
      GetIndexOfCommandId(kBookmarksMenuPlaceholder);

  // If bookmark is not used, we don't need to adjust download item.
  if (bookmark_item_index.has_value()) {
    // Place download menu under bookmark.
    DCHECK(IsCommandIdEnabled(IDC_SHOW_DOWNLOADS));
    RemoveItemAt(GetIndexOfCommandId(IDC_SHOW_DOWNLOADS).value());
    InsertItemWithStringIdAt(bookmark_item_index.value(), IDC_SHOW_DOWNLOADS,
                             IDS_SHOW_DOWNLOADS);
  }
}

void BraveAppMenuModel::BuildMoreToolsSubMenu() {
  ui::SimpleMenuModel* more_tools_menu_model =
      static_cast<ui::SimpleMenuModel*>(GetSubmenuModelAt(
          GetIndexOfCommandId(kMoreToolsMenuPlaceholder).value()));
  DCHECK(more_tools_menu_model);

  size_t next_target_index = 0;
  bool need_separator = false;

  // Create New Profile
  if (IsCommandIdEnabled(IDC_ADD_NEW_PROFILE)) {
    more_tools_menu_model->InsertItemWithStringIdAt(
        next_target_index++, IDC_ADD_NEW_PROFILE, IDS_ADD_NEW_PROFILE);
    need_separator = true;
  }

  // Open Guest Window
  if (IsCommandIdEnabled(IDC_OPEN_GUEST_PROFILE)) {
    more_tools_menu_model->InsertItemWithStringIdAt(
        next_target_index++, IDC_OPEN_GUEST_PROFILE, IDS_OPEN_GUEST_PROFILE);
    need_separator = true;
  }

  if (need_separator) {
    more_tools_menu_model->InsertSeparatorAt(next_target_index++,
                                             ui::NORMAL_SEPARATOR);
    need_separator = false;
  }

  // Insert sync menu
  if (IsCommandIdEnabled(IDC_SHOW_BRAVE_SYNC)) {
    more_tools_menu_model->InsertItemWithStringIdAt(
        next_target_index++, IDC_SHOW_BRAVE_SYNC, IDS_SHOW_BRAVE_SYNC);
    need_separator = true;
  }

  if (need_separator) {
    more_tools_menu_model->InsertSeparatorAt(next_target_index++,
                                             ui::NORMAL_SEPARATOR);
    need_separator = false;
  }

  if (auto index = more_tools_menu_model->GetIndexOfCommandId(
          IDC_TASK_MANAGER_APP_MENU)) {
    more_tools_menu_model->InsertItemWithStringIdAt(*index, IDC_DEV_TOOLS,
                                                    IDS_DEV_TOOLS);
  }
}

void BraveAppMenuModel::BuildHelpSubMenu() {
  // Put help sub menu above the settings menu.
  if (const auto index = GetIndexOfCommandId(IDC_OPTIONS)) {
    sub_menus().push_back(std::make_unique<BraveHelpMenuModel>(this));
    InsertSubMenuWithStringIdAt(*index, kHelpMenuPlaceholder, IDS_HELP_MENU,
                                sub_menus().back().get());
  }
}

void BraveAppMenuModel::RemoveUpstreamMenus() {
  ui::SimpleMenuModel* more_tools_model =
      static_cast<ui::SimpleMenuModel*>(GetSubmenuModelAt(
          GetIndexOfCommandId(kMoreToolsMenuPlaceholder).value()));
  DCHECK(more_tools_model);

  {
    // Remove upstream's profile menu. "Add new profile" will be added into more
    // tools sub menu.
    auto index = GetIndexOfCommandId(kProfileMenuPlaceholder);
    CHECK(index);
    RemoveItemAt(*index);

    // Upstream uses spacing separator after profile menu but we don't use
    // profile menu here. Spacing separator just adds unnecessary space.
    if (GetSeparatorTypeAt(*index) == ui::SPACING_SEPARATOR) {
      RemoveItemAt(*index);
    }
  }

  {
    // Remove upstream's translate item, as it's not hooked up to the
    // translation UI in the omnibox
    auto index = GetIndexOfCommandId(IDC_SHOW_TRANSLATE);
    CHECK(index);
    RemoveItemAt(*index);
  }

  // Remove upstream's "Tab groups" menu item, as this functionality is already
  // available in multiple other places
  if (const auto index = GetIndexOfCommandId(kSavedTabGroupsMenuPlaceholder)) {
    RemoveItemAt(*index);
  }

  // Remove upstream's dev tools menu and associated separator.
  // It'll be changed its position in more tools.
  if (const auto index = more_tools_model->GetIndexOfCommandId(IDC_DEV_TOOLS)) {
    // If task manager is not existed, just remove separator above the dev tools
    // item. Otherwise, remove separator and item both.
    DCHECK_EQ(ui::MenuModel::TYPE_SEPARATOR,
              more_tools_model->GetTypeAt((*index) - 1));
    if (!more_tools_model->GetIndexOfCommandId(IDC_TASK_MANAGER_APP_MENU)
             .has_value()) {
      more_tools_model->RemoveItemAt((*index) - 1);
    } else {
      more_tools_model->RemoveItemAt(*index);
      more_tools_model->RemoveItemAt((*index) - 1);
    }
  }

  // Remove upstream's `Reading Mode` item as we have our own `Speed reader`.
  // Also remove associated separator.
  if (const auto index = more_tools_model->GetIndexOfCommandId(
          IDC_SHOW_READING_MODE_SIDE_PANEL)) {
    CHECK_EQ(ui::MenuModel::TYPE_SEPARATOR,
             more_tools_model->GetTypeAt(*index + 1));
    more_tools_model->RemoveItemAt(*index + 1);
    more_tools_model->RemoveItemAt(*index);
  }

  // Remove upstream's "Customize Chrome" menu item, as we plan to enhance this
  // feature before exposing it to users
  if (const auto index = more_tools_model->GetIndexOfCommandId(
          IDC_SHOW_CUSTOMIZE_CHROME_SIDE_PANEL)) {
    more_tools_model->RemoveItemAt(*index);
  }

  // Remove upstream's about menu. It's moved into help sub menu.
  if (const auto index = GetIndexOfCommandId(IDC_ABOUT)) {
    RemoveItemAt(*index);
  }
}

void BraveAppMenuModel::ExecuteCommand(int id, int event_flags) {
  return AppMenuModel::ExecuteCommand(id, event_flags);
}

bool BraveAppMenuModel::IsCommandIdEnabled(int id) const {
  return AppMenuModel::IsCommandIdEnabled(id);
}

size_t BraveAppMenuModel::GetNextIndexOfBraveProductsSection() const {
  std::vector<int> commands_to_check = {
      IDC_TOGGLE_AI_CHAT, IDC_NEW_INCOGNITO_WINDOW, IDC_NEW_WINDOW};
  const auto last_index_of_second_section =
      GetProperItemIndex(commands_to_check, false).value();
  const auto last_cmd_id_of_second_section =
      GetCommandIdAt(last_index_of_second_section);

  // If |last_cmd_id_of_second_section| is from new tab & windows section,
  // no item is added to second section yet.
  if (last_cmd_id_of_second_section == IDC_NEW_INCOGNITO_WINDOW ||
      last_cmd_id_of_second_section == IDC_NEW_WINDOW) {
    // Used additional "+1" to skip separator.
    DCHECK_EQ(ui::MenuModel::TYPE_SEPARATOR,
              GetTypeAt(last_index_of_second_section + 1));
    return last_index_of_second_section + 2;
  }

  return last_index_of_second_section + 1;
}

std::optional<size_t> BraveAppMenuModel::GetProperItemIndex(
    std::vector<int> commands_to_check,
    bool insert_next) const {
  const size_t commands_size = commands_to_check.size();
  for (size_t i = 0; i < commands_size; i++) {
    std::optional<size_t> item_index =
        GetIndexOfCommandId(commands_to_check[i]);
    if (item_index.has_value()) {
      return insert_next ? item_index.value() + 1 : item_index;
    }
  }

  NOTREACHED() << "At least, a menu item for this command should exist: "
               << commands_to_check[commands_size - 1];
}

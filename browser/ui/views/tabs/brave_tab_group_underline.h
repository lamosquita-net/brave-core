/* Copyright (c) 2022 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_BROWSER_UI_VIEWS_TABS_BRAVE_TAB_GROUP_UNDERLINE_H_
#define BRAVE_BROWSER_UI_VIEWS_TABS_BRAVE_TAB_GROUP_UNDERLINE_H_

#include "chrome/browser/ui/views/tabs/tab_group_underline.h"
#include "ui/base/metadata/metadata_header_macros.h"

class BraveTabGroupUnderline : public TabGroupUnderline {
  METADATA_HEADER(BraveTabGroupUnderline, TabGroupUnderline)
 public:
  BraveTabGroupUnderline(TabGroupViews* tab_group_views,
                         const tab_groups::TabGroupId& group,
                         const TabGroupStyle& style);
  ~BraveTabGroupUnderline() override;

  static int GetStrokeInset();

  // TabGroupUnderline:
  gfx::Insets GetInsetsForUnderline(
      const views::View* sibling_view) const override;
  void OnPaint(gfx::Canvas* canvas) override;
};

#endif  // BRAVE_BROWSER_UI_VIEWS_TABS_BRAVE_TAB_GROUP_UNDERLINE_H_

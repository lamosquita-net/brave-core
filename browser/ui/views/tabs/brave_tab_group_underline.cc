/* Copyright (c) 2022 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/browser/ui/views/tabs/brave_tab_group_underline.h"

#include "brave/browser/ui/tabs/brave_tab_layout_constants.h"
#include "brave/browser/ui/tabs/features.h"
#include "cc/paint/paint_flags.h"
#include "chrome/browser/ui/views/tabs/tab_group_style.h"
#include "chrome/browser/ui/views/tabs/tab_group_views.h"
#include "ui/base/metadata/metadata_impl_macros.h"
#include "ui/gfx/canvas.h"

BraveTabGroupUnderline::BraveTabGroupUnderline(
    TabGroupViews* tab_group_views,
    const tab_groups::TabGroupId& group,
    const TabGroupStyle& style)
    : TabGroupUnderline(tab_group_views, group, style) {}

BraveTabGroupUnderline::~BraveTabGroupUnderline() = default;

gfx::Insets BraveTabGroupUnderline::GetInsetsForUnderline(
    const views::View* sibling_view) const {
  if (!tabs::features::HorizontalTabsUpdateEnabled()) {
    return TabGroupUnderline::GetInsetsForUnderline(sibling_view);
  }

  // For horizontal tabs, the underline should be inset slightly within the
  // visual edges of the tab.
  int horizontal_inset =
      TabGroupUnderline::kStrokeThickness + brave_tabs::kHorizontalTabInset;

  return gfx::Insets::VH(0, horizontal_inset);
}

void BraveTabGroupUnderline::OnPaint(gfx::Canvas* canvas) {
  if (!tabs::features::HorizontalTabsUpdateEnabled()) {
    return TabGroupUnderline::OnPaint(canvas);
  }

  const SkColor color =
      SkColorSetA(tab_group_views_->GetGroupColor(), 0.6 * 255);

  SkPath path = style_->GetUnderlinePath(GetLocalBounds());
  cc::PaintFlags flags;
  flags.setAntiAlias(true);
  flags.setColor(color);
  flags.setStyle(cc::PaintFlags::kFill_Style);
  canvas->DrawPath(path, flags);
}

// static
int BraveTabGroupUnderline::GetStrokeInset() {
  if (!tabs::features::HorizontalTabsUpdateEnabled()) {
    return TabGroupUnderline::GetStrokeInset();
  }
  return brave_tabs::kHorizontalTabInset;
}

BEGIN_METADATA(BraveTabGroupUnderline)
END_METADATA

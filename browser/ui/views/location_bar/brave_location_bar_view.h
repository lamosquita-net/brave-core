/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_BROWSER_UI_VIEWS_LOCATION_BAR_BRAVE_LOCATION_BAR_VIEW_H_
#define BRAVE_BROWSER_UI_VIEWS_LOCATION_BAR_BRAVE_LOCATION_BAR_VIEW_H_

#include <memory>
#include <vector>

#include "base/types/pass_key.h"
#include "brave/browser/ui/views/toolbar/brave_toolbar_view.h"
#include "brave/browser/ui/views/view_shadow.h"
#include "chrome/browser/ui/views/location_bar/location_bar_view.h"
#include "ui/base/metadata/metadata_header_macros.h"

class BraveActionsContainer;
class BraveActionsContainerTest;
class BrowserWindowInterface;
class SkPath;

// The purposes of this subclass are to:
// - Add the BraveActionsContainer to the location bar
class BraveLocationBarView : public LocationBarView {
  METADATA_HEADER(BraveLocationBarView, LocationBarView)
 public:
  BraveLocationBarView(BrowserWindowInterface* browser,
                       Profile* profile,
                       CommandUpdater* command_updater,
                       Delegate* delegate,
                       bool is_popup_mode);
  ~BraveLocationBarView() override;

  BraveLocationBarView(const BraveLocationBarView&) = delete;
  BraveLocationBarView& operator=(const BraveLocationBarView&) = delete;

  void Init() override;
  void Update(content::WebContents* contents) override;
  void OnChanged() override;
  BraveActionsContainer* GetBraveActionsContainer() { return brave_actions_; }

  // LocationBarView:
  // Views that locates at right side of upstream's trailing views.
  std::vector<views::View*> GetRightMostTrailingViews() override;
  // Views that locates at left side of upstream's trailing views.
  std::vector<views::View*> GetLeftMostTrailingViews() override;
  void RefreshBackground() override;
  void Layout(PassKey) override;
  void OnVisibleBoundsChanged() override;
  int GetMinimumTrailingWidth() const override;

  // views::View:
  gfx::Size GetMinimumSize() const override;
  void OnThemeChanged() override;
  void ChildVisibilityChanged(views::View* child) override;
  void AddedToWidget() override;
  int GetBorderRadius() const override;
  void FocusLocation(bool is_user_initiated,
                     bool clear_focus_if_failed) override;

  SkPath GetFocusRingHighlightPath() const;
  ContentSettingImageView* GetContentSettingsImageViewForTesting(size_t idx);
  BraveActionsContainer* brave_actions_contatiner_view() {
    return brave_actions_;
  }

  void set_ignore_layout(base::PassKey<BraveToolbarView::LayoutGuard>,
                         bool ignore) {
    ignore_layout_ = ignore;
  }

 private:
  friend class ::BraveActionsContainerTest;

  void SetupShadow();

  // Prevent layout with invalid rect.
  // It also could make omnibox popup have wrong position.
  // See the comments of BraveToolbarView::Layout().
  bool ignore_layout_ = false;
  std::unique_ptr<ViewShadow> shadow_;
  raw_ptr<BraveActionsContainer> brave_actions_ = nullptr;
};

#endif  // BRAVE_BROWSER_UI_VIEWS_LOCATION_BAR_BRAVE_LOCATION_BAR_VIEW_H_

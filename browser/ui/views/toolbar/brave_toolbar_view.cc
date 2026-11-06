/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/browser/ui/views/toolbar/brave_toolbar_view.h"

#include <algorithm>
#include <memory>
#include <utility>

#include "base/check.h"
#include "base/check_op.h"
#include "base/functional/bind.h"
#include "brave/app/brave_command_ids.h"
#include "brave/browser/ui/views/location_bar/brave_location_bar_view.h"
#include "brave/browser/ui/views/toolbar/bookmark_button.h"
#include "brave/components/constants/pref_names.h"
#include "chrome/app/chrome_command_ids.h"
#include "chrome/browser/browser_process.h"
#include "chrome/browser/defaults.h"
#include "chrome/browser/profiles/profile_manager.h"
#include "chrome/browser/ui/browser_commands.h"
#include "chrome/browser/ui/browser_window/public/browser_window_interface.h"
#include "chrome/browser/ui/layout_constants.h"
#include "chrome/browser/ui/ui_features.h"
#include "chrome/browser/ui/views/bookmarks/bookmark_bubble_view.h"
#include "chrome/browser/ui/views/profiles/avatar_toolbar_button.h"
#include "chrome/browser/ui/views/toolbar/browser_app_menu_button.h"
#include "chrome/browser/ui/views/toolbar/toolbar_divider.h"
#include "chrome/browser/ui/views/toolbar/toolbar_view.h"
#include "components/bookmarks/common/bookmark_pref_names.h"
#include "components/prefs/pref_service.h"
#include "ui/base/hit_test.h"
#include "ui/base/metadata/metadata_impl_macros.h"
#include "ui/base/window_open_disposition_utils.h"
#include "ui/events/event.h"
#include "ui/views/bubble/bubble_anchor.h"
#include "ui/views/window/hit_test_utils.h"

#if BUILDFLAG(IS_LINUX)
#include "chrome/common/pref_names.h"
#endif

namespace {
constexpr int kLocationBarMaxWidth = 1080;

double GetLocationBarMarginHPercent(int toolbar_width) {
  double location_bar_margin_h_pc = 0.07;
  if (toolbar_width < 700)
    location_bar_margin_h_pc = 0;
  else if (toolbar_width < 850)
    location_bar_margin_h_pc = 0.03;
  else if (toolbar_width < 1000)
    location_bar_margin_h_pc = 0.05;
  return location_bar_margin_h_pc;
}

gfx::Insets CalcLocationBarMargin(int toolbar_width,
                                  int available_location_bar_width,
                                  int location_bar_min_width,
                                  int location_bar_x) {
  // Apply the target margin, adjusting for min and max width of LocationBar
  // Make sure any margin doesn't shrink the LocationBar beyond minimum width
  int location_bar_max_margin_h =
      (available_location_bar_width - location_bar_min_width) / 2;
  int location_bar_margin_h =
      std::min(static_cast<int>(toolbar_width *
                                GetLocationBarMarginHPercent(toolbar_width)),
               location_bar_max_margin_h);
  int location_bar_width =
      available_location_bar_width - (location_bar_margin_h * 2);
  // Allow the margin to expand so LocationBar is restrained to max width
  if (location_bar_width > kLocationBarMaxWidth) {
    location_bar_margin_h += (location_bar_width - kLocationBarMaxWidth) / 2;
    location_bar_width = kLocationBarMaxWidth;
  }

  // Center LocationBar as much as possible within Toolbar
  const int location_bar_toolbar_center_point =
      location_bar_x + location_bar_margin_h + (location_bar_width / 2);
  // Calculate offset - positive for move left and negative for move right
  int location_bar_center_offset =
      location_bar_toolbar_center_point - (toolbar_width / 2);
  // Can't shim more than we have space for, so restrict to margin size
  // or in the case of moving-right, 25% of the space since we want to avoid
  // touching browser actions where possible
  location_bar_center_offset =
      (location_bar_center_offset > 0)
          ? std::min(location_bar_margin_h, location_bar_center_offset)
          : std::max(static_cast<int>(-location_bar_margin_h * .25),
                     location_bar_center_offset);

  // // Apply offset to margin
  const int location_bar_margin_l =
      location_bar_margin_h - location_bar_center_offset;
  const int location_bar_margin_r =
      location_bar_margin_h + location_bar_center_offset;
  return gfx::Insets::TLBR(0, location_bar_margin_l, 0, location_bar_margin_r);
}

bool HasMultipleUserProfiles() {
  ProfileAttributesStorage* profile_storage =
      &g_browser_process->profile_manager()->GetProfileAttributesStorage();
  size_t profile_count = profile_storage->GetNumberOfProfiles();
  return (profile_count != 1);
}

bool IsAvatarButtonHideable(Profile* profile) {
  return !profile->IsIncognitoProfile() && !profile->IsGuestSession();
}

}  // namespace

// Keeps BraveLocationBarView from laying out with the intermediate (wide)
// bounds that upstream's flex layout gives it before ResetLocationBarBounds().
class BraveToolbarView::LayoutGuard {
 public:
  explicit LayoutGuard(BraveLocationBarView* b) : bar_(b) {}
  ~LayoutGuard() { set_ignore_layout(false); }

  LayoutGuard(const LayoutGuard&) = delete;
  LayoutGuard& operator=(const LayoutGuard&) = delete;

  void set_ignore_layout(bool ignore) {
    if (bar_) {
      bar_->set_ignore_layout({}, ignore);
    }
  }

 private:
  raw_ptr<BraveLocationBarView> bar_ = nullptr;
};

BraveToolbarView::BraveToolbarView(BrowserWindowInterface* browser,
                                   BrowserView* browser_view)
    : ToolbarView(browser, browser_view) {}

BraveToolbarView::~BraveToolbarView() = default;

void BraveToolbarView::Init() {
  ToolbarView::Init();

  // This will allow us to move this window by dragging toolbar.
  // See brave_non_client_hit_test_helper.h
  views::SetHitTestComponent(this, HTCAPTION);

  // For non-normal mode, we don't have to more.
  if (display_mode_ != DisplayMode::kNormal) {
    brave_initialized_ = true;
    return;
  }

  // Only the WebUI location bar (force-disabled by us) leaves this null.
  if (!location_bar_view_) {
    brave_initialized_ = true;
    return;
  }

  Profile* profile = browser()->GetProfile();

  // We don't use divider between extensions container and other toolbar
  // buttons. Upstream conditionally creates |toolbar_divider_|, they check
  // whether it's null or not. So safe to make remove here.
  if (toolbar_divider_) {
    auto view = RemoveChildViewT(toolbar_divider_.get());
    toolbar_divider_ = nullptr;
  }

  // Track changes in profile count
  if (IsAvatarButtonHideable(profile)) {
    profile_observer_.Observe(
        &g_browser_process->profile_manager()->GetProfileAttributesStorage());
  }
  // track changes in bookmarks enabled setting
  edit_bookmarks_enabled_.Init(
      bookmarks::prefs::kEditBookmarksEnabled, profile->GetPrefs(),
      base::BindRepeating(&BraveToolbarView::OnEditBookmarksEnabledChanged,
                          base::Unretained(this)));
  show_bookmarks_button_.Init(
      kShowBookmarksButton, profile->GetPrefs(),
      base::BindRepeating(&BraveToolbarView::OnShowBookmarksButtonChanged,
                          base::Unretained(this)));

  // track changes in wide locationbar setting
  location_bar_is_wide_.Init(
      kLocationBarIsWide, profile->GetPrefs(),
      base::BindRepeating(&BraveToolbarView::OnLocationBarIsWideChanged,
                          base::Unretained(this)));

  const auto callback = [](BrowserWindowInterface* browser, int command,
                           const ui::Event& event) {
    chrome::ExecuteCommandWithDisposition(
        browser, command, ui::DispositionFromEventFlags(event.flags()));
  };

  bookmark_ =
      AddChildViewAt(std::make_unique<BraveBookmarkButton>(base::BindRepeating(
                         callback, browser_, IDC_BOOKMARK_THIS_TAB)),
                     *GetIndexOf(location_bar_view_));
  bookmark_->SetTriggerableEventFlags(ui::EF_LEFT_MOUSE_BUTTON |
                                      ui::EF_MIDDLE_MOUSE_BUTTON);
  bookmark_->UpdateImageAndText();

  // Make sure that avatar button should be located right before the app menu.
  if (auto* avatar_button = static_cast<AvatarToolbarButton*>(
          GetAvatarToolbarButtonInterface());
      avatar_button && app_menu_button_) {
    ReorderChildView(avatar_button, *GetIndexOf(app_menu_button_) - 1);
  }

  // Side panel pin buttons are kept out of the toolbar by the
  // PinnedToolbarActionsContainer override (only downloads and send tab to
  // self show ephemerally). Since 156 the container also hosts the downloads
  // button, so it is no longer removed here.

  brave_initialized_ = true;
}

void BraveToolbarView::OnEditBookmarksEnabledChanged() {
  DCHECK_EQ(DisplayMode::kNormal, display_mode_);
  Update(nullptr);
}

void BraveToolbarView::OnShowBookmarksButtonChanged() {
  if (!bookmark_)
    return;

  UpdateBookmarkVisibility();
}

void BraveToolbarView::OnLocationBarIsWideChanged() {
  DCHECK_EQ(DisplayMode::kNormal, display_mode_);

  DeprecatedLayoutImmediately();
  SchedulePaint();
}

void BraveToolbarView::OnThemeChanged() {
  ToolbarView::OnThemeChanged();

  if (!brave_initialized_)
    return;

  if (display_mode_ == DisplayMode::kNormal && bookmark_)
    bookmark_->UpdateImageAndText();
}

void BraveToolbarView::OnProfileAdded(const base::FilePath& profile_path) {
  Update(nullptr);
}

void BraveToolbarView::OnProfileWasRemoved(const base::FilePath& profile_path,
                                           const std::u16string& profile_name) {
  Update(nullptr);
}

void BraveToolbarView::LoadImages() {
  ToolbarView::LoadImages();
  if (bookmark_)
    bookmark_->UpdateImageAndText();
}

void BraveToolbarView::Update(content::WebContents* tab) {
  ToolbarView::Update(tab);

  // Decide whether to show the bookmark button
  UpdateBookmarkVisibility();

  // Remove avatar menu if only a single user profile exists.
  // Always show if private / tor / guest window, as an indicator.
  auto* avatar_button =
      static_cast<AvatarToolbarButton*>(GetAvatarToolbarButtonInterface());
  if (avatar_button) {
    auto* profile = browser_->GetProfile();
    const bool should_show_profile =
        !IsAvatarButtonHideable(profile) || HasMultipleUserProfiles();
    avatar_button->SetVisible(should_show_profile);
  }
}

void BraveToolbarView::UpdateBookmarkVisibility() {
  if (!bookmark_)
    return;

  DCHECK_EQ(DisplayMode::kNormal, display_mode_);
  bookmark_->SetVisible(browser_defaults::bookmarks_enabled &&
                        edit_bookmarks_enabled_.GetValue() &&
                        show_bookmarks_button_.GetValue());
}

void BraveToolbarView::ShowBookmarkBubble(const GURL& url,
                                          bool already_bookmarked) {
  // Show BookmarkBubble attached to Brave's bookmark button
  // or the location bar if there is no bookmark button
  // (i.e. in non-normal display mode).
  views::View* anchor_view = location_bar_view_;
  if (bookmark_ && bookmark_->GetVisible()) {
    anchor_view = bookmark_;
  }

  // Passing `nullptr` to `highlighted_button` as this argument only gets used
  // by the legacy page action framework.
  BookmarkBubbleView::ShowBubble(
      views::BubbleAnchor(anchor_view), GetWebContents(),
      /*highlighted_button=*/nullptr, browser_, url, already_bookmarked);
}

void BraveToolbarView::ViewHierarchyChanged(
    const views::ViewHierarchyChangedDetails& details) {
  ToolbarView::ViewHierarchyChanged(details);

  // Since 156 the toolbar buttons are direct children of ToolbarView.
  if (details.is_add && details.parent == this) {
    // Mark children of the container view as client area so that they are not
    // perceived as caption area. See brave_non_client_hit_test_helper.h
    views::SetHitTestComponent(details.child, HTCLIENT);
  }
}

void BraveToolbarView::Layout(PassKey) {
  if (!brave_initialized_ || display_mode_ != DisplayMode::kNormal ||
      !location_bar_view_) {
    LayoutSuperclass<ToolbarView>(this);
    return;
  }

  // In this Layout, location bar's rect is set twice: first by upstream's
  // flex layout (fits the wide address bar) and, if the wide address bar
  // option is off, again by ResetLocationBarBounds(). The omnibox popup is
  // anchored to the location bar, so skip the location bar layout for the
  // first rect.
  LayoutGuard guard(static_cast<BraveLocationBarView*>(location_bar_view_));
  if (!location_bar_is_wide_.GetValue()) {
    guard.set_ignore_layout(true);
  }

  LayoutSuperclass<ToolbarView>(this);

  if (!location_bar_is_wide_.GetValue()) {
    guard.set_ignore_layout(false);
    ResetLocationBarBounds();
    ResetBookmarkButtonBounds();
  }
}

void BraveToolbarView::ResetLocationBarBounds() {
  DCHECK_EQ(DisplayMode::kNormal, display_mode_);

  // Calculate proper location bar's margin and set its bounds.
  const gfx::Insets margin = CalcLocationBarMargin(
      width(), location_bar_view_->width(),
      location_bar_view_->GetMinimumSize().width(), location_bar_view_->x());

  // At minimum width the margin is empty and SetBounds() exits early without
  // laying out, which would leave children at stale positions.
  if (margin.IsEmpty()) {
    location_bar_view_->views::View::InvalidateLayout(
        /*avoid_propagate_during_layout*/ true);
  }
  location_bar_view_->SetBounds(location_bar_view_->x() + margin.left(),
                                location_bar_view_->y(),
                                location_bar_view_->width() - margin.width(),
                                location_bar_view_->height());
}

void BraveToolbarView::ResetBookmarkButtonBounds() {
  DCHECK_EQ(DisplayMode::kNormal, display_mode_);

  int button_right_margin =
      GetLayoutConstant(LayoutConstant::kLocationBarMargin);

  if (bookmark_ && bookmark_->GetVisible()) {
    const int bookmark_width = bookmark_->GetPreferredSize().width();
    const int bookmark_x =
        location_bar_view_->x() - bookmark_width - button_right_margin;
    bookmark_->SetX(bookmark_x);
  }
}

BEGIN_METADATA(BraveToolbarView)
END_METADATA

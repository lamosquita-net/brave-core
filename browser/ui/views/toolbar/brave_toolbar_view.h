/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_BROWSER_UI_VIEWS_TOOLBAR_BRAVE_TOOLBAR_VIEW_H_
#define BRAVE_BROWSER_UI_VIEWS_TOOLBAR_BRAVE_TOOLBAR_VIEW_H_

#include "base/memory/raw_ptr.h"
#include "base/scoped_observation.h"
#include "chrome/browser/profiles/profile_attributes_storage.h"
#include "chrome/browser/ui/views/toolbar/toolbar_view.h"
#include "components/prefs/pref_member.h"
#include "ui/base/metadata/metadata_header_macros.h"

class BraveBookmarkButton;

class BraveToolbarView : public ToolbarView,
                         public ProfileAttributesStorage::Observer {
  METADATA_HEADER(BraveToolbarView, ToolbarView)
 public:
  explicit BraveToolbarView(Browser* browser, BrowserView* browser_view);
  ~BraveToolbarView() override;

  BraveBookmarkButton* bookmark_button() const { return bookmark_; }

  void Init() override;
  void Layout(PassKey) override;
  void Update(content::WebContents* tab) override;
  void OnThemeChanged() override;
  void OnEditBookmarksEnabledChanged();
  void OnLocationBarIsWideChanged();
  void OnShowBookmarksButtonChanged();
  void ShowBookmarkBubble(const GURL& url, bool already_bookmarked) override;
  void ViewHierarchyChanged(
      const views::ViewHierarchyChangedDetails& details) override;

 private:
  FRIEND_TEST_ALL_PREFIXES(BraveToolbarViewTest, ToolbarDividerNotShownTest);

  void LoadImages() override;
  void ResetLocationBarBounds();
  void ResetButtonBounds();
  void UpdateBookmarkVisibility();

  // ToolbarButtonProvider:
  views::View* GetAnchorView(std::optional<PageActionIconType> type) override;

  // ProfileAttributesStorage::Observer:
  void OnProfileAdded(const base::FilePath& profile_path) override;
  void OnProfileWasRemoved(const base::FilePath& profile_path,
                           const std::u16string& profile_name) override;

  views::View* toolbar_divider_for_testing() { return toolbar_divider_; }

  raw_ptr<BraveBookmarkButton> bookmark_ = nullptr;
  // Tracks the preference to determine whether bookmark editing is allowed.
  BooleanPrefMember edit_bookmarks_enabled_;

  BooleanPrefMember show_bookmarks_button_;

  BooleanPrefMember location_bar_is_wide_;

  // Whether this toolbar has been initialized.
  bool brave_initialized_ = false;
  // Tracks profile count to determine whether profile switcher should be shown.
  base::ScopedObservation<ProfileAttributesStorage,
                          ProfileAttributesStorage::Observer>
      profile_observer_{this};
};

#endif  // BRAVE_BROWSER_UI_VIEWS_TOOLBAR_BRAVE_TOOLBAR_VIEW_H_

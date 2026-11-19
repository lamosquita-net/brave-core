/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "chrome/browser/ui/bookmarks/bookmark_utils.h"

#include <utility>

#include "brave/app/vector_icons/vector_icons.h"
#include "build/build_config.h"
#include "components/vector_icons/vector_icons.h"
#include "ui/base/models/image_model.h"
#include "ui/color/color_variant.h"

#define IsAppsShortcutEnabled IsAppsShortcutEnabled_Unused
#define ShouldShowAppsShortcutInBookmarkBar \
  ShouldShowAppsShortcutInBookmarkBar_Unused

#if defined(TOOLKIT_VIEWS)
#define GetBookmarkFolderIcon GetBookmarkFolderIcon_UnUsed
#endif

#include "src/chrome/browser/ui/bookmarks/bookmark_utils.cc"
#undef IsAppsShortcutEnabled
#undef ShouldShowAppsShortcutInBookmarkBar

#if defined(TOOLKIT_VIEWS)
#undef GetBookmarkFolderIcon
#endif

namespace chrome {

bool IsAppsShortcutEnabled(Profile* profile) {
  return false;
}

bool ShouldShowAppsShortcutInBookmarkBar(Profile* profile) {
  return false;
}

#if defined(TOOLKIT_VIEWS)
// FlyWeb 3.2.0 interface: a flat vector folder from Titín's design, in the
// color asked for by the caller (kColorBookmarkFolderIcon = the toolbar icon
// color), instead of Brave's PNG folders with a gradient.
ui::ImageModel GetBookmarkFolderIcon(BookmarkFolderIconType icon_type,
                                     ui::ColorVariant color) {
  constexpr int kFolderIconSize = 16;
  return ui::ImageModel::FromVectorIcon(
      icon_type == BookmarkFolderIconType::kNormal
          ? kFlywebBookmarkFolderIcon
          : vector_icons::kFolderManagedRefreshOldIcon,
      std::move(color), kFolderIconSize);
}
#endif

}  // namespace chrome

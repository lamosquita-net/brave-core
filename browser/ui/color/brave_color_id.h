/* Copyright (c) 2022 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_BROWSER_UI_COLOR_BRAVE_COLOR_ID_H_
#define BRAVE_BROWSER_UI_COLOR_BRAVE_COLOR_ID_H_

#include "chrome/browser/ui/color/chrome_color_id.h"

// clang-format off

#define BRAVE_COMMON_COLOR_IDS                       \
    E_CPONLY(kColorForTest)                          \
    E_CPONLY(kColorIconBase)                         \
    E_CPONLY(kColorMenuItemSubText)                  \
    E_CPONLY(kColorBookmarkBarInstructionsText)      \
    E_CPONLY(kColorBookmarkBarInstructionsLink)      \
    E_CPONLY(kColorLocationBarFocusRing)             \
    E_CPONLY(kColorLocationBarHoveredShadow)         \
    E_CPONLY(kColorDialogDontAskAgainButton)         \
    E_CPONLY(kColorDialogDontAskAgainButtonHovered)  \
    E_CPONLY(kColorTabGroupBackgroundAlpha)          \
    E_CPONLY(kColorWebDiscoveryInfoBarBackground)    \
    E_CPONLY(kColorWebDiscoveryInfoBarMessage)       \
    E_CPONLY(kColorWebDiscoveryInfoBarLink)          \
    E_CPONLY(kColorWebDiscoveryInfoBarNoThanks)      \
    E_CPONLY(kColorWebDiscoveryInfoBarClose)         \
    E_CPONLY(kColorBraveDownloadToolbarButtonActive) \
    E_CPONLY(kColorToolbarButtonActivated)

#define BRAVE_SEARCH_CONVERSION_COLOR_IDS                             \
    E_CPONLY(kColorSearchConversionCloseButton)                       \
    E_CPONLY(kColorSearchConversionBannerTypeBackground)              \
    E_CPONLY(kColorSearchConversionBannerTypeBackgroundBorder)        \
    E_CPONLY(kColorSearchConversionBannerTypeBackgroundBorderHovered) \
    E_CPONLY(kColorSearchConversionBannerTypeBackgroundGradientFrom)  \
    E_CPONLY(kColorSearchConversionBannerTypeBackgroundGradientTo)    \
    E_CPONLY(kColorSearchConversionBannerTypeDescText)

#define BRAVE_SIDEBAR_COLOR_IDS                               \
    E_CPONLY(kColorSidebarPanelHeaderSeparator)               \
    E_CPONLY(kColorSidebarPanelHeaderBackground)              \
    E_CPONLY(kColorSidebarPanelHeaderTitle)                   \
    E_CPONLY(kColorSidebarPanelHeaderButton)                  \
    E_CPONLY(kColorSidebarPanelHeaderButtonHovered)

#define BRAVE_SHARED_PINNED_TAB_COLOR_IDS                    \
    E_CPONLY(kColorBraveSharedPinnedTabDummyViewBackground)  \
    E_CPONLY(kColorBraveSharedPinnedTabDummyViewTitle)       \
    E_CPONLY(kColorBraveSharedPinnedTabDummyViewDescription) \
    E_CPONLY(kColorBraveSharedPinnedTabDummyViewThumbnailBorder)

#define BRAVE_OMNIBOX_COLOR_IDS \
    E_CPONLY(kColorBraveOmniboxResultViewSeparator) \
    E_CPONLY(kColorBravePlayerActionViewBorder)

#define BRAVE_EXTENSION_MENU_COLOR_IDS \
    E_CPONLY(kColorBraveExtensionMenuIcon)

#define BRAVE_APP_MENU_COLOR_IDS \
    E_CPONLY(kColorBraveAppMenuAccentColor)

#define BRAVE_COLOR_IDS               \
    BRAVE_COMMON_COLOR_IDS            \
    BRAVE_SEARCH_CONVERSION_COLOR_IDS \
    BRAVE_SIDEBAR_COLOR_IDS           \
    BRAVE_SHARED_PINNED_TAB_COLOR_IDS \
    BRAVE_OMNIBOX_COLOR_IDS           \
    BRAVE_EXTENSION_MENU_COLOR_IDS    \
    BRAVE_APP_MENU_COLOR_IDS

#include "ui/color/color_id_macros.inc"

enum BraveColorIds : ui::ColorId {
  kBraveColorsStart = kChromeColorsEnd,

  BRAVE_COLOR_IDS

  kBraveColorsEnd,
};

#include "ui/color/color_id_macros.inc"  // NOLINT

// clang-format on

#endif  // BRAVE_BROWSER_UI_COLOR_BRAVE_COLOR_ID_H_

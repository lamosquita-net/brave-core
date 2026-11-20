/* Copyright (c) 2022 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/browser/ui/color/brave_color_mixer.h"

#include <cmath>
#include <optional>

#include "base/notreached.h"
#include "base/numerics/safe_conversions.h"
#include "brave/browser/ui/color/brave_color_id.h"
#include "brave/browser/ui/color/color_palette.h"
#include "brave/browser/ui/color/leo/colors.h"
#include "chrome/browser/themes/theme_properties.h"
#include "chrome/browser/ui/color/chrome_color_id.h"
#include "chrome/browser/ui/color/chrome_color_provider_utils.h"
#include "chrome/browser/ui/color/material_chrome_color_mixer.h"
#include "chrome/browser/ui/color/material_side_panel_color_mixer.h"
#include "ui/base/ui_base_features.h"
#include "ui/color/color_id.h"
#include "ui/color/color_provider.h"
#include "ui/color/color_provider_key.h"
#include "ui/color/color_recipe.h"
#include "ui/color/color_transform.h"
#include "ui/gfx/color_palette.h"
#include "ui/gfx/color_utils.h"
#include "ui/native_theme/native_theme.h"

namespace {

// Location bar colors
constexpr SkColor kPrivateLocationBarBgBase = kPrivateFrame;
constexpr SkColor kLightLocationBarBgBase = kFlyWebLocationBarBackground;
constexpr SkColor kDarkLocationBarBgBase =
    kFlyWebLightOmniboxInDarkMode ? kFlyWebLocationBarBackground
                                  : kFlyWebDarkLocationBarBackground;

// Copied from //chrome/browser/ui/omnibox//omnibox_theme.h
// As below values are not changed for several years, it would be safe to copy.
// Can't include //chrome/browser/ui/omnibox in here due to circular deps.
constexpr float kOmniboxOpacityHovered = 0.10f;
constexpr float kOmniboxOpacitySelected = 0.16f;

SkColor PickColorContrastingToToolbar(const ui::ColorProviderKey& key,
                                      const ui::ColorMixer& mixer,
                                      SkColor color1,
                                      SkColor color2) {
  // Custom theme's toolbar color will be the final color.
  auto toolbar_color = mixer.GetResultColor(kColorToolbar);
  SkColor custom_toolbar_color;
  if (key.custom_theme &&
      key.custom_theme->GetColor(ThemeProperties::COLOR_TOOLBAR,
                                 &custom_toolbar_color)) {
    toolbar_color = custom_toolbar_color;
  }
  return color_utils::PickContrastingColor(color1, color2, toolbar_color);
}

bool HasCustomToolbarColor(const ui::ColorProviderKey& key) {
  SkColor custom_toolbar_color;
  return key.custom_theme &&
         key.custom_theme->GetColor(ThemeProperties::COLOR_TOOLBAR,
                                    &custom_toolbar_color);
}

// FlyWeb tab text, regular windows only (called from
// AddBravifiedTabStripColorMixer(), which private windows skip because their
// theme supplier is a custom theme): black or white on
// the active tab in both window states; inactive tabs use it at
// kFlyWebInactiveTabTextAlpha over the tab strip, made opaque, as Chromium's
// tab code expects.
void AddFlyWebTabTextColorMixer(ui::ColorProvider* provider,
                                const ui::ColorProviderKey& key) {
  ui::ColorMixer& mixer = provider->AddMixer();
  mixer[kColorTabForegroundActiveFrameActive] = {
      key.color_mode == ui::ColorProviderKey::ColorMode::kDark
          ? kFlyWebDarkTabText
          : kFlyWebLightTabText};
  mixer[kColorTabForegroundActiveFrameInactive] = {
      kColorTabForegroundActiveFrameActive};
  mixer[kColorTabForegroundInactiveFrameActive] = ui::AlphaBlend(
      {kColorTabForegroundActiveFrameActive},
      {kColorTabBackgroundInactiveFrameActive}, kFlyWebInactiveTabTextAlpha);
  mixer[kColorTabForegroundInactiveFrameInactive] = ui::AlphaBlend(
      {kColorTabForegroundActiveFrameInactive},
      {kColorTabBackgroundInactiveFrameInactive}, kFlyWebInactiveTabTextAlpha);

  // Inactive tab under the pointer (Titín, 20-11), in both window states.
  mixer[kColorTabBackgroundInactiveHoverFrameActive] = {
      key.color_mode == ui::ColorProviderKey::ColorMode::kDark
          ? kFlyWebDarkInactiveTabHover
          : kFlyWebLightInactiveTabHover};
  mixer[kColorTabBackgroundInactiveHoverFrameInactive] = {
      kColorTabBackgroundInactiveHoverFrameActive};
}

void AddChromeLightThemeColorMixer(ui::ColorProvider* provider,
                                   const ui::ColorProviderKey& key) {
  ui::ColorMixer& mixer = provider->AddMixer();

  mixer[kColorTabThrobber] = {SkColorSetRGB(0xd7, 0x55, 0x26)};
  mixer[kColorBookmarkBarForeground] = {kColorTabForegroundActiveFrameActive};
  mixer[ui::kColorBadgeBackground] = {SkColorSetRGB(95, 92, 241)};
  mixer[ui::kColorBadgeForeground] = {SkColorSetRGB(245, 244, 254)};
  mixer[kColorForTest] = {kLightColorForTest};
  mixer[kColorNewTabButtonBackgroundFrameActive] = {ui::kColorFrameActive};
  mixer[kColorNewTabButtonBackgroundFrameInactive] = {ui::kColorFrameInactive};
  mixer[kColorNewTabPageBackground] = {kBraveNewTabBackgroundLight};
  mixer[kColorTabBackgroundInactiveFrameActive] = {ui::kColorFrameActive};
  mixer[kColorTabBackgroundInactiveFrameInactive] = {ui::kColorFrameInactive};
  mixer[kColorTabForegroundActiveFrameActive] = {
      leo::GetColor(leo::Color::kColorTextPrimary, leo::Theme::kLight)};
  mixer[kColorTabForegroundActiveFrameInactive] = {
      leo::GetColor(leo::Color::kColorTextPrimary, leo::Theme::kLight)};
  mixer[kColorTabForegroundInactiveFrameActive] = {
      leo::GetColor(leo::Color::kColorTextSecondary, leo::Theme::kLight)};
  mixer[kColorTabStrokeFrameActive] = {SkColorSetA(SK_ColorBLACK, 0.07 * 255)};
  mixer[kColorTabStrokeFrameInactive] = {kColorTabStrokeFrameActive};
  mixer[kColorToolbar] = {kFlyWebLightToolbar};
  mixer[kColorToolbarButtonIcon] = {kFlyWebLightToolbarIcon};
  mixer[kColorToolbarButtonIconInactive] = {
      ui::SetAlpha(kColorToolbarButtonIcon, kBraveDisabledControlAlpha)};
  mixer[kColorToolbarContentAreaSeparator] = {ui::kColorFrameActive};
  mixer[kColorToolbarTopSeparatorFrameActive] = {kColorToolbar};
  mixer[kColorToolbarTopSeparatorFrameInactive] = {kColorToolbar};
  mixer[ui::kColorFrameActive] = {kLightFrame};
  // FlyWeb: the window without focus is a little dimmer, with Chromium's
  // recipe for the inactive header (ui/color/sys_color_mixer.cc).
  mixer[ui::kColorFrameInactive] =
      ui::AlphaBlend({kLightFrame}, {ui::kColorRefNeutral98}, 0x48);
  mixer[ui::kColorToggleButtonThumbOff] = {SK_ColorWHITE};
  mixer[ui::kColorToggleButtonThumbOn] = {SkColorSetRGB(0x4C, 0x54, 0xD2)};
  mixer[ui::kColorToggleButtonTrackOff] = {SkColorSetRGB(0xDA, 0xDC, 0xE8)};
  mixer[ui::kColorToggleButtonTrackOn] = {SkColorSetRGB(0xE1, 0xE2, 0xF6)};

  // Used for download button progress ring color.
  mixer[kColorDownloadToolbarButtonActive] = {SkColorSetRGB(0x42, 0x3E, 0xEE)};
  mixer[kColorDownloadToolbarButtonRingBackground] = {
      SkColorSetARGB(0x0F, 0x1D, 0x1F, 0x25)};

  mixer[kColorTabCloseButtonFocusRingActive] = {
      ui::kColorFocusableBorderFocused};
  mixer[kColorTabCloseButtonFocusRingInactive] = {
      ui::kColorFocusableBorderFocused};
  mixer[kColorTabFocusRingActive] = {ui::kColorFocusableBorderFocused};
  mixer[kColorTabFocusRingInactive] = {ui::kColorFocusableBorderFocused};

  // Upstream uses tab's background color as omnibox chip background color.
  // In our light mode, there is no difference between location bar's bg
  // color and tab's bg color. So, it looks like chip's bg color is transparent.
  // Use frame color as chip background to have different bg color.
  mixer[kColorOmniboxChipBackground] = {kLightFrame};
}

void AddChromeDarkThemeColorMixer(ui::ColorProvider* provider,
                                  const ui::ColorProviderKey& key) {
  ui::ColorMixer& mixer = provider->AddMixer();

  mixer[kColorTabThrobber] = {SkColorSetRGB(0xd7, 0x55, 0x26)};
  mixer[kColorBookmarkBarForeground] = {kColorTabForegroundActiveFrameActive};
  mixer[ui::kColorBadgeBackground] = {SkColorSetRGB(135, 132, 244)};
  mixer[ui::kColorBadgeForeground] = {SkColorSetRGB(14, 14, 52)};
  mixer[kColorForTest] = {kDarkColorForTest};
  mixer[kColorNewTabButtonBackgroundFrameActive] = {ui::kColorFrameActive};
  mixer[kColorNewTabButtonBackgroundFrameInactive] = {ui::kColorFrameInactive};
  mixer[kColorNewTabPageBackground] = {kBraveNewTabBackgroundDark};
  mixer[kColorTabBackgroundInactiveFrameActive] = {ui::kColorFrameActive};
  mixer[kColorTabBackgroundInactiveFrameInactive] = {ui::kColorFrameInactive};
  mixer[kColorTabForegroundActiveFrameActive] = {
      leo::GetColor(leo::Color::kColorTextPrimary, leo::Theme::kDark)};
  mixer[kColorTabForegroundActiveFrameInactive] = {
      leo::GetColor(leo::Color::kColorTextPrimary, leo::Theme::kDark)};
  mixer[kColorTabForegroundInactiveFrameActive] = {
      leo::GetColor(leo::Color::kColorTextSecondary, leo::Theme::kDark)};
  mixer[kColorTabStrokeFrameActive] = {kColorToolbar};
  mixer[kColorTabStrokeFrameInactive] = {kColorToolbar};
  mixer[kColorToolbar] = {kFlyWebDarkToolbar};
  mixer[kColorToolbarButtonIcon] = {kFlyWebDarkToolbarIcon};
  mixer[kColorToolbarButtonIconInactive] = {
      ui::SetAlpha(kColorToolbarButtonIcon, kBraveDisabledControlAlpha)};
  mixer[kColorToolbarContentAreaSeparator] = {kColorToolbar};
  mixer[kColorToolbarTopSeparatorFrameActive] = {kColorToolbar};
  mixer[kColorToolbarTopSeparatorFrameInactive] = {kColorToolbar};
  mixer[ui::kColorFrameActive] = {kDarkFrame};
  // FlyWeb: dimmer without focus, Chromium's recipe (see the light mixer).
  mixer[ui::kColorFrameInactive] =
      ui::AlphaBlend({kDarkFrame}, {ui::kColorRefNeutral25}, 0x99);
  mixer[ui::kColorToggleButtonThumbOff] = {SK_ColorWHITE};
  mixer[ui::kColorToggleButtonThumbOn] = {SkColorSetRGB(0x44, 0x36, 0xE1)};
  mixer[ui::kColorToggleButtonTrackOff] = {SkColorSetRGB(0x5E, 0x61, 0x75)};
  mixer[ui::kColorToggleButtonTrackOn] = {SkColorSetRGB(0x76, 0x79, 0xB1)};
  mixer[kColorDownloadToolbarButtonActive] = {SkColorSetRGB(0x87, 0x84, 0xF4)};
  mixer[kColorDownloadToolbarButtonRingBackground] = {
      SkColorSetARGB(0x33, 0x16, 0x17, 0x1D)};

  mixer[kColorTabCloseButtonFocusRingActive] = {
      ui::kColorFocusableBorderFocused};
  mixer[kColorTabCloseButtonFocusRingInactive] = {
      ui::kColorFocusableBorderFocused};
  mixer[kColorTabFocusRingActive] = {ui::kColorFocusableBorderFocused};
  mixer[kColorTabFocusRingInactive] = {ui::kColorFocusableBorderFocused};
}

void AddChromeColorMixerForAllThemes(ui::ColorProvider* provider,
                                     const ui::ColorProviderKey& key) {
  ui::ColorMixer& mixer = provider->AddMixer();

  // Use same ink drop effect for all themes including custome themes.
  // Toolbar button's inkdrop highlight/visible colors depends on toolbar color.
  auto get_toolbar_ink_drop_color = [](float dark_opacity, float light_opacity,
                                       SkColor input,
                                       const ui::ColorMixer& mixer) {
    const float highlight_opacity =
        color_utils::IsDark(mixer.GetResultColor(kColorToolbar))
            ? dark_opacity
            : light_opacity;
    return SkColorSetA(SK_ColorBLACK, 0xFF * highlight_opacity);
  };
  mixer[kColorToolbarInkDropHover] = {
      base::BindRepeating(get_toolbar_ink_drop_color, 0.25f, 0.05f)};
  mixer[kColorToolbarInkDropRipple] = {
      base::BindRepeating(get_toolbar_ink_drop_color, 0.4f, 0.1f)};

  if (key.custom_theme) {
    return;
  }

  // FlyWeb: bookmark folders in the same flat color as the toolbar icons.
  mixer[kColorBookmarkFolderIcon] = {kColorToolbarButtonIcon};

  mixer[kColorLocationBarBackground] = {kColorToolbarBackgroundSubtleEmphasis};
  mixer[kColorLocationBarBackgroundHovered] = {kColorLocationBarBackground};

  // We don't show border when omnibox doesn't have focus but still
  // contains in-progress user input.
  mixer[kColorLocationBarBorderOnMismatch] = {SK_ColorTRANSPARENT};
}

void AddBraveColorMixerForAllThemes(ui::ColorProvider* provider,
                                    const ui::ColorProviderKey& key) {
  ui::ColorMixer& mixer = provider->AddMixer();

  // Custom theme will use this color. Other themes could have another
  // color by their color mixers.
  mixer[kColorToolbarButtonActivated] = {SkColorSetRGB(0x7C, 0x91, 0xFF)};
}

void AddBraveOmniboxLightThemeColorMixer(ui::ColorProvider* provider,
                                         const ui::ColorProviderKey& key) {
  ui::ColorMixer& mixer = provider->AddMixer();

  // Apply bravified color when there is no custom theme.
  mixer[kColorToolbarBackgroundSubtleEmphasis] = {GetLocationBarBackground(
      /*dark*/ false, /*private*/ false)};
  // Use same color for normal & hover location bar background.
  // Instead, shadow is set when hovered.
  mixer[kColorToolbarBackgroundSubtleEmphasisHovered] = {
      kColorToolbarBackgroundSubtleEmphasis};
  mixer[kColorOmniboxText] = {kLightOmniboxText};

  mixer[kColorOmniboxResultsBackground] = {GetOmniboxResultBackground(
      kColorOmniboxResultsBackground, /*dark*/ false, /*incognito*/ false)};
  mixer[kColorOmniboxResultsBackgroundHovered] = {
      GetOmniboxResultBackground(kColorOmniboxResultsBackgroundHovered,
                                 /*dark*/ false, /*incognito*/ false)};
  mixer[kColorOmniboxResultsBackgroundSelected] = {
      GetOmniboxResultBackground(kColorOmniboxResultsBackgroundSelected,
                                 /*dark*/ false, /*incognito*/ false)};
  mixer[kColorOmniboxResultsFocusIndicator] = {
      ui::kColorFocusableBorderFocused};
  mixer[kColorOmniboxResultsUrl] = {
      leo::GetColor(leo::Color::kColorTextInteractive, leo::Theme::kLight)};
  mixer[kColorOmniboxResultsUrlSelected] = {kColorOmniboxResultsUrl};
}

// FlyWeb: unused since the dark omnibox of 3.2.0 (Brave's dark field).
[[maybe_unused]] void AddBraveOmniboxDarkThemeColorMixer(
    ui::ColorProvider* provider,
    const ui::ColorProviderKey& key) {
  ui::ColorMixer& mixer = provider->AddMixer();

  // Apply bravified color when there is no custom theme.
  mixer[kColorToolbarBackgroundSubtleEmphasis] = {GetLocationBarBackground(
      /*dark*/ true, /*private*/ false)};
  mixer[kColorToolbarBackgroundSubtleEmphasisHovered] = {
      kColorToolbarBackgroundSubtleEmphasis};
  mixer[kColorOmniboxText] = {kDarkOmniboxText};

  mixer[kColorOmniboxResultsBackground] = {GetOmniboxResultBackground(
      kColorOmniboxResultsBackground, /*dark*/ true, /*incognito*/ false)};
  mixer[kColorOmniboxResultsBackgroundHovered] = {
      GetOmniboxResultBackground(kColorOmniboxResultsBackgroundHovered,
                                 /*dark*/ true, /*incognito*/ false)};
  mixer[kColorOmniboxResultsBackgroundSelected] = {
      GetOmniboxResultBackground(kColorOmniboxResultsBackgroundSelected,
                                 /*dark*/ true, /*incognito*/ false)};
  mixer[kColorOmniboxResultsFocusIndicator] = {
      ui::kColorFocusableBorderFocused};
  mixer[kColorOmniboxResultsUrl] = {
      leo::GetColor(leo::Color::kColorTextInteractive, leo::Theme::kDark)};
  mixer[kColorOmniboxResultsUrlSelected] = {kColorOmniboxResultsUrl};
}

// FlyWeb 3.2.0: dark mode URL field and popup in the dark range (Titín,
// 20-11, maqueta-oscuro-desplegable-hd.webp). Sets the backgrounds and the
// text; AddFlyWebLightOmniboxOnDarkColorMixer() then derives the field's
// other colors from them, and AddFlyWebDarkOmniboxPopupColorMixer() sets the
// popup's own.
void AddFlyWebDarkOmniboxColorMixer(ui::ColorProvider* provider,
                                    const ui::ColorProviderKey& key) {
  ui::ColorMixer& mixer = provider->AddMixer();
  mixer[kColorToolbarBackgroundSubtleEmphasis] = {
      kFlyWebDarkLocationBarBackground};
  mixer[kColorToolbarBackgroundSubtleEmphasisHovered] = {
      kColorToolbarBackgroundSubtleEmphasis};
  mixer[kColorOmniboxText] = {kFlyWebDarkOmniboxText};
  mixer[kColorOmniboxResultsBackground] = {GetOmniboxResultBackground(
      kColorOmniboxResultsBackground, /*dark*/ true, /*incognito*/ false)};
  mixer[kColorOmniboxResultsBackgroundHovered] = {
      kFlyWebDarkOmniboxResultsHovered};
  mixer[kColorOmniboxResultsBackgroundSelected] = {
      kFlyWebDarkOmniboxResultsSelected};
}

// The popup on #767676 with white text (Titín, 20-11: 4.5:1, AA; the
// mockup's #909294 gave 3.1:1). The secondary text, the URLs and the icons
// are white too, 4.5:1 on the popup and 5.4:1 on a hovered row. The
// selected row (#542671) is white on purple, 11:1.
void AddFlyWebDarkOmniboxPopupColorMixer(ui::ColorProvider* provider,
                                         const ui::ColorProviderKey& key) {
  ui::ColorMixer& mixer = provider->AddMixer();
  mixer[kColorOmniboxResultsTextSelected] = {kFlyWebDarkOmniboxText};
  mixer[kColorOmniboxResultsTextDimmed] = {kFlyWebDarkOmniboxText};
  mixer[kColorOmniboxResultsTextDimmedSelected] = {kFlyWebDarkOmniboxText};
  mixer[kColorOmniboxResultsUrl] = {kFlyWebDarkOmniboxText};
  mixer[kColorOmniboxResultsUrlSelected] = {kFlyWebDarkOmniboxText};
  mixer[kColorOmniboxResultsTextAnswer] = {kFlyWebDarkOmniboxText};
  mixer[kColorOmniboxKeywordSelected] = {kFlyWebDarkOmniboxText};
  mixer[kColorOmniboxResultsIcon] = {kFlyWebDarkOmniboxText};
  mixer[kColorOmniboxResultsIconSelected] = {kFlyWebDarkOmniboxText};
  mixer[kColorOmniboxResultsFocusIndicator] = {
      kFlyWebDarkOmniboxFocusIndicator};
}

// FlyWeb: dark mode with a white URL field and popup
// (kFlyWebLightOmniboxInDarkMode). Chromium's CR2023 omnibox colors come from
// the ui::kColorSys* tokens, which are meant for a dark field in dark mode.
// Use instead Chromium's own contrast-based recipes (omnibox_color_mixer.cc,
// the path for custom themes), so every color follows the field and popup
// backgrounds and the text color set by the light omnibox mixer.
void AddFlyWebLightOmniboxOnDarkColorMixer(ui::ColorProvider* provider,
                                           const ui::ColorProviderKey& key) {
  ui::ColorMixer& mixer = provider->AddMixer();
  const float contrast_ratio = color_utils::kMinimumReadableContrastRatio;

  // Text.
  mixer[kColorOmniboxResultsTextSelected] = {kColorOmniboxText};
  mixer[kColorOmniboxContextEntrypointText] =
      ui::GetColorWithMaxContrast(kColorToolbarBackgroundSubtleEmphasis);
  const auto blend_with_clamped_contrast =
      [contrast_ratio](ui::ColorId foreground_id, ui::ColorId background_id) {
        return ui::BlendForMinContrast(
            foreground_id, foreground_id,
            ui::BlendForMinContrast(background_id, background_id, std::nullopt,
                                    contrast_ratio),
            contrast_ratio);
      };
  mixer[kColorOmniboxTextDimmed] = blend_with_clamped_contrast(
      kColorOmniboxText, kColorToolbarBackgroundSubtleEmphasisHovered);
  mixer[kColorOmniboxResultsTextDimmed] = blend_with_clamped_contrast(
      kColorOmniboxText, kColorOmniboxResultsBackgroundHovered);
  mixer[kColorOmniboxResultsTextDimmedSelected] = blend_with_clamped_contrast(
      kColorOmniboxResultsTextSelected, kColorOmniboxResultsBackgroundSelected);
  mixer[kColorOmniboxResultsTextAnswer] = {kColorOmniboxResultsUrl};
  mixer[kColorOmniboxForegroundDisabled] = {kColorOmniboxTextDimmed};
  mixer[kColorOmniboxKeywordSelected] = {kColorOmniboxResultsUrl};
  mixer[kColorOmniboxKeywordSeparator] = {kColorOmniboxText};

  // Text selection. A dark field (private windows) keeps Chromium's colors.
  mixer[kColorOmniboxSelectionBackground] = ui::SelectBasedOnDarkInput(
      kColorToolbarBackgroundSubtleEmphasis, ui::kColorSysStateTextHighlight,
      kFlyWebOmniboxSelectionOnLight);
  mixer[kColorOmniboxSelectionForeground] = ui::SelectBasedOnDarkInput(
      kColorToolbarBackgroundSubtleEmphasis, ui::kColorSysStateOnTextHighlight,
      ui::GetColorWithMaxContrast(kColorOmniboxSelectionBackground));

  // Popup.
  mixer[kColorOmniboxResultsBackgroundIph] = {
      kColorOmniboxResultsBackgroundHovered};
  mixer[kColorOmniboxResultsBackgroundHoverOverlay] = {
      ui::SetAlpha(ui::GetColorWithMaxContrast(kColorOmniboxResultsBackground),
                   std::ceil(0.08f * 255.0f))};
  mixer[kColorOmniboxResultsFocusIndicator] = {
      ui::kColorFocusableBorderFocused};

  // Icons.
  const auto results_icon = [contrast_ratio](ui::ColorId text_id,
                                             ui::ColorId background_id) {
    return ui::BlendForMinContrast(ui::DeriveDefaultIconColor(text_id),
                                   background_id, std::nullopt, contrast_ratio);
  };
  mixer[kColorOmniboxResultsIcon] =
      results_icon(kColorOmniboxText, kColorOmniboxResultsBackground);
  mixer[kColorOmniboxResultsIconSelected] = results_icon(
      kColorOmniboxResultsTextSelected, kColorOmniboxResultsBackgroundSelected);
  mixer[kColorOmniboxResultsIconHovered] =
      ui::SetAlpha({kColorOmniboxResultsIcon}, std::ceil(0.128f * 255.0f));
  mixer[kColorOmniboxResultsStarterPackIcon] = ui::BlendForMinContrast(
      gfx::kGoogleBlue600, kColorOmniboxResultsBackground, std::nullopt,
      color_utils::kMinimumVisibleContrastRatio);
  mixer[kColorOmniboxActionIcon] = {kColorOmniboxResultsIcon};
  mixer[kColorOmniboxActionIconHover] = {kColorOmniboxIconHover};
  mixer[kColorOmniboxIconBackgroundTonal] = {SK_ColorTRANSPARENT};
  mixer[kColorOmniboxIconForeground] =
      ui::DeriveDefaultIconColor(kColorOmniboxText);
  mixer[kColorOmniboxIconForegroundTonal] = {kColorOmniboxIconForeground};
  mixer[kColorOmniboxIconPressed] = {
      ui::SetAlpha(kColorOmniboxText, std::ceil(0.16f * 255.0f))};

  // Suggestion chips (action buttons).
  mixer[kColorOmniboxResultsButtonInkDrop] =
      ui::GetColorWithMaxContrast(kColorOmniboxResultsBackgroundHovered);
  mixer[kColorOmniboxResultsButtonInkDropSelected] =
      ui::GetColorWithMaxContrast(kColorOmniboxResultsBackgroundSelected);
  mixer[kColorOmniboxResultsButtonInkDropRowHovered] = {ui::SetAlpha(
      kColorOmniboxResultsButtonInkDrop, std::ceil(0.10f * 255.0f))};
  mixer[kColorOmniboxResultsButtonInkDropSelectedRowHovered] = {ui::SetAlpha(
      kColorOmniboxResultsButtonInkDrop, std::ceil(0.16f * 255.0f))};
  mixer[kColorOmniboxResultsButtonInkDropRowSelected] = {ui::SetAlpha(
      kColorOmniboxResultsButtonInkDropSelected, std::ceil(0.10f * 255.0f))};
  mixer[kColorOmniboxResultsButtonInkDropSelectedRowSelected] = {ui::SetAlpha(
      kColorOmniboxResultsButtonInkDropSelected, std::ceil(0.16f * 255.0f))};

  // Security chip.
  mixer[kColorOmniboxSecurityChipDangerousBackground] =
      ui::SelectBasedOnDarkInput(kColorOmniboxResultsBackground,
                                 gfx::kGoogleRed300, gfx::kGoogleRed800);
  mixer[kColorOmniboxSecurityChipText] = ui::SelectBasedOnDarkInput(
      kColorOmniboxSecurityChipDangerousBackground,
      ui::GetColorWithMaxContrast(kColorOmniboxSecurityChipDangerousBackground),
      gfx::kGoogleRed800);
  mixer[kColorOmniboxSecurityChipInkDropHover] = {
      ui::SetAlpha(kColorOmniboxSecurityChipText, std::ceil(0.10f * 255.0f))};
  mixer[kColorOmniboxSecurityChipInkDropRipple] = {
      ui::SetAlpha(kColorOmniboxSecurityChipText, std::ceil(0.16f * 255.0f))};
}

}  // namespace

SkColor GetLocationBarBackground(bool dark, bool priv) {
  if (priv) {
    return kPrivateLocationBarBgBase;
  }

  if (dark) {
    return kDarkLocationBarBgBase;
  }

  return kLightLocationBarBgBase;
}

// Omnibox result bg colors
SkColor GetOmniboxResultBackground(int id, bool dark, bool priv) {
#if !defined(USE_AURA)
  ui::NativeTheme* native_theme = nullptr;
#else
  // For high contrast, selected rows use inverted colors to stand out more.
  ui::NativeTheme* native_theme = ui::NativeTheme::GetInstanceForNativeUi();
#endif  // !defined(USE_AURA)
  bool high_contrast =
      native_theme && native_theme->preferred_contrast() ==
                          ui::NativeTheme::PreferredContrast::kMore;
  float omnibox_opacity = 0.0f;
  if (id == kColorOmniboxResultsBackgroundHovered) {
    omnibox_opacity = kOmniboxOpacityHovered;
  } else if (id == kColorOmniboxResultsBackgroundSelected) {
    omnibox_opacity = kOmniboxOpacitySelected;
  }

  SkColor color;
  if (priv) {
    color = high_contrast ? color_utils::HSLShift(kPrivateLocationBarBgBase,
                                                  {-1, -1, 0.45})
                          : kPrivateLocationBarBgBase;
  } else if (dark && !kFlyWebLightOmniboxInDarkMode) {
    // FlyWeb 3.2.0: the dark popup is lighter than the dark field (Titín,
    // 20-11).
    color = high_contrast ? gfx::kGoogleGrey900
                          : kFlyWebDarkOmniboxResultsBackground;
  } else {
    color = kFlyWebLocationBarBackground;
  }
  return color_utils::BlendTowardMaxContrast(
      color, base::ClampRound(omnibox_opacity * 0xff));
}

void AddBravifiedChromeThemeColorMixer(ui::ColorProvider* provider,
                                       const ui::ColorProviderKey& key) {
  AddChromeColorMixerForAllThemes(provider, key);

  AddMaterialChromeColorMixer(provider, key);
  AddMaterialSidePanelColorMixer(provider, key);

  // TODO(simonhong): Use leo color when it's ready.
  // TODO(simonhong): Move these overriding to
  // AddChromeColorMixerForAllThemes().
  ui::ColorMixer& mixer = provider->AddMixer();
  mixer[kColorAppMenuHighlightSeverityLow] = {
      PickColorContrastingToToolbar(key, mixer, SkColorSetRGB(0x00, 0x46, 0x07),
                                    SkColorSetRGB(0x58, 0xE1, 0x55))};
  mixer[kColorAppMenuHighlightSeverityMedium] = {
      PickColorContrastingToToolbar(key, mixer, SkColorSetRGB(0x4A, 0x39, 0x00),
                                    SkColorSetRGB(0xF1, 0xC0, 0x0F))};
  mixer[kColorAppMenuHighlightSeverityHigh] = {
      PickColorContrastingToToolbar(key, mixer, SkColorSetRGB(0x7D, 0x00, 0x1A),
                                    SkColorSetRGB(0xFF, 0xB3, 0xB2))};

  if (key.custom_theme) {
    return;
  }

  key.color_mode == ui::ColorProviderKey::ColorMode::kDark
      ? AddChromeDarkThemeColorMixer(provider, key)
      : AddChromeLightThemeColorMixer(provider, key);
}

void AddBraveLightThemeColorMixer(ui::ColorProvider* provider,
                                  const ui::ColorProviderKey& key) {
  ui::ColorMixer& mixer = provider->AddMixer();

  mixer[kColorForTest] = {kLightColorForTest};

  mixer[kColorIconBase] = {SkColorSetRGB(0x49, 0x50, 0x57)};
  mixer[kColorBookmarkBarInstructionsText] = {
      PickColorContrastingToToolbar(key, mixer, SkColorSetRGB(0x49, 0x50, 0x57),
                                    SkColorSetRGB(0xFF, 0xFF, 0xFF))};
  mixer[kColorBookmarkBarInstructionsLink] = {PickColorContrastingToToolbar(
      key, mixer, leo::light::kColorTextInteractive,
      leo::dark::kColorTextInteractive)};

  if (!HasCustomToolbarColor(key)) {
    mixer[kColorToolbarButtonActivated] = {
        leo::GetColor(leo::Color::kColorIconInteractive, leo::Theme::kLight)};
  }

  mixer[kColorLocationBarHoveredShadow] = {
      SkColorSetA(SK_ColorBLACK, 0.07 * 255)};

  // Colors for HelpBubble. IDs are defined in
  // chrome/browser/ui/color/chrome_color_id.h
  mixer[kColorFeaturePromoBubbleBackground] = {SK_ColorWHITE};
  mixer[kColorFeaturePromoBubbleForeground] = {SkColorSetRGB(0x42, 0x45, 0x52)};
  mixer[kColorFeaturePromoBubbleCloseButtonInkDrop] = {
      kColorToolbarInkDropHover};

  mixer[kColorBraveExtensionMenuIcon] = {
      leo::GetColor(leo::Color::kColorIconActive, leo::Theme::kLight)};

}

void AddBraveDarkThemeColorMixer(ui::ColorProvider* provider,
                                 const ui::ColorProviderKey& key) {
  ui::ColorMixer& mixer = provider->AddMixer();

  mixer[kColorForTest] = {kDarkColorForTest};

  mixer[kColorIconBase] = {SkColorSetRGB(0xC2, 0xC4, 0xCF)};
  mixer[kColorBookmarkBarInstructionsText] = {
      PickColorContrastingToToolbar(key, mixer, SkColorSetRGB(0x49, 0x50, 0x57),
                                    SkColorSetRGB(0xFF, 0xFF, 0xFF))};
  mixer[kColorBookmarkBarInstructionsLink] = {PickColorContrastingToToolbar(
      key, mixer, leo::light::kColorTextInteractive,
      leo::dark::kColorTextInteractive)};

  if (!HasCustomToolbarColor(key)) {
    mixer[kColorToolbarButtonActivated] = {
        leo::GetColor(leo::Color::kColorIconInteractive, leo::Theme::kDark)};
  }

  mixer[kColorLocationBarHoveredShadow] = {
      SkColorSetA(SK_ColorBLACK, 0.4 * 255)};

  // Colors for HelpBubble. IDs are defined in
  // chrome/browser/ui/color/chrome_color_id.h
  mixer[kColorFeaturePromoBubbleBackground] = {SkColorSetRGB(0x12, 0x13, 0x16)};
  mixer[kColorFeaturePromoBubbleForeground] = {SkColorSetRGB(0xC6, 0xC8, 0xD0)};
  mixer[kColorFeaturePromoBubbleCloseButtonInkDrop] = {
      kColorToolbarInkDropHover};

  mixer[kColorBraveExtensionMenuIcon] = {
      leo::GetColor(leo::Color::kColorIconActive, leo::Theme::kDark)};

}

// Handling dark or light theme on normal profile.
void AddBraveThemeColorMixer(ui::ColorProvider* provider,
                             const ui::ColorProviderKey& key) {
  AddBraveColorMixerForAllThemes(provider, key);

  key.color_mode == ui::ColorProviderKey::ColorMode::kDark
      ? AddBraveDarkThemeColorMixer(provider, key)
      : AddBraveLightThemeColorMixer(provider, key);
}

void AddBravePrivateThemeColorMixer(ui::ColorProvider* provider,
                                    const ui::ColorProviderKey& key) {
  AddBraveDarkThemeColorMixer(provider, key);

  // Add private theme specific brave colors here.
  ui::ColorMixer& mixer = provider->AddMixer();
  mixer[kColorForTest] = {kPrivateColorForTest};

  mixer[kColorToolbarButtonActivated] = {SkColorSetRGB(0x7C, 0x91, 0xFF)};
  mixer[kColorTabDividerFrameInactive] = {SkColorSetRGB(0x3F, 0x32, 0x56)};
  mixer[kColorTabDividerFrameActive] = {SkColorSetRGB(0x3F, 0x32, 0x56)};
}

void AddPrivateThemeColorMixer(ui::ColorProvider* provider,
                               const ui::ColorProviderKey& key) {
  AddBravePrivateThemeColorMixer(provider, key);
  AddChromeDarkThemeColorMixer(provider, key);

  ui::ColorMixer& mixer = provider->AddMixer();

  mixer[kColorBookmarkBarForeground] = {SkColorSetRGB(0xFF, 0xFF, 0xFF)};
  mixer[kColorLocationBarFocusRing] = {SkColorSetRGB(0xC6, 0xB3, 0xFF)};
  mixer[kColorNewTabButtonBackgroundFrameActive] = {ui::kColorFrameActive};
  mixer[kColorNewTabButtonBackgroundFrameInactive] = {ui::kColorFrameInactive};
  mixer[kColorNewTabPageBackground] = {kPrivateFrame};
  // FlyWeb: the active tab and the toolbar are one piece of the same color.
  mixer[kColorTabBackgroundActiveFrameActive] = {kColorToolbar};
  mixer[kColorTabBackgroundActiveFrameInactive] = {
      kColorTabBackgroundActiveFrameActive};
  mixer[kColorTabBackgroundInactiveFrameActive] = {ui::kColorFrameActive};
  mixer[kColorTabBackgroundInactiveFrameInactive] = {ui::kColorFrameInactive};

  // TODO(simonhong): Get color from leo when it's available.
  mixer[kColorTabForegroundActiveFrameActive] = {
      SkColorSetRGB(0xF5, 0xF3, 0xFF)};
  mixer[kColorTabForegroundActiveFrameInactive] = {
      SkColorSetRGB(0xCC, 0xBE, 0xFE)};
  mixer[kColorTabForegroundInactiveFrameActive] = {
      SkColorSetRGB(0xCC, 0xBE, 0xFE)};
  mixer[kColorToolbar] = {kFlyWebPrivateDarkToolbar};
  mixer[kColorToolbarButtonIcon] = {leo::kColorPrimitivePrivateWindow40};
  mixer[kColorToolbarButtonIconInactive] = {
      ui::SetAlpha(kColorToolbarButtonIcon, kBraveDisabledControlAlpha)};
  mixer[kColorToolbarContentAreaSeparator] = {kColorToolbar};
  mixer[ui::kColorFrameActive] = {kFlyWebPrivateFrame};
  mixer[ui::kColorFrameInactive] = {kFlyWebPrivateFrame};
}

void AddBraveOmniboxPrivateThemeColorMixer(ui::ColorProvider* provider,
                                           const ui::ColorProviderKey& key) {
  ui::ColorMixer& mixer = provider->AddMixer();

  mixer[kColorToolbarBackgroundSubtleEmphasis] = {GetLocationBarBackground(
      /*dark*/ false, /*private*/ true)};
  mixer[kColorToolbarBackgroundSubtleEmphasisHovered] = {
      kColorToolbarBackgroundSubtleEmphasis};
  mixer[kColorOmniboxText] = {kDarkOmniboxText};

  mixer[kColorOmniboxResultsBackground] = {GetOmniboxResultBackground(
      kColorOmniboxResultsBackground, /*dark*/ false, /*incognito*/ true)};
  mixer[kColorOmniboxResultsBackgroundHovered] = {
      GetOmniboxResultBackground(kColorOmniboxResultsBackgroundHovered,
                                 /*dark*/ false, /*incognito*/ true)};
  mixer[kColorOmniboxResultsBackgroundSelected] = {
      GetOmniboxResultBackground(kColorOmniboxResultsBackgroundSelected,
                                 /*dark*/ false, /*incognito*/ true)};
}

void AddBraveOmniboxColorMixer(ui::ColorProvider* provider,
                               const ui::ColorProviderKey& key) {
  ui::ColorMixer& mixer = provider->AddMixer();

  // Re-apply non-material color.
  mixer[kColorOmniboxResultsButtonBorder] = ui::BlendTowardMaxContrast(
      kColorToolbarBackgroundSubtleEmphasis, gfx::kGoogleGreyAlpha400);
  mixer[kColorOmniboxResultsButtonIcon] = {kColorOmniboxResultsIcon};
  mixer[kColorOmniboxResultsButtonIconSelected] = {
      kColorOmniboxResultsIconSelected};
  mixer[kColorOmniboxIconHover] = {
      ui::SetAlpha(kColorOmniboxText, std::ceil(0.10f * 255.0f))};

  // We don't use bg color for location icon view.
  mixer[kColorOmniboxIconBackground] = {SK_ColorTRANSPARENT};

  if (key.custom_theme) {
    return;
  }

  const bool dark = key.color_mode == ui::ColorProviderKey::ColorMode::kDark;
  AddBraveOmniboxLightThemeColorMixer(provider, key);
  if (dark) {
    if (!kFlyWebLightOmniboxInDarkMode) {
      AddFlyWebDarkOmniboxColorMixer(provider, key);
    }
    AddFlyWebLightOmniboxOnDarkColorMixer(provider, key);
    if (!kFlyWebLightOmniboxInDarkMode) {
      AddFlyWebDarkOmniboxPopupColorMixer(provider, key);
    }
  }
}

void AddBravifiedTabStripColorMixer(ui::ColorProvider* provider,
                                    const ui::ColorProviderKey& key) {
  if (key.custom_theme) {
    return;
  }

  ui::ColorMixer& mixer = provider->AddMixer();

  mixer[kColorNewTabButtonFocusRing] = {ui::kColorFocusableBorderFocused};
  // FlyWeb: the active tab and the toolbar are one piece of the same color.
  mixer[kColorTabBackgroundActiveFrameActive] = {kColorToolbar};
  mixer[kColorTabBackgroundActiveFrameInactive] = {
      kColorTabBackgroundActiveFrameActive};

  auto divider_color = ui::AlphaBlend(
      {leo::GetColor(leo::Color::kColorDividerSubtle,
                     key.color_mode == ui::ColorProviderKey::ColorMode::kDark
                         ? leo::Theme::kDark
                         : leo::Theme::kLight)},
      {kColorTabBackgroundInactiveFrameActive}, 0.75 * 0xff);
  mixer[kColorTabDividerFrameInactive] = {divider_color};
  mixer[kColorTabDividerFrameActive] = {divider_color};

  // FlyWeb tab text. It goes here, not with the other FlyWeb colors in
  // AddBravifiedChromeThemeColorMixer(), because Chromium's
  // AddTabStripColorMixer() runs after the chrome mixers and recomputes the
  // tab foregrounds from kColorToolbarText.
  AddFlyWebTabTextColorMixer(provider, key);
}

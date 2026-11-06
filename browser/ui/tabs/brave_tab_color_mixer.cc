/* Copyright (c) 2023 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "brave/browser/ui/tabs/brave_tab_color_mixer.h"

#include "brave/browser/ui/color/brave_color_id.h"
#include "brave/browser/ui/color/leo/colors.h"
#include "ui/color/color_mixer.h"
#include "ui/color/color_provider.h"
#include "ui/color/color_provider_key.h"

namespace tabs {

void AddBraveTabLightThemeColorMixer(ui::ColorProvider* provider,
                                     const ui::ColorProviderKey& key) {
  ui::ColorMixer& mixer = provider->AddMixer();
  mixer[kColorBraveSharedPinnedTabDummyViewBackground] = {leo::GetColor(
      leo::Color::kColorContainerBackgroundDesktop, leo::Theme::kLight)};
  mixer[kColorBraveSharedPinnedTabDummyViewTitle] = {
      leo::GetColor(leo::Color::kColorTextPrimary, leo::Theme::kLight)};
  mixer[kColorBraveSharedPinnedTabDummyViewDescription] = {
      leo::GetColor(leo::Color::kColorTextSecondary, leo::Theme::kLight)};
  mixer[kColorBraveSharedPinnedTabDummyViewThumbnailBorder] = {
      leo::GetColor(leo::Color::kColorDividerSubtle, leo::Theme::kLight)};
}

void AddBraveTabDarkThemeColorMixer(ui::ColorProvider* provider,
                                    const ui::ColorProviderKey& key) {
  ui::ColorMixer& mixer = provider->AddMixer();
  mixer[kColorBraveSharedPinnedTabDummyViewBackground] = {leo::GetColor(
      leo::Color::kColorContainerBackgroundDesktop, leo::Theme::kDark)};
  mixer[kColorBraveSharedPinnedTabDummyViewTitle] = {
      leo::GetColor(leo::Color::kColorTextPrimary, leo::Theme::kDark)};
  mixer[kColorBraveSharedPinnedTabDummyViewDescription] = {
      leo::GetColor(leo::Color::kColorTextSecondary, leo::Theme::kDark)};
  mixer[kColorBraveSharedPinnedTabDummyViewThumbnailBorder] = {
      leo::GetColor(leo::Color::kColorDividerSubtle, leo::Theme::kDark)};
}

}  // namespace tabs

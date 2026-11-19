/* Copyright (c) 2024 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include <cmath>

#include "brave/browser/ui/color/color_palette.h"
#include "chrome/browser/ui/color/chrome_color_id.h"
#include "chrome/browser/ui/color/chrome_color_mixers.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "third_party/skia/include/core/SkColor.h"
#include "ui/color/color_provider.h"
#include "ui/color/color_provider_key.h"
#include "ui/gfx/color_utils.h"

class BraveColorMixersTest : public testing::Test {
 public:
  BraveColorMixersTest() = default;

  ui::ColorProvider& color_provider() { return color_provider_; }
  void SetDarkMode() {
    color_provider_key_.color_mode = ui::ColorProviderKey::ColorMode::kDark;
  }

  void AddColorMixers() {
    // AddChromeColorMixers() calls all our mixers.
    AddChromeColorMixers(&color_provider_, color_provider_key_);
  }

 private:
  ui::ColorProvider color_provider_;
  ui::ColorProviderKey color_provider_key_;
};

TEST_F(BraveColorMixersTest, ColorOverrideTest) {
  AddColorMixers();

  EXPECT_EQ(color_provider().GetColor(kColorToolbar),
            color_provider().GetColor(kColorInfoBarBackground));
  EXPECT_EQ(color_provider().GetColor(kColorOmniboxIconHover),
            SkColorSetA(color_provider().GetColor(kColorOmniboxText),
                        std::ceil(0.10f * 255.0f)));
}

// FlyWeb 3.2.0 interface colors (FlyWeb/branding/interfaz-3.2.0 in softmac).
TEST_F(BraveColorMixersTest, FlyWebLightColors) {
  AddColorMixers();

  EXPECT_EQ(color_provider().GetColor(kColorToolbar), kFlyWebLightToolbar);
  EXPECT_EQ(color_provider().GetColor(kColorTabBackgroundActiveFrameActive),
            kFlyWebLightToolbar);
  EXPECT_EQ(color_provider().GetColor(ui::kColorFrameActive), kLightFrame);
  // Window without focus: dimmer, with Chromium's recipe.
  EXPECT_NE(color_provider().GetColor(ui::kColorFrameInactive), kLightFrame);
  EXPECT_EQ(color_provider().GetColor(kColorToolbarButtonIcon),
            kFlyWebLightToolbarIcon);
  EXPECT_EQ(color_provider().GetColor(kColorBookmarkFolderIcon),
            kFlyWebLightToolbarIcon);
  EXPECT_EQ(color_provider().GetColor(kColorTabForegroundActiveFrameActive),
            kFlyWebLightTabText);
  EXPECT_EQ(color_provider().GetColor(kColorTabForegroundInactiveFrameActive),
            color_utils::AlphaBlend(kFlyWebLightTabText, kLightFrame,
                                    kFlyWebInactiveTabTextAlpha));
  EXPECT_EQ(color_provider().GetColor(kColorLocationBarBackground),
            kFlyWebLocationBarBackground);
}

TEST_F(BraveColorMixersTest, FlyWebDarkColors) {
  SetDarkMode();
  AddColorMixers();

  EXPECT_EQ(color_provider().GetColor(kColorToolbar), kFlyWebDarkToolbar);
  EXPECT_EQ(color_provider().GetColor(kColorTabBackgroundActiveFrameActive),
            kFlyWebDarkToolbar);
  EXPECT_EQ(color_provider().GetColor(ui::kColorFrameActive), kDarkFrame);
  EXPECT_NE(color_provider().GetColor(ui::kColorFrameInactive), kDarkFrame);
  EXPECT_EQ(color_provider().GetColor(kColorToolbarButtonIcon),
            kFlyWebDarkToolbarIcon);
  EXPECT_EQ(color_provider().GetColor(kColorTabForegroundActiveFrameActive),
            kFlyWebDarkTabText);
  EXPECT_EQ(color_provider().GetColor(kColorTabForegroundInactiveFrameActive),
            color_utils::AlphaBlend(kFlyWebDarkTabText, kDarkFrame,
                                    kFlyWebInactiveTabTextAlpha));

  if (kFlyWebLightOmniboxInDarkMode) {
    // White URL field and popup, with dark text.
    EXPECT_EQ(color_provider().GetColor(kColorLocationBarBackground),
              kFlyWebLocationBarBackground);
    EXPECT_EQ(color_provider().GetColor(kColorOmniboxResultsBackground),
              kFlyWebLocationBarBackground);
    EXPECT_EQ(color_provider().GetColor(kColorOmniboxText), kLightOmniboxText);
    EXPECT_EQ(color_provider().GetColor(kColorOmniboxResultsTextSelected),
              kLightOmniboxText);
  }
}

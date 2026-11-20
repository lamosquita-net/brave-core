/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/browser/ui/views/location_bar/brave_location_bar_view.h"

#include <algorithm>
#include <cmath>
#include <optional>
#include <utility>

#include "base/check.h"
#include "base/feature_list.h"
#include "base/numerics/safe_conversions.h"
#include "brave/app/vector_icons/vector_icons.h"
#include "brave/browser/themes/brave_theme_service.h"
#include "brave/browser/ui/color/brave_color_id.h"
#include "brave/browser/ui/tabs/brave_tab_prefs.h"
#include "brave/browser/ui/tabs/features.h"
#include "brave/browser/ui/views/brave_actions/brave_actions_container.h"
#include "brave/browser/ui/views/toolbar/brave_toolbar_view.h"
#include "brave/components/l10n/common/localization_util.h"
#include "brave/grit/brave_theme_resources.h"
#include "cc/paint/paint_flags.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/browser/ui/color/chrome_color_id.h"
#include "chrome/browser/themes/theme_service_factory.h"
#include "chrome/browser/ui/layout_constants.h"
#include "chrome/browser/ui/browser_window/public/browser_window_interface.h"
#include "chrome/browser/ui/omnibox/omnibox_controller.h"
#include "chrome/browser/ui/omnibox/omnibox_edit_model.h"
#include "chrome/browser/ui/omnibox/omnibox_theme.h"
#include "chrome/browser/ui/views/chrome_layout_provider.h"
#include "chrome/browser/ui/views/location_bar/location_bar_view.h"
#include "chrome/browser/ui/views/omnibox/omnibox_view_views.h"
#include "chrome/grit/branded_strings.h"
#include "components/grit/brave_components_strings.h"
#include "components/version_info/channel.h"
#include "content/public/browser/navigation_entry.h"
#include "ui/base/metadata/metadata_header_macros.h"
#include "ui/base/metadata/metadata_impl_macros.h"
#include "ui/base/resource/resource_bundle.h"
#include "ui/color/color_provider.h"
#include "ui/compositor/layer.h"
#include "ui/gfx/canvas.h"
#include "ui/gfx/color_utils.h"
#include "ui/gfx/geometry/rect_f.h"
#include "ui/gfx/geometry/size.h"
#include "ui/gfx/geometry/skia_conversions.h"
#include "ui/gfx/image/image_skia.h"
#include "ui/gfx/paint_vector_icon.h"
#include "ui/views/controls/focus_ring.h"
#include "ui/views/controls/highlight_path_generator.h"

namespace {

// FlyWeb 3.2.0 interface (softmac FlyWeb/branding/interfaz-3.2.0): instead of
// the colored focus ring, the focused URL field shows a soft gray inner glow.
// The values are meant to be adjusted in tests with Titín.
// Titín (20-11, maqueta-claro-foco-hd.webp): a blurred gray inner shadow of
// 3-4 px on the four sides, the same in light and dark mode (the field is
// white in both). Its opacity halves every dip from the edge inwards, which
// at 1x gives about #DD, #EE, #F6, #FB over white, as in the mockup.
// Width of the shadow from the edge inwards, in dip.
constexpr float kFlyWebOmniboxFocusGlowThickness = 4.0f;
// Color of the shadow; its opacity fades to transparent inwards.
constexpr SkColor kFlyWebOmniboxFocusGlowColor =
    SkColorSetRGB(0x80, 0x80, 0x80);
// Opacity at the very edge, 0-1, while the field has the caret (typing).
constexpr float kFlyWebOmniboxFocusGlowOpacity = 0.37f;
// Titín (20-11, bt-320c): the shadow is always inside the field. With the
// mouse over the field and no caret it shows the same shadow, softer (about
// #ED at the edge at 1x, against #DD with the caret). Brave's outer hover
// shadow is not used in these windows. Opacity at the very edge, 0-1.
constexpr float kFlyWebOmniboxHoverGlowOpacity = 0.20f;
// Titín, 20-11 (maqueta-oscuro-desplegable-hd.webp): on the dark field of dark
// mode the shadow is darker than the field, not gray: about #3A-#41 at the
// edge on #4D4D4D, so black at these edge opacities (same fade).
constexpr SkColor kFlyWebOmniboxFocusGlowColorOnDark = SK_ColorBLACK;
constexpr float kFlyWebOmniboxFocusGlowOpacityOnDark = 0.22f;
constexpr float kFlyWebOmniboxHoverGlowOpacityOnDark = 0.12f;
// Distance, in dip, over which the opacity halves.
constexpr float kFlyWebOmniboxFocusGlowHalvingDistance = 1.0f;
// Number of rings that draw the fade (0.5 dip each, smooth on Retina too).
// Plain strokes, no blur filter, so the cost is the same on old GPUs
// (MacPro5,1).
constexpr int kFlyWebOmniboxFocusGlowSteps = 8;

// Transparent view on top of the location bar children that paints the inner
// glow. It must be above the omnibox textfield, which has an opaque
// background. It ignores events and accessibility.
class FlyWebOmniboxFocusGlowView : public views::View {
  METADATA_HEADER(FlyWebOmniboxFocusGlowView, views::View)

 public:
  FlyWebOmniboxFocusGlowView() {
    SetCanProcessEventsWithinSubtree(false);
    GetViewAccessibility().SetIsIgnored(true);
    SetVisible(false);
  }
  FlyWebOmniboxFocusGlowView(const FlyWebOmniboxFocusGlowView&) = delete;
  FlyWebOmniboxFocusGlowView& operator=(const FlyWebOmniboxFocusGlowView&) =
      delete;
  ~FlyWebOmniboxFocusGlowView() override = default;

  void set_corner_radius(float radius) { corner_radius_ = radius; }

  // Color and opacity at the very edge: kFlyWebOmniboxFocusGlow* or
  // kFlyWebOmniboxHoverGlowOpacity*, gray on a light field and black on a
  // dark one.
  void set_edge(SkColor color, float opacity) {
    if (edge_color_ == color && edge_opacity_ == opacity) {
      return;
    }
    edge_color_ = color;
    edge_opacity_ = opacity;
    SchedulePaint();
  }

  // views::View:
  void OnPaint(gfx::Canvas* canvas) override {
    const float step =
        kFlyWebOmniboxFocusGlowThickness / kFlyWebOmniboxFocusGlowSteps;
    cc::PaintFlags flags;
    flags.setAntiAlias(true);
    flags.setStyle(cc::PaintFlags::kStroke_Style);
    flags.setStrokeWidth(step);
    for (int i = 0; i < kFlyWebOmniboxFocusGlowSteps; ++i) {
      const float inset = (i + 0.5f) * step;
      gfx::RectF ring(GetLocalBounds());
      ring.Inset(inset);
      if (ring.IsEmpty()) {
        break;
      }
      // Exponential fade from the edge inwards, sampled at the ring center.
      const float opacity =
          edge_opacity_ *
          std::exp2(-inset / kFlyWebOmniboxFocusGlowHalvingDistance);
      flags.setColor(SkColorSetA(edge_color_,
                                 base::ClampRound<SkAlpha>(opacity * 255)));
      canvas->DrawRoundRect(ring, std::max(corner_radius_ - inset, 0.0f),
                            flags);
    }
  }

 private:
  float corner_radius_ = 0;
  SkColor edge_color_ = kFlyWebOmniboxFocusGlowColor;
  float edge_opacity_ = kFlyWebOmniboxFocusGlowOpacity;
};

BEGIN_METADATA(FlyWebOmniboxFocusGlowView)
END_METADATA

// Regular and guest windows use the FlyWeb glow. Private and Tor windows keep
// Brave's focus ring and its color.
bool UsesFlyWebFocusGlow(Profile* profile) {
  return profile->IsRegularProfile() || profile->IsGuestSession();
}

class BraveLocationBarViewFocusRingHighlightPathGenerator
    : public views::HighlightPathGenerator {
 public:
  BraveLocationBarViewFocusRingHighlightPathGenerator() = default;
  BraveLocationBarViewFocusRingHighlightPathGenerator(
      const BraveLocationBarViewFocusRingHighlightPathGenerator&) = delete;
  BraveLocationBarViewFocusRingHighlightPathGenerator& operator=(
      const BraveLocationBarViewFocusRingHighlightPathGenerator&) = delete;

  // HighlightPathGenerator
  SkPath GetHighlightPath(const views::View* view) override {
    return static_cast<const BraveLocationBarView*>(view)
        ->GetFocusRingHighlightPath();
  }
};

std::optional<BraveColorIds> GetFocusRingColor(Profile* profile) {
  if (profile->IsRegularProfile() || profile->IsGuestSession()) {
    // Don't update color.
    return std::nullopt;
  }
  // Private or Tor window - use color mixer.
  return kColorLocationBarFocusRing;
}

}  // namespace

BraveLocationBarView::BraveLocationBarView(BrowserWindowInterface* browser,
                                           Profile* profile,
                                           CommandUpdater* command_updater,
                                           Delegate* delegate,
                                           bool is_popup_mode)
    : LocationBarView(browser,
                      profile,
                      command_updater,
                      delegate,
                      is_popup_mode) {}

BraveLocationBarView::~BraveLocationBarView() = default;

void BraveLocationBarView::Init() {
  // base method calls Update and Layout
  LocationBarView::Init();
  // FlyWeb: regular and guest windows show the inner glow instead of the
  // focus ring. Chromium only installs the focus ring outside popup mode.
  const bool use_focus_glow =
      views::FocusRing::Get(this) && UsesFlyWebFocusGlow(GetProfile());
  if (use_focus_glow) {
    views::FocusRing::Remove(this);
  }

  // Change focus ring highlight path
  views::FocusRing* focus_ring = views::FocusRing::Get(this);
  if (focus_ring) {
    focus_ring->SetPathGenerator(
        std::make_unique<
            BraveLocationBarViewFocusRingHighlightPathGenerator>());
    if (const auto color_id = GetFocusRingColor(GetProfile())) {
      focus_ring->SetColorId(color_id.value());
    }
  }

  // brave action buttons
  brave_actions_ = AddChildView(
      std::make_unique<BraveActionsContainer>(browser_, GetProfile()));
  brave_actions_->Init();

  // Added after every other child so the glow paints on top of them.
  if (use_focus_glow) {
    focus_glow_ = AddChildView(std::make_unique<FlyWebOmniboxFocusGlowView>());
  }

  // Call Update again to cause a Layout
  Update(nullptr);

  // Stop slide animation for all content settings views icon.
  for (ContentSettingImageView* content_setting_view : content_setting_views_) {
    content_setting_view->disable_animation();
  }
}

void BraveLocationBarView::Update(content::WebContents* contents) {
  // base Init calls update before our Init is run, so our children
  // may not be initialized yet
  if (brave_actions_) {
    brave_actions_->Update();
  }

  LocationBarView::Update(contents);
}

void BraveLocationBarView::Layout(PassKey) {
  if (ignore_layout_) {
    return;
  }

  LayoutSuperclass<LocationBarView>(this);

  if (focus_glow_) {
    focus_glow_->SetBoundsRect(GetLocalBounds());
    static_cast<FlyWebOmniboxFocusGlowView*>(focus_glow_.get())
        ->set_corner_radius(GetBorderRadius());
  }
}

void BraveLocationBarView::OnVisibleBoundsChanged() {
  if (ignore_layout_) {
    return;
  }

  LocationBarView::OnVisibleBoundsChanged();
}

void BraveLocationBarView::OnChanged() {
  auto hide_page_actions = ShouldHidePageActionIcons();
  if (brave_actions_) {
    brave_actions_->SetShouldHide(hide_page_actions);
  }

  // OnChanged calls Layout
  LocationBarView::OnChanged();
}

std::vector<views::View*> BraveLocationBarView::GetRightMostTrailingViews() {
  std::vector<views::View*> views;
  if (brave_actions_) {
    views.push_back(brave_actions_);
  }

  return views;
}

std::vector<views::View*> BraveLocationBarView::GetLeftMostTrailingViews() {
  std::vector<views::View*> views;
  return views;
}

void BraveLocationBarView::RefreshBackground() {
  LocationBarView::RefreshBackground();

  // Chromium calls RefreshBackground() on focus, blur, popup and hover
  // changes. With the caret: the inner shadow, also while the suggestions
  // popup is open (Titín, 20-11, bt-320e: the field keeps its inner shadow
  // when typing; the popup shows the field through its cutout). Without it:
  // the softer shadow while the mouse is over the field, where Brave showed
  // its outer shadow.
  if (focus_glow_) {
    const bool caret_visible =
        GetOmniboxController()->edit_model()->is_caret_visible();
    const SkColor field_color =
        GetColorProvider()
            ? GetColorProvider()->GetColor(kColorLocationBarBackground)
            : SK_ColorWHITE;
    const bool dark_field = color_utils::IsDark(field_color);

    // FlyWeb 3.2.0 (Titín, 20-11): Chromium paints the focused field with
    // the popup background. In dark mode they differ (#4D4D4D field, #909294
    // popup): keep the field's own color while it has the caret.
    if (caret_visible && GetColorProvider() &&
        field_color !=
            GetColorProvider()->GetColor(kColorOmniboxResultsBackground)) {
      SetBackground(CreateRoundRectBackground(
          field_color, SK_ColorTRANSPARENT, size(),
          /*blend_mode=*/SkBlendMode::kSrcOver,
          /*antialias=*/true, /*should_border_scale=*/true));
      omnibox_view_->SetBackgroundColor(field_color);
    }

    float opacity;
    if (dark_field) {
      opacity = caret_visible ? kFlyWebOmniboxFocusGlowOpacityOnDark
                              : kFlyWebOmniboxHoverGlowOpacityOnDark;
    } else {
      opacity = caret_visible ? kFlyWebOmniboxFocusGlowOpacity
                              : kFlyWebOmniboxHoverGlowOpacity;
    }
    static_cast<FlyWebOmniboxFocusGlowView*>(focus_glow_.get())
        ->set_edge(dark_field ? kFlyWebOmniboxFocusGlowColorOnDark
                              : kFlyWebOmniboxFocusGlowColor,
                   opacity);
    focus_glow_->SetVisible(caret_visible || IsMouseHovered());
  }

  if (shadow_) {
    const bool show_shadow =
        IsMouseHovered() &&
        !GetOmniboxController()->edit_model()->is_caret_visible();
    shadow_->SetVisible(show_shadow);
    return;
  }
}

int BraveLocationBarView::GetMinimumTrailingWidth() const {
  int trailing_width = LocationBarView::GetMinimumTrailingWidth();
  const int elem_pad =
      GetLayoutConstant(LayoutConstant::kLocationBarElementPadding);

  if (brave_actions_ && brave_actions_->GetVisible()) {
    trailing_width += brave_actions_->GetMinimumSize().width() + elem_pad;
  }

  return trailing_width;
}

gfx::Size BraveLocationBarView::GetMinimumSize() const {
  gfx::Size min_size = LocationBarView::GetMinimumSize();
  if (!IsInitialized()) {
    return min_size;
  }

  // Skip the additive formula for non-normal browser windows (popups, apps):
  // a minimum wider than the width requested by window.open() would push the
  // clamped popup partially outside the work area.
  if (browser_->GetType() != BrowserWindowInterface::Type::TYPE_NORMAL) {
    return min_size;
  }

  // Unlike upstream which uses max(omnibox, leading+trailing), reserve space
  // for all children simultaneously so the omnibox always has its minimum
  // alongside all visible decorations.
  const int padding =
      GetLayoutConstant(LayoutConstant::kLocationBarElementPadding);
  const int width = GetInsets().width() + GetMinimumLeadingWidth() + padding +
                    omnibox_view_->GetMinimumSize().width() +
                    GetMinimumTrailingWidth();
  min_size.set_width(width);
  return min_size;
}

void BraveLocationBarView::OnThemeChanged() {
  LocationBarView::OnThemeChanged();

  if (!IsInitialized()) {
    return;
  }

  Update(nullptr);
  SetupShadow();
}

void BraveLocationBarView::AddedToWidget() {
  SetupShadow();
}

void BraveLocationBarView::ChildVisibilityChanged(views::View* child) {
  LocationBarView::ChildVisibilityChanged(child);
  // Normally, PageActionIcons are in a container which is always visible, only
  // the size changes when an icon is shown or hidden. The LocationBarView
  // does not listen to ChildVisibilityChanged events so we must make we Layout
  // and re-caculate trailing decorator positions when a child changes.
  if (std::ranges::contains(GetLeftMostTrailingViews(), child) ||
      std::ranges::contains(GetRightMostTrailingViews(), child)) {
    DeprecatedLayoutImmediately();
    SchedulePaint();
  }
}

void BraveLocationBarView::SetupShadow() {
  // FlyWeb: regular and guest windows, which have the inner glow, have no
  // outer hover shadow (Titín, 20-11: the shadow is always inside the field).
  // Checked by profile, since this can run before Init() adds the glow.
  if (UsesFlyWebFocusGlow(GetProfile())) {
    shadow_.reset();
    return;
  }

  const auto* const color_provider = GetColorProvider();
  if (!color_provider) {
    return;
  }

  const int radius = GetBorderRadius();
  ViewShadow::ShadowParameters shadow{
      .offset_x = 0,
      .offset_y = 1,
      .blur_radius = radius,
      .shadow_color = color_provider->GetColor(kColorLocationBarHoveredShadow)};

  shadow_ = std::make_unique<ViewShadow>(this, radius, shadow);
}

int BraveLocationBarView::GetBorderRadius() const {
  // FlyWeb: the subtle radius of the mockups (ComputeBorderRadius() in
  // chromium_src location_bar_view.cc), the same for the inner glow.
  return ComputeBorderRadius(size());
}

void BraveLocationBarView::FocusLocation(bool is_user_initiated,
                                         bool clear_focus_if_failed) {
  if (base::FeatureList::IsEnabled(tabs::features::kBraveSharedPinnedTabs) &&
      browser_->GetProfile()->GetPrefs()->GetBoolean(
          brave_tabs::kSharedPinnedTab)) {
    // When updating dummy contents, this could be called even when the widget
    // is inactive. We shouldn't focus the omnibox in that case.
    if (auto* widget = GetWidget(); !widget || !widget->IsActive()) {
      return;
    }
  }

  omnibox_view_->SetFocus(is_user_initiated);
}

SkPath BraveLocationBarView::GetFocusRingHighlightPath() const {
  const SkScalar radius = GetBorderRadius();
  return SkPath::RRect(gfx::RectToSkRect(GetLocalBounds()), radius, radius);
}

ContentSettingImageView*
BraveLocationBarView::GetContentSettingsImageViewForTesting(size_t idx) {
  DCHECK(idx < content_setting_views_.size());
  return content_setting_views_[idx];
}

BEGIN_METADATA(BraveLocationBarView)
END_METADATA

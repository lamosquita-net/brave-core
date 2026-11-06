/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/browser/ui/views/frame/brave_browser_widget.h"

#include "brave/browser/themes/brave_private_window_theme_supplier.h"
#include "brave/browser/ui/tabs/features.h"
#include "brave/browser/ui/tabs/shared_pinned_tab_service.h"
#include "brave/browser/ui/tabs/shared_pinned_tab_service_factory.h"
#include "brave/browser/ui/views/frame/brave_browser_root_view.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/browser/ui/browser.h"
#include "chrome/browser/ui/views/frame/browser_view.h"
#include "chrome/browser/ui/views/frame/browser_widget.h"
#include "ui/color/color_provider_key.h"

BraveBrowserWidget::BraveBrowserWidget(BrowserView* browser_view)
    : BrowserWidget(browser_view), view_(browser_view) {
  if (view_->browser()->GetProfile()->IsIncognitoProfile() ||
      view_->browser()->GetProfile()->IsGuestSession()) {
    theme_supplier_ = base::MakeRefCounted<BravePrivateWindowThemeSupplier>();
  }
}

BraveBrowserWidget::~BraveBrowserWidget() = default;

ui::ColorProviderKey::ThemeInitializerSupplier*
BraveBrowserWidget::GetCustomTheme() const {
  // To provider private windows's theme color via color provider,
  // we use |theme_supplier_| for both as upstream doesn't use separated
  // mix for private window.
  if (theme_supplier_) {
    return theme_supplier_.get();
  }

  return BrowserWidget::GetCustomTheme();
}

views::internal::RootView* BraveBrowserWidget::CreateRootView() {
  root_view_ = new BraveBrowserRootView(browser_view_, this);
  return root_view_;
}

void BraveBrowserWidget::SetTabDragKind(TabDragKind kind) {
  const bool should_sync_shared_pinned_tab =
      base::FeatureList::IsEnabled(tabs::features::kBraveSharedPinnedTabs) &&
      tab_drag_kind_ == TabDragKind::kAllTabs && kind == TabDragKind::kNone;

  BrowserWidget::SetTabDragKind(kind);

  if (should_sync_shared_pinned_tab) {
    SharedPinnedTabServiceFactory::GetForProfile(browser_view_->GetProfile())
        ->TabDraggingEnded(browser_view_->browser());
  }
}

// 156 has no NativeTheme instance for dark UI any more: private and guest
// windows get their colors from the color provider key (as in Brave 1.98).
ui::ColorProviderKey BraveBrowserWidget::GetColorProviderKey() const {
  auto key = BrowserWidget::GetColorProviderKey();

  // `ThemeService::GetColorProviderKey()` clears `custom_theme` for incognito
  // profiles, which drops the private/guest window theme supplier we install
  // via GetCustomTheme(). Re-apply it here so private window colors come from
  // BravePrivateWindowThemeSupplier.
  if (theme_supplier_) {
    key.custom_theme = GetCustomTheme();
  }

  // We want to use dark mode for guest profile.
  if (view_->browser()->GetProfile()->IsGuestSession()) {
    key.color_mode = ui::ColorProviderKey::ColorMode::kDark;
    key.user_color_source = ui::ColorProviderKey::UserColorSource::kGrayscale;
  }

  return key;
}

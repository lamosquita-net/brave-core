/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/browser/ui/views/tabs/brave_tab_strip.h"

#include <memory>
#include <utility>

#include "brave/browser/themes/brave_dark_mode_utils.h"
#include "brave/browser/ui/color/brave_color_id.h"
#include "brave/browser/ui/tabs/brave_tab_prefs.h"
#include "brave/browser/ui/tabs/features.h"
#include "brave/browser/ui/tabs/shared_pinned_tab_service.h"
#include "brave/browser/ui/tabs/shared_pinned_tab_service_factory.h"
#include "brave/browser/ui/views/tabs/brave_tab.h"
#include "brave/browser/ui/views/tabs/brave_tab_hover_card_controller.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/browser/themes/theme_service.h"
#include "chrome/browser/themes/theme_service_factory.h"
#include "chrome/browser/ui/browser_window/public/browser_window_interface.h"
#include "chrome/browser/ui/color/chrome_color_id.h"
#include "chrome/browser/ui/views/tabs/tab.h"
#include "chrome/browser/ui/views/tabs/tab_slot_view.h"
#include "chrome/browser/ui/views/tabs/tab_strip_controller.h"
#include "third_party/skia/include/core/SkColor.h"
#include "ui/base/metadata/metadata_impl_macros.h"
#include "ui/gfx/canvas.h"

BraveTabStrip::BraveTabStrip(
    std::unique_ptr<TabStripController> controller,
    std::unique_ptr<BraveTabHoverCardController> hover_card_controller)
    : TabStrip(std::move(controller), std::move(hover_card_controller)) {}

BraveTabStrip::~BraveTabStrip() = default;

void BraveTabStrip::UpdateHoverCard(HoverCardAnchorTarget* anchor_target,
                                    HoverCardUpdateType update_type) {
  if (brave_tabs::AreTooltipsEnabled(
          controller_->GetBrowserWindowInterface()->GetProfile()->GetPrefs())) {
    return;
  }
  TabStrip::UpdateHoverCard(anchor_target, update_type);
}

void BraveTabStrip::MaybeStartDrag(TabSlotView* source,
                                   const ui::LocatedEvent& event,
                                   ui::ListSelectionModel original_selection) {
  if (base::FeatureList::IsEnabled(tabs::features::kBraveSharedPinnedTabs)) {
    // When source tab is bound for dummy web contents for a shared pinned tab,
    // we shouldn't kick off drag-and-drop session as the web contents will be
    // replaced soon.
    if (source->GetTabSlotViewType() == TabSlotView::ViewType::kTab &&
        static_cast<Tab*>(source)->data().pinned) {
      auto index = GetModelIndexOf(source).value();
      auto* browser = controller_->GetBrowserWindowInterface();
      DCHECK(browser);

      auto* shared_pinned_tab_service =
          SharedPinnedTabServiceFactory::GetForProfile(browser->GetProfile());
      DCHECK(shared_pinned_tab_service);
      if (shared_pinned_tab_service->IsDummyContents(
              browser->GetTabStripModel()->GetWebContentsAt(index))) {
        return;
      }
    }
  }

  TabStrip::MaybeStartDrag(source, event, original_selection);
}

BEGIN_METADATA(BraveTabStrip)
END_METADATA

/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/browser/ui/views/tabs/brave_tab_strip.h"

#include <memory>
#include <optional>
#include <utility>

#include "base/functional/callback_helpers.h"
#include "brave/browser/themes/brave_dark_mode_utils.h"
#include "brave/browser/ui/color/brave_color_id.h"
#include "brave/browser/ui/tabs/brave_tab_prefs.h"
#include "brave/browser/ui/tabs/features.h"
#include "brave/browser/ui/tabs/shared_pinned_tab_service.h"
#include "brave/browser/ui/tabs/shared_pinned_tab_service_factory.h"
#include "brave/browser/ui/views/frame/brave_browser_view.h"
#include "brave/browser/ui/views/tabs/brave_compound_tab_container.h"
#include "brave/browser/ui/views/tabs/brave_tab.h"
#include "brave/browser/ui/views/tabs/brave_tab_container.h"
#include "brave/browser/ui/views/tabs/brave_tab_hover_card_controller.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/browser/themes/theme_service.h"
#include "chrome/browser/themes/theme_service_factory.h"
#include "chrome/browser/ui/color/chrome_color_id.h"
#include "chrome/browser/ui/tabs/features.h"
#include "chrome/browser/ui/tabs/tab_group_model.h"
#include "chrome/browser/ui/ui_features.h"
#include "chrome/browser/ui/views/tabs/tab.h"
#include "chrome/browser/ui/views/tabs/tab_container.h"
#include "chrome/browser/ui/views/tabs/tab_slot_view.h"
#include "chrome/browser/ui/views/tabs/tab_strip_controller.h"
#include "chrome/browser/ui/views/tabs/tab_strip_observer.h"
#include "chrome/browser/ui/views/tabs/tab_strip_scroll_container.h"
#include "components/tab_groups/tab_group_id.h"
#include "components/tabs/public/tab_group.h"
#include "third_party/skia/include/core/SkColor.h"
#include "ui/base/metadata/metadata_impl_macros.h"
#include "ui/compositor/layer.h"
#include "ui/gfx/canvas.h"
#include "ui/views/layout/flex_layout.h"

BraveTabStrip::BraveTabStrip(std::unique_ptr<TabStripController> controller)
    : TabStrip(std::move(controller)) {}

BraveTabStrip::~BraveTabStrip() = default;

void BraveTabStrip::UpdateHoverCard(Tab* tab, HoverCardUpdateType update_type) {
  if (brave_tabs::AreTooltipsEnabled(controller_->GetProfile()->GetPrefs())) {
    return;
  }
  TabStrip::UpdateHoverCard(tab, update_type);
}

void BraveTabStrip::MaybeStartDrag(
    TabSlotView* source,
    const ui::LocatedEvent& event,
    const ui::ListSelectionModel& original_selection) {
  if (base::FeatureList::IsEnabled(tabs::features::kBraveSharedPinnedTabs)) {
    // When source tab is bound for dummy web contents for a shared pinned tab,
    // we shouldn't kick off drag-and-drop session as the web contents will be
    // replaced soon.
    if (source->GetTabSlotViewType() == TabSlotView::ViewType::kTab &&
        static_cast<Tab*>(source)->data().pinned) {
      auto index = GetModelIndexOf(source).value();
      auto* browser = controller_->GetBrowser();
      DCHECK(browser);

      auto* shared_pinned_tab_service =
          SharedPinnedTabServiceFactory::GetForProfile(browser->profile());
      DCHECK(shared_pinned_tab_service);
      if (shared_pinned_tab_service->IsDummyContents(
              browser->tab_strip_model()->GetWebContentsAt(index))) {
        return;
      }
    }
  }

  TabStrip::MaybeStartDrag(source, event, original_selection);
}

void BraveTabStrip::AddedToWidget() {
  TabStrip::AddedToWidget();

  if (BrowserView::GetBrowserViewForBrowser(GetBrowser())) {
    UpdateTabContainer();
  } else {
    // Schedule UpdateTabContainer(). At this point, BrowserWindow could still
    // be being created and it could be unbound to Browser.
    base::SequencedTaskRunner::GetCurrentDefault()->PostTask(
        FROM_HERE, base::BindOnce(&BraveTabStrip::UpdateTabContainer,
                                  weak_factory_.GetWeakPtr()));
  }
}

void BraveTabStrip::UpdateTabContainer() {
  const bool should_use_compound_tab_container =
      base::FeatureList::IsEnabled(tabs::kSplitTabStrip);
  const bool is_using_compound_tab_container =
      views::IsViewClass<BraveCompoundTabContainer>(
          base::to_address(tab_container_));

  base::ScopedClosureRunner layout_lock;
  if (should_use_compound_tab_container != is_using_compound_tab_container) {
    // Before removing any child, we should complete the 'tab closing animation'
    // so that we don't delete views twice.
    tab_container_->CompleteAnimationAndLayout();

    // Resets TabContainer to use.
    auto original_container = RemoveChildViewT(
        static_cast<TabContainer*>(base::to_address(tab_container_)));

    if (should_use_compound_tab_container) {
      // Container should be attached before TabDragContext so that dragged
      // views can be atop container.
      auto* drag_context = GetDragContext();
      auto* brave_tab_container = AddChildViewAt(
          std::make_unique<BraveCompoundTabContainer>(
              *this, hover_card_controller_.get(), drag_context, *this, this),
          0);
      tab_container_ = *brave_tab_container;
      layout_lock =
          base::ScopedClosureRunner(brave_tab_container->LockLayout());

      // Make dragged views on top of container's layer.
      drag_context->SetPaintToLayer();
      drag_context->layer()->SetFillsBoundsOpaquely(false);
      drag_context->parent()->ReorderChildView(drag_context, -1);
    } else {
      // Container should be attached before TabDragContext so that dragged
      // views can be atop container.
      auto* brave_tab_container =
          AddChildViewAt(std::make_unique<BraveTabContainer>(
                             *this, hover_card_controller_.get(),
                             GetDragContext(), *this, this),
                         0);
      tab_container_ = *brave_tab_container;
      layout_lock =
          base::ScopedClosureRunner(brave_tab_container->LockLayout());

      GetDragContext()->DestroyLayer();
    }

    // Resets TabSlotViews for the new TabContainer.
    auto* model = GetBrowser()->tab_strip_model();
    for (int i = 0; i < model->count(); i++) {
      auto* tab = original_container->GetTabAtModelIndex(i);
      // At this point, we don't have group views in the container. So before
      // restoring groups, clears group for tabs.
      tab->set_group(std::nullopt);
      tab_container_->AddTab(
          tab->parent()->RemoveChildViewT(tab), i,
          tab->data().pinned ? TabPinned::kPinned : TabPinned::kUnpinned);
      if (tab->dragging()) {
        GetDragContext()->AddChildView(tab);
      }
    }

    auto* group_model = model->group_model();
    for (auto group_id : group_model->ListTabGroups()) {
      auto* group = group_model->GetTabGroup(group_id);
      tab_container_->OnGroupCreated(group_id);
      const auto tabs = group->ListTabs();
      for (auto i = tabs.start(); i < tabs.end(); i++) {
        AddTabToGroup(group_id, i);
        tab_at(i)->SchedulePaint();
      }
      auto* group_views = tab_container_->GetGroupViews(group_id);
      group_views->UpdateBounds();

      if (auto* original_header =
              original_container->GetGroupViews(group_id)->header();
          original_header->dragging()) {
        group_views->header()->set_dragging(true);
        GetDragContext()->AddChildView(group_views->header());

        group_views->header()->SetBoundsRect(original_header->bounds());
        DCHECK_NE(original_header->parent(), original_container.get())
            << "The header should be child of TabDragContext at this point.";
        original_container->AddChildView(
            original_header->parent()->RemoveChildViewT(original_header));
      }
    }

    for (auto group_id : group_model->ListTabGroups()) {
      auto* visual_data = group_model->GetTabGroup(group_id)->visual_data();
      tab_container_->OnGroupVisualsChanged(group_id, visual_data, visual_data);
    }

    if (observer_) {
      for (int i = 0; i < model->count(); i++) {
        observer_->OnTabAdded(i);
      }
    }

    // During drag-and-drop session, the active value could be invalid.
    if (const auto& selection_model = model->selection_model();
        selection_model.active().has_value()) {
      SetSelection(selection_model);
    }
  }

  // Update layout of TabContainer
  auto* browser = GetBrowser();
  DCHECK(browser);
  if (base::FeatureList::IsEnabled(tabs::kScrollableTabStrip)) {
    auto* browser_view = static_cast<BraveBrowserView*>(
        BrowserView::GetBrowserViewForBrowser(browser));
    DCHECK(browser_view);
    auto* scroll_container = static_cast<TabStripScrollContainer*>(
        browser_view->tab_strip_region_view()->tab_strip_container_);
    DCHECK(scroll_container);
    SetAvailableWidthCallback(base::BindRepeating(
        &TabStripScrollContainer::GetTabStripAvailableWidth,
        base::Unretained(scroll_container)));
  } else {
    SetAvailableWidthCallback(base::NullCallback());
  }

  if (should_use_compound_tab_container) {
    // Upstream's compound tab container lay out its sub containers manually.
    tab_container_->SetLayoutManager(nullptr);
  }

  if (const auto active_index = GetActiveIndex(); active_index) {
    // In order to update shadow state, call ActiveStateChanged().
    tab_at(active_index.value())->ActiveStateChanged();
  }
}

BEGIN_METADATA(BraveTabStrip)
END_METADATA

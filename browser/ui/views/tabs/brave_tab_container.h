/* Copyright (c) 2022 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_BROWSER_UI_VIEWS_TABS_BRAVE_TAB_CONTAINER_H_
#define BRAVE_BROWSER_UI_VIEWS_TABS_BRAVE_TAB_CONTAINER_H_

#include "base/functional/callback_forward.h"
#include "chrome/browser/ui/views/tabs/tab_container_impl.h"

class BraveTabContainer : public TabContainerImpl {
  METADATA_HEADER(BraveTabContainer, TabContainerImpl)
 public:
  BraveTabContainer(TabContainerController& controller,
                    TabHoverCardController* hover_card_controller,
                    TabDragContextBase* drag_context,
                    TabSlotController& tab_slot_controller,
                    views::View* scroll_contents_view);
  ~BraveTabContainer() override;

  // Calling this will freeze this view's layout. When the returned closure
  // runs, layout will be unlocked and run immediately.
  // This is to avoid accessing invalid index during reconstruction
  // of TabContainer. In addition, we can avoid redundant layout as a side
  // effect.
  base::OnceClosure LockLayout();

  // TabContainerImpl:
  gfx::Size CalculatePreferredSize(
      const views::SizeBounds& available_size) const override;
  void StartInsertTabAnimation(int model_index) override;
  void OnTabCloseAnimationCompleted(Tab* tab) override;
  void CompleteAnimationAndLayout() override;
  void PaintChildren(const views::PaintInfo& paint_info) override;
  void SetTabSlotVisibility() override;

 private:
  void OnUnlockLayout();

  bool layout_locked_ = false;
};

#endif  // BRAVE_BROWSER_UI_VIEWS_TABS_BRAVE_TAB_CONTAINER_H_

/* Copyright (c) 2022 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_BROWSER_UI_VIEWS_TABS_BRAVE_TAB_CONTAINER_H_
#define BRAVE_BROWSER_UI_VIEWS_TABS_BRAVE_TAB_CONTAINER_H_

#include "chrome/browser/ui/views/tabs/tab_container_impl.h"

class BraveTabContainer : public TabContainerImpl {
  METADATA_HEADER(BraveTabContainer, TabContainerImpl)
 public:
  BraveTabContainer(TabContainerController& controller,
                    TabHoverCardController* hover_card_controller,
                    TabDragPositioningDelegateBase* drag_position_delegate,
                    TabSlotController& tab_slot_controller);
  ~BraveTabContainer() override;

  // TabContainerImpl:
  void OnTabCloseAnimationCompleted(Tab* tab) override;
  void CompleteAnimationAndLayout() override;
  void SetTabSlotVisibility() override;
};

#endif  // BRAVE_BROWSER_UI_VIEWS_TABS_BRAVE_TAB_CONTAINER_H_

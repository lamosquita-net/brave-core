/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_BROWSER_UI_VIEWS_FRAME_BRAVE_BROWSER_VIEW_H_
#define BRAVE_BROWSER_UI_VIEWS_FRAME_BRAVE_BROWSER_VIEW_H_

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "base/callback_list.h"
#include "base/gtest_prod_util.h"
#include "base/memory/raw_ptr.h"
#include "base/memory/weak_ptr.h"
#include "base/scoped_observation.h"
#include "brave/browser/ui/commands/accelerator_service.h"
#include "brave/browser/ui/tabs/brave_tab_strip_model.h"
#include "brave/components/commands/browser/accelerator_pref_manager.h"
#include "build/build_config.h"
#include "chrome/browser/ui/bookmarks/bookmark_tab_helper.h"
#include "chrome/browser/ui/bookmarks/bookmark_tab_helper_observer.h"
#include "chrome/browser/ui/views/frame/browser_view.h"
#include "components/tabs/public/tab_interface.h"
#include "ui/base/accelerators/accelerator.h"
#include "ui/base/metadata/metadata_header_macros.h"

namespace content {
class WebContents;
}  // namespace content

class BraveBrowser;
class BraveHelpBubbleHostView;
class BrowserWindowInterface;

class BraveBrowserView : public BrowserView,
                         public BookmarkTabHelperObserver,
                         public commands::AcceleratorService::Observer {
  METADATA_HEADER(BraveBrowserView, BrowserView)
 public:
  explicit BraveBrowserView(BrowserWindowInterface* browser);
  BraveBrowserView(const BraveBrowserView&) = delete;
  BraveBrowserView& operator=(const BraveBrowserView&) = delete;
  ~BraveBrowserView() override;

  void ShowUpdateChromeDialog() override;

  // BrowserView overrides:
  void StartTabCycling() override;
  gfx::Rect GetShieldsBubbleRect() override;
  void OnActiveTabChanged(content::WebContents* old_contents,
                          content::WebContents* new_contents,
                          int index,
                          int reason) override;
  bool AcceleratorPressed(const ui::Accelerator& accelerator) override;
  bool IsInTabDragging() const override;

  bool ShowBraveHelpBubbleView(const std::string& text) override;

  // BookmarkTabHelperObserver:
  void URLStarredChanged(content::WebContents* web_contents,
                         bool starred) override;

  // commands::AcceleratorService:
  void OnAcceleratorsChanged(const commands::Accelerators& changed) override;

 private:
  class TabCyclingEventHandler;
  friend class WindowClosingConfirmBrowserTest;

  static void SetDownloadConfirmReturnForTesting(bool allow);

  // BrowserView overrides:
  void LoadAccelerators() override;
  void OnTabStripModelChanged(
      TabStripModel* tab_strip_model,
      const TabStripModelChange& change,
      const TabStripSelectionChange& selection) override;
  views::CloseRequestResult OnWindowCloseRequested() override;
  void ConfirmBrowserCloseWithPendingDownloads(
      int download_count,
      DownloadCloseType dialog_type,
      base::OnceCallback<void(bool)> callback) override;

  void StopTabCycling();
  void OnWindowClosingConfirmResponse(bool allowed_to_close);
  BraveBrowser* GetBraveBrowser() const;

  // 156 dropped BrowserWindow::SetStarredState(): observe the active tab's
  // BookmarkTabHelper to keep the toolbar bookmark button in sync.
  void ObserveBookmarkTabHelper(content::WebContents* contents);
  void SetStarredState(bool is_starred);

  // The active tab's WebContents can be replaced or destroyed without an
  // OnActiveTabChanged() call; move the observation off it before that.
  void OnActiveTabWillDiscardContents(tabs::TabInterface* tab,
                                      content::WebContents* old_contents,
                                      content::WebContents* new_contents);
  void OnActiveTabWillDetach(tabs::TabInterface* tab,
                             tabs::TabInterface::DetachReason reason);

  bool HasSelectedURL() const override;
  void CleanAndCopySelectedURL() override;

  bool closing_confirm_dialog_activated_ = false;
  raw_ptr<BraveHelpBubbleHostView> brave_help_bubble_host_view_ = nullptr;

  std::unique_ptr<TabCyclingEventHandler> tab_cycling_event_handler_;

  base::ScopedObservation<commands::AcceleratorService,
                          commands::AcceleratorService::Observer>
      accelerators_observation_{this};
  base::ScopedObservation<BookmarkTabHelper, BookmarkTabHelperObserver>
      bookmark_tab_helper_observation_{this};
  base::CallbackListSubscription active_tab_will_discard_contents_subscription_;
  base::CallbackListSubscription active_tab_will_detach_subscription_;

  base::WeakPtrFactory<BraveBrowserView> weak_ptr_{this};
};

#endif  // BRAVE_BROWSER_UI_VIEWS_FRAME_BRAVE_BROWSER_VIEW_H_

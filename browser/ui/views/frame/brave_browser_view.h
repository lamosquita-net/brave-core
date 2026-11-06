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

#include "base/gtest_prod_util.h"
#include "base/memory/raw_ptr.h"
#include "base/memory/weak_ptr.h"
#include "base/scoped_observation.h"
#include "brave/browser/ui/commands/accelerator_service.h"
#include "brave/browser/ui/tabs/brave_tab_strip_model.h"
#include "brave/components/commands/browser/accelerator_pref_manager.h"
#include "build/build_config.h"
#include "chrome/browser/ui/views/frame/browser_view.h"
#include "ui/base/accelerators/accelerator.h"
#include "ui/base/metadata/metadata_header_macros.h"

namespace content {
class WebContents;
}  // namespace content

class BraveBrowser;
class ViewShadow;
class BraveHelpBubbleHostView;

class BraveBrowserView : public BrowserView,
                         public commands::AcceleratorService::Observer {
  METADATA_HEADER(BraveBrowserView, BrowserView)
 public:
  explicit BraveBrowserView(std::unique_ptr<Browser> browser);
  BraveBrowserView(const BraveBrowserView&) = delete;
  BraveBrowserView& operator=(const BraveBrowserView&) = delete;
  ~BraveBrowserView() override;

  void SetStarredState(bool is_starred) override;
  void ShowUpdateChromeDialog() override;

  // BrowserView overrides:
  void Layout(PassKey) override;
  void StartTabCycling() override;
  gfx::Rect GetShieldsBubbleRect() override;
  bool AcceleratorPressed(const ui::Accelerator& accelerator) override;
  bool IsInTabDragging() const override;

#if defined(USE_AURA)
  views::View* side_panel_host_view() { return side_panel_host_view_; }
#endif

  bool ShowBraveHelpBubbleView(const std::string& text) override;

  // commands::AcceleratorService:
  void OnAcceleratorsChanged(const commands::Accelerators& changed) override;

 private:
  class TabCyclingEventHandler;
  friend class WindowClosingConfirmBrowserTest;

  static void SetDownloadConfirmReturnForTesting(bool allow);

  // BrowserView overrides:
  void AddedToWidget() override;
  void LoadAccelerators() override;
  void OnTabStripModelChanged(
      TabStripModel* tab_strip_model,
      const TabStripModelChange& change,
      const TabStripSelectionChange& selection) override;
  views::CloseRequestResult OnWindowCloseRequested() override;
  void ConfirmBrowserCloseWithPendingDownloads(
      int download_count,
      Browser::DownloadCloseType dialog_type,
      base::OnceCallback<void(bool)> callback) override;
  void MaybeShowReadingListInSidePanelIPH() override;
  void UpdateDevToolsForContents(content::WebContents* web_contents,
                                 bool update_devtools_web_contents) override;

  void StopTabCycling();
  void UpdateSearchTabsButtonState();
  void OnPreferenceChanged(const std::string& pref_name);
  void OnWindowClosingConfirmResponse(bool allowed_to_close);
  BraveBrowser* GetBraveBrowser() const;
  void UpdateWebViewRoundedCorners();

  bool HasSelectedURL() const override;
  void CleanAndCopySelectedURL() override;

  bool closing_confirm_dialog_activated_ = false;
  raw_ptr<BraveHelpBubbleHostView> brave_help_bubble_host_view_ = nullptr;
  raw_ptr<views::View> contents_background_view_ = nullptr;

#if defined(USE_AURA)
  raw_ptr<views::View> side_panel_host_view_ = nullptr;
#endif

  std::unique_ptr<TabCyclingEventHandler> tab_cycling_event_handler_;
  std::unique_ptr<ViewShadow> contents_shadow_;

  PrefChangeRegistrar pref_change_registrar_;
  base::ScopedObservation<commands::AcceleratorService,
                          commands::AcceleratorService::Observer>
      accelerators_observation_{this};

  base::WeakPtrFactory<BraveBrowserView> weak_ptr_{this};
};

#endif  // BRAVE_BROWSER_UI_VIEWS_FRAME_BRAVE_BROWSER_VIEW_H_

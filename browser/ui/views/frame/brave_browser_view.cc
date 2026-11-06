/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "brave/browser/ui/views/frame/brave_browser_view.h"

#include <algorithm>
#include <iterator>
#include <map>
#include <memory>
#include <optional>
#include <utility>
#include <vector>

#include "base/feature_list.h"
#include "base/functional/bind.h"
#include "base/task/sequenced_task_runner.h"
#include "brave/browser/sparkle_buildflags.h"
#include "brave/browser/translate/brave_translate_utils.h"
#include "brave/browser/ui/brave_browser.h"
#include "brave/browser/ui/commands/accelerator_service.h"
#include "brave/browser/ui/commands/accelerator_service_factory.h"
#include "brave/browser/ui/tabs/brave_tab_prefs.h"
#include "brave/browser/ui/tabs/features.h"
#include "brave/browser/ui/views/brave_actions/brave_actions_container.h"
#include "brave/browser/ui/views/brave_actions/brave_shields_action_view.h"
#include "brave/browser/ui/views/brave_help_bubble/brave_help_bubble_host_view.h"
#include "brave/browser/ui/views/location_bar/brave_location_bar_view.h"
#include "brave/browser/ui/views/omnibox/brave_omnibox_view_views.h"
#include "brave/browser/ui/views/toolbar/bookmark_button.h"
#include "brave/browser/ui/views/toolbar/brave_toolbar_view.h"
#include "brave/browser/ui/views/window_closing_confirm_dialog_view.h"
#include "brave/components/commands/common/features.h"
#include "chrome/app/chrome_command_ids.h"
#include "chrome/browser/app_mode/app_mode_utils.h"
#include "chrome/browser/ui/bookmarks/bookmark_tab_helper.h"
#include "chrome/browser/ui/browser_commands.h"
#include "chrome/browser/ui/browser_window/public/browser_window_features.h"
#include "chrome/browser/ui/browser_window/public/browser_window_interface.h"
#include "chrome/browser/ui/frame/window_frame_util.h"
#include "chrome/browser/ui/tabs/tab_strip_model.h"
#include "chrome/browser/ui/views/frame/browser_view.h"
#include "chrome/browser/ui/views/frame/browser_widget.h"
#include "chrome/browser/ui/views/frame/top_container_view.h"
#include "chrome/browser/ui/views/toolbar/browser_app_menu_button.h"
#include "chrome/common/pref_names.h"
#include "components/tabs/public/tab_interface.h"
#include "content/public/browser/page_navigator.h"
#include "content/public/browser/web_contents.h"
#include "extensions/buildflags/buildflags.h"
#include "ui/base/accelerators/accelerator.h"
#include "ui/base/accelerators/accelerator_manager.h"
#include "ui/base/metadata/metadata_header_macros.h"
#include "ui/base/metadata/metadata_impl_macros.h"
#include "ui/events/event_observer.h"
#include "ui/views/bubble/bubble_dialog_delegate_view.h"
#include "ui/views/event_monitor.h"

#if BUILDFLAG(ENABLE_SPARKLE)
#include "brave/browser/ui/views/update_recommended_message_box_mac.h"
#endif

namespace {

std::optional<bool> g_download_confirm_return_allow_for_testing;

bool IsUnsupportedCommand(int command_id, BrowserWindowInterface* browser) {
  return IsRunningInForcedAppMode() &&
         !IsCommandAllowedInAppMode(
             command_id,
             browser->GetType() == BrowserWindowInterface::Type::TYPE_POPUP);
}

}  // namespace

// static
void BraveBrowserView::SetDownloadConfirmReturnForTesting(bool allow) {
  g_download_confirm_return_allow_for_testing = allow;
}

class BraveBrowserView::TabCyclingEventHandler : public ui::EventObserver,
                                                 public views::WidgetObserver {
 public:
  explicit TabCyclingEventHandler(BraveBrowserView* browser_view)
      : browser_view_(browser_view) {
    Start();
  }

  ~TabCyclingEventHandler() override { Stop(); }

  TabCyclingEventHandler(const TabCyclingEventHandler&) = delete;
  TabCyclingEventHandler& operator=(const TabCyclingEventHandler&) = delete;

 private:
  // ui::EventObserver overrides:
  void OnEvent(const ui::Event& event) override {
    if (event.type() == ui::EventType::kKeyReleased &&
        event.AsKeyEvent()->key_code() == ui::VKEY_CONTROL) {
      // Ctrl key was released, stop the tab cycling
      Stop();
      return;
    }

    if (event.type() == ui::EventType::kMousePressed) {
      Stop();
    }
  }

  // views::WidgetObserver overrides:
  void OnWidgetActivationChanged(views::Widget* widget, bool active) override {
    // We should stop cycling if other application gets active state.
    if (!active) {
      Stop();
    }
  }

  // Handle Browser widget closing while tab Cycling is in-progress.
  void OnWidgetClosing(views::Widget* widget) override { Stop(); }

  void Start() {
    // Add the event handler
    auto* widget = browser_view_->GetWidget();
    if (widget->GetNativeWindow()) {
      monitor_ = views::EventMonitor::CreateWindowMonitor(
          this, widget->GetNativeWindow(),
          {ui::EventType::kMousePressed, ui::EventType::kKeyReleased});
    }

    widget->AddObserver(this);
  }

  void Stop() {
    if (!monitor_.get()) {
      // We already stopped
      return;
    }

    // Remove event handler
    auto* widget = browser_view_->GetWidget();
    monitor_.reset();
    widget->RemoveObserver(this);
    browser_view_->StopTabCycling();
  }

  raw_ptr<BraveBrowserView> browser_view_ = nullptr;
  std::unique_ptr<views::EventMonitor> monitor_;
};

BraveBrowserView::BraveBrowserView(BrowserWindowInterface* browser)
    : BrowserView(browser) {}

BraveBrowserView::~BraveBrowserView() {
  tab_cycling_event_handler_.reset();
  DCHECK(!tab_cycling_event_handler_);
}

gfx::Rect BraveBrowserView::GetShieldsBubbleRect() {
  auto* brave_location_bar_view =
      static_cast<BraveLocationBarView*>(GetLocationBarView());
  if (!brave_location_bar_view) {
    return gfx::Rect();
  }

  auto* shields_action_view =
      brave_location_bar_view->brave_actions_contatiner_view()
          ->GetShieldsActionView();
  if (!shields_action_view) {
    return gfx::Rect();
  }

  auto* bubble_widget = shields_action_view->GetBubbleWidget();
  if (!bubble_widget) {
    return gfx::Rect();
  }

  return bubble_widget->GetClientAreaBoundsInScreen();
}

void BraveBrowserView::SetStarredState(bool is_starred) {
  BraveBookmarkButton* button =
      static_cast<BraveToolbarView*>(toolbar())->bookmark_button();
  if (button) {
    button->SetToggled(is_starred);
  }
}

void BraveBrowserView::URLStarredChanged(content::WebContents* web_contents,
                                         bool starred) {
  if (web_contents == GetActiveWebContents()) {
    SetStarredState(starred);
  }
}

void BraveBrowserView::ObserveBookmarkTabHelper(
    content::WebContents* contents) {
  bookmark_tab_helper_observation_.Reset();
  if (auto* bookmark_helper =
          contents ? BookmarkTabHelper::FromWebContents(contents) : nullptr) {
    bookmark_tab_helper_observation_.Observe(bookmark_helper);
    SetStarredState(bookmark_helper->is_starred());
  } else {
    SetStarredState(false);
  }
}

void BraveBrowserView::OnActiveTabWillDiscardContents(
    tabs::TabInterface* tab,
    content::WebContents* old_contents,
    content::WebContents* new_contents) {
  ObserveBookmarkTabHelper(new_contents);
}

void BraveBrowserView::OnActiveTabWillDetach(
    tabs::TabInterface* tab,
    tabs::TabInterface::DetachReason reason) {
  bookmark_tab_helper_observation_.Reset();
}

void BraveBrowserView::OnActiveTabChanged(content::WebContents* old_contents,
                                          content::WebContents* new_contents,
                                          int index,
                                          int reason) {
  BrowserView::OnActiveTabChanged(old_contents, new_contents, index, reason);

  ObserveBookmarkTabHelper(new_contents);
  active_tab_will_discard_contents_subscription_ = {};
  active_tab_will_detach_subscription_ = {};
  if (auto* tab = new_contents
                      ? tabs::TabInterface::GetFromContents(new_contents)
                      : nullptr) {
    active_tab_will_discard_contents_subscription_ =
        tab->RegisterWillDiscardContents(base::BindRepeating(
            &BraveBrowserView::OnActiveTabWillDiscardContents,
            base::Unretained(this)));
    active_tab_will_detach_subscription_ =
        tab->RegisterWillDetach(base::BindRepeating(
            &BraveBrowserView::OnActiveTabWillDetach, base::Unretained(this)));
  }
}

void BraveBrowserView::ShowUpdateChromeDialog() {
#if BUILDFLAG(ENABLE_SPARKLE)
  // On mac, sparkle frameworks's relaunch api is used.
  UpdateRecommendedMessageBoxMac::Show(GetNativeWindow());
#else
  BrowserView::ShowUpdateChromeDialog();
#endif
}

bool BraveBrowserView::HasSelectedURL() const {
  if (!GetLocationBarView() || !GetLocationBarView()->HasFocus()) {
    return false;
  }
  auto* brave_omnibox_view =
      static_cast<BraveOmniboxViewViews*>(GetLocationBarView()->omnibox_view());
  return brave_omnibox_view && brave_omnibox_view->SelectedTextIsURL();
}

void BraveBrowserView::CleanAndCopySelectedURL() {
  if (!GetLocationBarView()) {
    return;
  }
  auto* brave_omnibox_view =
      static_cast<BraveOmniboxViewViews*>(GetLocationBarView()->omnibox_view());
  if (!brave_omnibox_view) {
    return;
  }
  brave_omnibox_view->CleanAndCopySelectedURL();
}

void BraveBrowserView::OnAcceleratorsChanged(
    const commands::Accelerators& changed) {
  DCHECK(base::FeatureList::IsEnabled(commands::features::kBraveCommands));

  auto* focus_manager = GetFocusManager();
  DCHECK(focus_manager);

  for (const auto& [command_id, accelerators] : changed) {
    if (IsUnsupportedCommand(command_id, browser())) {
      continue;
    }

    std::vector<ui::Accelerator> old_accelerators;
    for (const auto& [accelerator, accelerator_command] : accelerator_table_) {
      if (accelerator_command != command_id) {
        continue;
      }
      old_accelerators.push_back(accelerator);
    }

    // Register current accelerators
    for (const auto& accelerator : accelerators) {
      if (focus_manager->IsAcceleratorRegistered(accelerator)) {
        focus_manager->UnregisterAccelerator(accelerator, this);
      }

      focus_manager->RegisterAccelerator(
          accelerator, ui::AcceleratorManager::kNormalPriority, this);
      accelerator_table_[accelerator] = command_id;
    }

    // Unregister removed accelerators
    for (const auto& old_accelerator : old_accelerators) {
      if (std::ranges::contains(accelerators, old_accelerator)) {
        continue;
      }
      focus_manager->UnregisterAccelerator(old_accelerator, this);
      accelerator_table_.erase(old_accelerator);
    }
  }
}

bool BraveBrowserView::ShowBraveHelpBubbleView(const std::string& text) {
  auto* shields_action_view =
      static_cast<BraveLocationBarView*>(GetLocationBarView())
          ->brave_actions_contatiner_view()
          ->GetShieldsActionView();
  if (!shields_action_view || !shields_action_view->GetVisible()) {
    return false;
  }

  // When help bubble is closed, this host view gets hidden.
  // For now, this help bubble host view is only used for shield icon, but it
  // could be re-used for other icons or views in the future.
  if (!brave_help_bubble_host_view_) {
    brave_help_bubble_host_view_ =
        AddChildView(std::make_unique<BraveHelpBubbleHostView>());
  }
  brave_help_bubble_host_view_->set_text(text);
  brave_help_bubble_host_view_->set_tracked_element(shields_action_view);
  return brave_help_bubble_host_view_->Show();
}

void BraveBrowserView::LoadAccelerators() {
  if (base::FeatureList::IsEnabled(commands::features::kBraveCommands)) {
    auto* accelerator_service =
        commands::AcceleratorServiceFactory::GetForContext(
            browser()->GetProfile());
    if (accelerator_service) {
      accelerators_observation_.Observe(accelerator_service);
      return;
    }
  }
  BrowserView::LoadAccelerators();
}

void BraveBrowserView::OnTabStripModelChanged(
    TabStripModel* tab_strip_model,
    const TabStripModelChange& change,
    const TabStripSelectionChange& selection) {
  BrowserView::OnTabStripModelChanged(tab_strip_model, change, selection);

  if (change.type() != TabStripModelChange::kSelectionOnly) {
    // Stop tab cycling if tab is closed dusing the cycle.
    // This can happen when tab is closed by shortcut (ex, ctrl + F4).
    // After stopping, current tab cycling, new tab cycling will be started.
    StopTabCycling();
  }

  if (selection.active_tab_changed() && brave_help_bubble_host_view_ &&
      brave_help_bubble_host_view_->GetVisible()) {
    brave_help_bubble_host_view_->Hide();
  }
}

views::CloseRequestResult BraveBrowserView::OnWindowCloseRequested() {
  if (GetBraveBrowser()->ShouldAskForBrowserClosingBeforeHandlers()) {
    if (!closing_confirm_dialog_activated_) {
      WindowClosingConfirmDialogView::Show(
          GetBraveBrowser(),
          base::BindOnce(&BraveBrowserView::OnWindowClosingConfirmResponse,
                         weak_ptr_.GetWeakPtr()));
      closing_confirm_dialog_activated_ = true;
    }
    return views::CloseRequestResult::kCannotClose;
  }

  return BrowserView::OnWindowCloseRequested();
}

void BraveBrowserView::OnWindowClosingConfirmResponse(bool allowed_to_close) {
  DCHECK(closing_confirm_dialog_activated_);
  closing_confirm_dialog_activated_ = false;

  auto* browser = GetBraveBrowser();
  // Set to Browser instance because Browser instance knows about the result
  // of any warning handlers or beforeunload handlers.
  browser->set_confirmed_to_close(allowed_to_close);
  if (allowed_to_close) {
    // Start close window again as user allowed to close it.
    // Confirm dialog will not be launched for this closing request
    // as we set BraveBrowser::confirmed_to_closed_window_ to true.
    // If user cancels this window closing via additional warnings
    // or beforeunload handler, this dialog will be shown again.
    chrome::CloseWindow(browser);
  }
}

void BraveBrowserView::ConfirmBrowserCloseWithPendingDownloads(
    int download_count,
    DownloadCloseType dialog_type,
    base::OnceCallback<void(bool)> callback) {
  // Simulate user response.
  if (g_download_confirm_return_allow_for_testing) {
    base::SequencedTaskRunner::GetCurrentDefault()->PostTask(
        FROM_HERE,
        base::BindOnce(std::move(callback),
                       *g_download_confirm_return_allow_for_testing));
    return;
  }
  BrowserView::ConfirmBrowserCloseWithPendingDownloads(
      download_count, dialog_type, std::move(callback));
}

bool BraveBrowserView::AcceleratorPressed(const ui::Accelerator& accelerator) {
  if (base::FeatureList::IsEnabled(tabs::features::kBraveSharedPinnedTabs) &&
      browser()->GetProfile()->GetPrefs()->GetBoolean(
          brave_tabs::kSharedPinnedTab)) {
    if (int command_id; FindCommandIdForAccelerator(accelerator, &command_id) &&
                        command_id == IDC_CLOSE_TAB) {
      auto* tab_strip_model = browser()->GetTabStripModel();
      if (tab_strip_model->IsTabPinned(tab_strip_model->active_index())) {
        // Ignore CLOSE TAB command via accelerator if the tab is shared/dummy
        // pinned tab.
        return true;
      }
    }
  }
  return BrowserView::AcceleratorPressed(accelerator);
}

bool BraveBrowserView::IsInTabDragging() const {
  return browser_widget()->tab_drag_kind() == TabDragKind::kAllTabs;
}

BraveBrowser* BraveBrowserView::GetBraveBrowser() const {
  return static_cast<BraveBrowser*>(browser_.get());
}

void BraveBrowserView::StartTabCycling() {
  tab_cycling_event_handler_ = std::make_unique<TabCyclingEventHandler>(this);
}

void BraveBrowserView::StopTabCycling() {
  tab_cycling_event_handler_.reset();
  static_cast<BraveTabStripModel*>(browser()->GetTabStripModel())
      ->StopMRUCycling();
}

BEGIN_METADATA(BraveBrowserView)
END_METADATA

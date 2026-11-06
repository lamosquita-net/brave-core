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
#include "brave/browser/ui/views/brave_shields/cookie_list_opt_in_bubble_host.h"
#include "brave/browser/ui/views/frame/brave_contents_view_util.h"
#include "brave/browser/ui/views/frame/vertical_tab_strip_region_view.h"
#include "brave/browser/ui/views/frame/vertical_tab_strip_widget_delegate_view.h"
#include "brave/browser/ui/views/location_bar/brave_location_bar_view.h"
#include "brave/browser/ui/views/omnibox/brave_omnibox_view_views.h"
#include "brave/browser/ui/views/tabs/vertical_tab_utils.h"
#include "brave/browser/ui/views/toolbar/bookmark_button.h"
#include "brave/browser/ui/views/toolbar/brave_toolbar_view.h"
#include "brave/browser/ui/views/window_closing_confirm_dialog_view.h"
#include "brave/components/commands/common/features.h"
#include "brave/components/constants/pref_names.h"
#include "chrome/app/chrome_command_ids.h"
#include "chrome/browser/app_mode/app_mode_utils.h"
#include "chrome/browser/ui/browser_commands.h"
#include "chrome/browser/ui/browser_window/public/browser_window_features.h"
#include "chrome/browser/ui/color/chrome_color_id.h"
#include "chrome/browser/ui/exclusive_access/exclusive_access_manager.h"
#include "chrome/browser/ui/exclusive_access/fullscreen_controller.h"
#include "chrome/browser/ui/frame/window_frame_util.h"
#include "chrome/browser/ui/views/frame/browser_view.h"
#include "chrome/browser/ui/views/frame/browser_widget.h"
#include "chrome/browser/ui/views/frame/tab_strip_region_view.h"
#include "chrome/browser/ui/views/frame/top_container_view.h"
#include "chrome/browser/ui/views/tabs/tab_search_button.h"
#include "chrome/browser/ui/views/tabs/tab_strip.h"
#include "chrome/browser/ui/views/toolbar/browser_app_menu_button.h"
#include "chrome/common/pref_names.h"
#include "content/public/browser/page_navigator.h"
#include "content/public/browser/web_contents.h"
#include "extensions/buildflags/buildflags.h"
#include "ui/base/accelerators/accelerator.h"
#include "ui/base/accelerators/accelerator_manager.h"
#include "ui/base/metadata/metadata_header_macros.h"
#include "ui/base/metadata/metadata_impl_macros.h"
#include "ui/compositor/layer.h"
#include "ui/events/event_observer.h"
#include "ui/gfx/geometry/rounded_corners_f.h"
#include "ui/views/bubble/bubble_dialog_delegate_view.h"
#include "ui/views/controls/native/native_view_host.h"
#include "ui/views/event_monitor.h"
#include "ui/views/layout/fill_layout.h"

#if BUILDFLAG(ENABLE_SPARKLE)
#include "brave/browser/ui/views/update_recommended_message_box_mac.h"
#endif

namespace {

std::optional<bool> g_download_confirm_return_allow_for_testing;

bool IsUnsupportedCommand(int command_id, Browser* browser) {
  return chrome::IsRunningInForcedAppMode() &&
         !chrome::IsCommandAllowedInAppMode(command_id,
                                            browser->is_type_popup());
}

// A view that paints a background under the content area of the browser view so
// that the web content area can be displayed with rounded corners and a shadow.
class ContentsBackground : public views::View {
  METADATA_HEADER(ContentsBackground, views::View)
 public:
  ContentsBackground() {
    SetBackground(views::CreateThemedSolidBackground(kColorToolbar));
    SetEnabled(false);
  }
};
BEGIN_METADATA(ContentsBackground)
END_METADATA

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

BraveBrowserView::BraveBrowserView(std::unique_ptr<Browser> browser)
    : BrowserView(std::move(browser)) {
  if (BraveBrowser::ShouldUseBraveWebViewRoundedCorners(browser_.get())) {
    // Collapse the separator line between the toolbar or bookmark bar and the
    // views below.
    contents_separator_->SetPreferredSize(gfx::Size());
    contents_shadow_ = BraveContentsViewUtil::CreateShadow(contents_container_);
    contents_background_view_ =
        AddChildView(std::make_unique<ContentsBackground>());
  }

  pref_change_registrar_.Init(GetProfile()->GetPrefs());
  pref_change_registrar_.Add(
      kTabsSearchShow,
      base::BindRepeating(&BraveBrowserView::OnPreferenceChanged,
                          base::Unretained(this)));
  // Show the correct value in settings on initial start
  UpdateSearchTabsButtonState();

  brave_shields::CookieListOptInBubbleHost::MaybeCreateForBrowser(
      browser_.get());

#if defined(USE_AURA)
  // Host view for the side panel resize widget.
  if (browser_->is_type_normal()) {
    side_panel_host_view_ = AddChildView(std::make_unique<views::View>());
  }
#endif

  const bool supports_vertical_tabs =
      tabs::utils::SupportsVerticalTabs(browser_.get());
  if (supports_vertical_tabs) {
    vertical_tab_strip_host_view_ =
        AddChildView(std::make_unique<views::View>());
  }

  if (!supports_vertical_tabs && !side_panel_host_view_) {
    return;
  }

  // Make sure |find_bar_host_view_| is the last child of BrowserView by
  // re-ordering. FindBarHost widgets uses this view as a  kHostViewKey.
  // See the comments of BrowserView::find_bar_host_view().
  ReorderChildView(find_bar_host_view_, -1);
}

void BraveBrowserView::OnPreferenceChanged(const std::string& pref_name) {
  if (pref_name == kTabsSearchShow) {
    UpdateSearchTabsButtonState();
    return;
  }
}

void BraveBrowserView::UpdateSearchTabsButtonState() {
  if (auto* tab_search_container =
          tab_strip_region_view()->tab_search_container()) {
    if (auto* button = tab_search_container->tab_search_button()) {
      auto is_tab_search_visible =
          GetProfile()->GetPrefs()->GetBoolean(kTabsSearchShow);
      button->SetVisible(is_tab_search_visible);
    }
  }
}

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

bool BraveBrowserView::GetTabStripVisible() const {
  if (tabs::utils::ShouldShowVerticalTabs(browser())) {
    return false;
  }

  return BrowserView::GetTabStripVisible();
}

#if BUILDFLAG(IS_WIN)
bool BraveBrowserView::GetSupportsTitle() const {
  if (tabs::utils::SupportsVerticalTabs(browser())) {
    return true;
  }

  return BrowserView::GetSupportsTitle();
}
#endif

void BraveBrowserView::SetStarredState(bool is_starred) {
  BraveBookmarkButton* button =
      static_cast<BraveToolbarView*>(toolbar())->bookmark_button();
  if (button) {
    button->SetToggled(is_starred);
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

void BraveBrowserView::AddedToWidget() {
  BrowserView::AddedToWidget();
  // we must call all new views once BraveBrowserView is added to widget

  GetBrowserViewLayout()->set_contents_background(contents_background_view_);

  UpdateWebViewRoundedCorners();

  if (vertical_tab_strip_host_view_) {
    vertical_tab_strip_widget_delegate_view_ =
        VerticalTabStripWidgetDelegateView::Create(
            this, vertical_tab_strip_host_view_);

    // By setting this property to the widget for vertical tabs,
    // BrowserView::GetBrowserViewForNativeWindow() will return browser view
    // properly even when we pass the native window for vertical tab strip.
    // As a result, we don't have to call GetTopLevelWidget() in order to
    // get browser view from the vertical tab strip's widget.
    SetNativeWindowPropertyForWidget(
        vertical_tab_strip_widget_delegate_view_->GetWidget());

    GetBrowserViewLayout()->set_vertical_tab_strip_host(
        vertical_tab_strip_host_view_.get());
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
          browser(),
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
    Browser::DownloadCloseType dialog_type,
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

void BraveBrowserView::MaybeShowReadingListInSidePanelIPH() {
  // Do nothing.
}

void BraveBrowserView::UpdateDevToolsForContents(
    content::WebContents* web_contents,
    bool update_devtools_web_contents) {
  BrowserView::UpdateDevToolsForContents(web_contents,
                                         update_devtools_web_contents);
  UpdateWebViewRoundedCorners();
}

bool BraveBrowserView::ShouldShowWindowTitle() const {
  if (BrowserView::ShouldShowWindowTitle()) {
    return true;
  }

  if (tabs::utils::ShouldShowWindowTitleForVerticalTabs(browser())) {
    return true;
  }

  return false;
}

void BraveBrowserView::OnThemeChanged() {
  BrowserView::OnThemeChanged();
  if (vertical_tab_strip_host_view_) {
    const auto background_color = GetColorProvider()->GetColor(kColorToolbar);
    vertical_tab_strip_host_view_->SetBackground(
        views::CreateSolidBackground(background_color));
  }
}

TabSearchBubbleHost* BraveBrowserView::GetTabSearchBubbleHost() {
  if (!tabs::utils::ShouldShowVerticalTabs(browser())) {
    return BrowserView::GetTabSearchBubbleHost();
  }

  return vertical_tab_strip_widget_delegate_view_
      ->vertical_tab_strip_region_view()
      ->GetTabSearchBubbleHost();
}

bool BraveBrowserView::AcceleratorPressed(const ui::Accelerator& accelerator) {
  if (base::FeatureList::IsEnabled(tabs::features::kBraveSharedPinnedTabs) &&
      browser()->GetProfile()->GetPrefs()->GetBoolean(
          brave_tabs::kSharedPinnedTab)) {
    if (int command_id; FindCommandIdForAccelerator(accelerator, &command_id) &&
                        command_id == IDC_CLOSE_TAB) {
      auto* tab_strip_model = browser()->tab_strip_model();
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

void BraveBrowserView::UpdateWebViewRoundedCorners() {
  if (!BraveBrowser::ShouldUseBraveWebViewRoundedCorners(browser_.get())) {
    return;
  }

  gfx::RoundedCornersF corners(BraveContentsViewUtil::kBorderRadius);

  // In fullscreen-for-tab mode (e.g. full-screen video), no corners should be
  // rounded.
  if (auto* exclusive_access_manager = GetExclusiveAccessManager()) {
    if (auto* controller = exclusive_access_manager->fullscreen_controller()) {
      if (controller->IsWindowFullscreenForTabOrPending()) {
        corners = gfx::RoundedCornersF(0);
      }
    }
  }

  // Set the appropriate corner radius for the view that contains both the web
  // contents and devtools.
  contents_container_->layer()->SetRoundedCornerRadius(corners);

  // In addition to giving the contents container rounded corners, we also
  // need to round the corners of the native view holder that displays the web
  // contents.

  // Devtools lies underneath the contents webview. Round all four corners.
  if (auto* devtools_holder = devtools_web_view_->holder()) {
    devtools_holder->SetCornerRadii(corners);
  }

  // In order to make the contents web view and devtools appear to be
  // contained within a single rounded-corner view, square the contents
  // webview corners that are adjacent to devtools.
  switch (devtools_docked_placement()) {
    case DevToolsDockedPlacement::kLeft:
      corners.set_upper_left(0);
      corners.set_lower_left(0);
      break;
    case DevToolsDockedPlacement::kRight:
      corners.set_upper_right(0);
      corners.set_lower_right(0);
      break;
    case DevToolsDockedPlacement::kBottom:
      corners.set_lower_left(0);
      corners.set_lower_right(0);
      break;
    case DevToolsDockedPlacement::kNone:
      break;
    case DevToolsDockedPlacement::kUnknown:
      break;
  }

  if (auto* contents_holder = contents_web_view_->holder()) {
    contents_holder->SetCornerRadii(corners);
  }
}

void BraveBrowserView::Layout(PassKey) {
  LayoutSuperclass<BrowserView>(this);
  UpdateWebViewRoundedCorners();
}

void BraveBrowserView::StartTabCycling() {
  tab_cycling_event_handler_ = std::make_unique<TabCyclingEventHandler>(this);
}

void BraveBrowserView::StopTabCycling() {
  tab_cycling_event_handler_.reset();
  static_cast<BraveTabStripModel*>(browser()->tab_strip_model())
      ->StopMRUCycling();
}

BEGIN_METADATA(BraveBrowserView)
END_METADATA

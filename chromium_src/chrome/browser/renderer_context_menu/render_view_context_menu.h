/* Copyright (c) 2018 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_CHROMIUM_SRC_CHROME_BROWSER_RENDERER_CONTEXT_MENU_RENDER_VIEW_CONTEXT_MENU_H_
#define BRAVE_CHROMIUM_SRC_CHROME_BROWSER_RENDERER_CONTEXT_MENU_RENDER_VIEW_CONTEXT_MENU_H_

#include "brave/components/text_recognition/common/buildflags/buildflags.h"

#define BRAVE_RENDER_VIEW_CONTEXT_MENU_H_ \
  private: \
    friend RenderViewContextMenu_BraveImpl; \
  public:
// define BRAVE_RENDER_VIEW_CONTEXT_MENU_H_

// As 1.98: our subclass takes the upstream name, so every user of
// RenderViewContextMenu gets it without a trailing #define (which broke
// render_view_context_menu_browsertest_util.cc). The alias names it inside
// the block where RenderViewContextMenu means the Chromium class.
class RenderViewContextMenu;
using RenderViewContextMenu_BraveImpl = RenderViewContextMenu;

// Get the Chromium declaration.
#define RenderViewContextMenu RenderViewContextMenu_Chromium

#define RegisterMenuShownCallbackForTesting                      \
  RegisterMenuShownCallbackForTesting(                           \
      base::OnceCallback<void(RenderViewContextMenu_BraveImpl*)> cb); \
  static void RegisterMenuShownCallbackForTesting_unused
#include "src/chrome/browser/renderer_context_menu/render_view_context_menu.h"  // IWYU pragma: export
#undef RegisterMenuShownCallbackForTesting
#undef RenderViewContextMenu
#undef BRAVE_RENDER_VIEW_CONTEXT_MENU_H_

// Declare our own subclass with overridden methods.
class RenderViewContextMenu : public RenderViewContextMenu_Chromium {
 public:
  // Non-const reference passed in the parent class upstream
  // NOLINTNEXTLINE(runtime/references)
  RenderViewContextMenu(content::RenderFrameHost& render_frame_host,
                             const content::ContextMenuParams& params,
                             bool is_paste_enabled,
                             bool is_paste_and_match_style_enabled);
  ~RenderViewContextMenu() override;
  // RenderViewContextMenuBase:
  bool IsCommandIdEnabled(int command_id) const override;
  void ExecuteCommand(int id, int event_flags) override;
  void AddSpellCheckServiceItem(bool is_checked) override;
  // Hide base class implementation.
  static void AddSpellCheckServiceItem(ui::SimpleMenuModel* menu,
                                       bool is_checked);
  void AddAccessibilityLabelsServiceItem(bool is_checked) override;

 private:
  friend class BraveRenderViewContextMenuTest;
  // RenderViewContextMenuBase:
  void InitMenu() override;
  void NotifyMenuShown() override;

#if BUILDFLAG(ENABLE_TEXT_RECOGNITION)
  void CopyTextFromImage();
#endif
};

#endif  // BRAVE_CHROMIUM_SRC_CHROME_BROWSER_RENDERER_CONTEXT_MENU_RENDER_VIEW_CONTEXT_MENU_H_

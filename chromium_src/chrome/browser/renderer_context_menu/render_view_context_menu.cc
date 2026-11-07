/* Copyright (c) 2018 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "chrome/browser/renderer_context_menu/render_view_context_menu.h"
#include "chrome/browser/ui/browser_window/public/browser_window_interface.h"

#include <optional>

#include "base/containers/fixed_flat_map.h"
#include "base/containers/fixed_flat_set.h"
#include "base/strings/string_util.h"
#include "brave/browser/autocomplete/brave_autocomplete_scheme_classifier.h"
#include "brave/browser/profiles/profile_util.h"
#include "brave/browser/renderer_context_menu/brave_spelling_options_submenu_observer.h"
#include "brave/browser/ui/browser_commands.h"
#include "brave/browser/ui/browser_dialogs.h"
#include "brave/grit/brave_theme_resources.h"
#include "build/build_config.h"
#include "chrome/browser/autocomplete/chrome_autocomplete_provider_client.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/common/channel_info.h"
#include "components/omnibox/browser/autocomplete_classifier.h"
#include "components/omnibox/browser/autocomplete_controller.h"
#include "components/omnibox/browser/autocomplete_controller_config.h"
#include "components/omnibox/browser/autocomplete_match_type.h"
#include "content/public/browser/web_contents.h"
#include "net/base/filename_util.h"
#include "ui/base/models/menu_separator_types.h"
#include "ui/base/resource/resource_bundle.h"
#include "ui/gfx/image/image_skia.h"
#include "ui/gfx/paint_vector_icon.h"
#include "url/origin.h"

#if BUILDFLAG(IS_MAC)
#include "base/mac/mac_util.h"
#endif

// Inside the upstream .cc, RenderViewContextMenu must mean the Chromium
// class (our subclass takes that name in the .h, as 1.98).
#undef RenderViewContextMenu
#define RenderViewContextMenu RenderViewContextMenu_Chromium

namespace {

AutocompleteMatch GetAutocompleteMatchForText(Profile* profile,
                                              const std::u16string& text) {
  AutocompleteMatch match;
  AutocompleteClassifier classifier(
      std::make_unique<AutocompleteController>(
          std::make_unique<ChromeAutocompleteProviderClient>(profile),
          AutocompleteControllerConfig{
              .provider_types =
                  AutocompleteClassifier::DefaultOmniboxProviders()}),
      std::make_unique<BraveAutocompleteSchemeClassifier>(profile));
  classifier.Classify(text, false, false,
                      metrics::OmniboxEventProto::INVALID_SPEC, &match, NULL);
  classifier.Shutdown();
  return match;
}

GURL GetSelectionNavigationURL(Profile* profile, const std::u16string& text) {
  return GetAutocompleteMatchForText(profile, text).destination_url;
}

std::optional<GURL> GetSelectedURL(Profile* profile,
                                   const std::u16string& text) {
  auto match = GetAutocompleteMatchForText(profile, text);
  if (match.type != omnibox::AutocompleteMatchType::kUrlWhatYouTyped) {
    return std::nullopt;
  }
  return match.destination_url;
}

base::OnceCallback<void(RenderViewContextMenu_BraveImpl*)>*
BraveGetMenuShownCallback() {
  static base::NoDestructor<
      base::OnceCallback<void(RenderViewContextMenu_BraveImpl*)>>
      callback;
  return callback.get();
}

}  // namespace

void RenderViewContextMenu::RegisterMenuShownCallbackForTesting(
    base::OnceCallback<void(RenderViewContextMenu_BraveImpl*)> cb) {
  *BraveGetMenuShownCallback() = std::move(cb);
}

#define BRAVE_APPEND_SEARCH_PROVIDER                                     \
  if (GetProfile()->IsOffTheRecord()) {                                  \
    selection_navigation_url_ =                                          \
        GetSelectionNavigationURL(GetProfile(), params_.selection_text); \
    if (!selection_navigation_url_.is_valid())                           \
      return;                                                            \
  }

// Use our subclass to initialize SpellingOptionsSubMenuObserver.
#define SpellingOptionsSubMenuObserver BraveSpellingOptionsSubMenuObserver
#define RegisterMenuShownCallbackForTesting \
  RegisterMenuShownCallbackForTesting_unused

#include "src/chrome/browser/renderer_context_menu/render_view_context_menu.cc"

#undef SpellingOptionsSubMenuObserver
#undef RegisterMenuShownCallbackForTesting

// Make it clear which class we mean here.
#undef RenderViewContextMenu
#undef BRAVE_APPEND_SEARCH_PROVIDER

namespace {

#if BUILDFLAG(ENABLE_TEXT_RECOGNITION)
void OnGetImageForTextCopy(base::WeakPtr<content::WebContents> web_contents,
                           const SkBitmap& image) {
  if (!web_contents) {
    return;
  }

  brave::ShowTextRecognitionDialog(web_contents.get(), image);
}
#endif

}  // namespace

RenderViewContextMenu::RenderViewContextMenu(
    content::RenderFrameHost& render_frame_host,
    const content::ContextMenuParams& params,
    bool is_paste_enabled,
    bool is_paste_and_match_style_enabled)
    : RenderViewContextMenu_Chromium(render_frame_host,
                                     params,
                                     is_paste_enabled,
                                     is_paste_and_match_style_enabled) {}

RenderViewContextMenu::~RenderViewContextMenu() = default;

bool RenderViewContextMenu::IsCommandIdEnabled(int id) const {
  switch (id) {
#if BUILDFLAG(ENABLE_TEXT_RECOGNITION)
    case IDC_CONTENT_CONTEXT_COPY_TEXT_FROM_IMAGE:
      return params_.has_image_contents;
#endif
    case IDC_COPY_CLEAN_LINK:
      return params_.link_url.is_valid() ||
             GetSelectedURL(GetProfile(), params_.selection_text).has_value();
    case IDC_CONTENT_CONTEXT_FORCE_PASTE:
      // only enable if there is plain text data to paste - this is what
      // IsPasteAndMatchStyleEnabled checks internally, but IsPasteEnabled
      // allows non text types
      return IsPasteAndMatchStyleEnabled();
    default:
      return RenderViewContextMenu_Chromium::IsCommandIdEnabled(id);
  }
}

void RenderViewContextMenu::ExecuteCommand(int id, int event_flags) {
  switch (id) {
    case IDC_COPY_CLEAN_LINK: {
      GURL link_url = params_.link_url;
      if (!link_url.is_valid()) {
        auto selected_url =
            GetSelectedURL(GetProfile(), params_.selection_text);
        if (selected_url.has_value()) {
          link_url = selected_url.value();
        } else {
          return;
        }
      }
      brave::CopyLinkWithStrictCleaning(GetBrowser(), link_url);
    }; break;
    case IDC_CONTENT_CONTEXT_FORCE_PASTE: {
      std::unique_ptr<ui::DataTransferEndpoint> data_dst =
          CreateDataEndpoint(/*notify_if_restricted=*/true);
      ui::Clipboard::GetForCurrentThread()->ReadText(
          ui::ClipboardBuffer::kCopyPaste,
          data_dst ? std::make_optional(*data_dst) : std::nullopt,
          base::BindOnce(
              [](base::WeakPtr<content::WebContents> web_contents,
                 std::u16string result) {
                if (!web_contents || result.empty()) {
                  return;
                }
                // Replace works just like Paste, but it doesn't trigger
                // onpaste handlers.
                web_contents->Replace(result);
              },
              source_web_contents_->GetWeakPtr()));
    }; break;
#if BUILDFLAG(ENABLE_TEXT_RECOGNITION)
    case IDC_CONTENT_CONTEXT_COPY_TEXT_FROM_IMAGE:
      CopyTextFromImage();
      break;
#endif
    default:
      RenderViewContextMenu_Chromium::ExecuteCommand(id, event_flags);
  }
}

#if BUILDFLAG(ENABLE_TEXT_RECOGNITION)
void RenderViewContextMenu::CopyTextFromImage() {
  RenderFrameHost* frame_host = GetRenderFrameHost();
  if (frame_host) {
    frame_host->GetImageAt(params_.x, params_.y,
                           base::BindOnce(OnGetImageForTextCopy,
                                          source_web_contents_->GetWeakPtr()));
  }
}
#endif

void RenderViewContextMenu::AddSpellCheckServiceItem(bool is_checked) {
  // Call our implementation, not the one in the base class.
  // Assumption:
  // Use of spelling service is disabled in Brave profile preferences.
  DCHECK(!GetProfile()->GetPrefs()->GetBoolean(
      spellcheck::prefs::kSpellCheckUseSpellingService));
  AddSpellCheckServiceItem(&menu_model_, is_checked);
}

// static
void RenderViewContextMenu::AddSpellCheckServiceItem(
    ui::SimpleMenuModel* menu,
    bool is_checked) {
  // Suppress adding "Spellcheck->Ask Brave for suggestions" item.
}

void RenderViewContextMenu::AddAccessibilityLabelsServiceItem(
    bool is_checked) {
  // Suppress adding "Get image descriptions from Brave"
}

void RenderViewContextMenu::InitMenu() {
  RenderViewContextMenu_Chromium::InitMenu();

  std::optional<size_t> index = menu_model_.GetIndexOfCommandId(
      IDC_CONTENT_CONTEXT_PASTE_AND_MATCH_STYLE);
  if (index.has_value()) {
    menu_model_.InsertItemWithStringIdAt(index.value() + 1,
                                         IDC_CONTENT_CONTEXT_FORCE_PASTE,
                                         IDS_CONTENT_CONTEXT_FORCE_PASTE);
  }
#if BUILDFLAG(ENABLE_TEXT_RECOGNITION)
  bool media_image = content_type_->SupportsGroup(
      ContextMenuContentType::ITEM_GROUP_MEDIA_IMAGE);
#if BUILDFLAG(IS_MAC)
  // FlyWeb (FM.42 b1): Vision recognizes text only on macOS 10.15+; on 10.14
  // the item would copy nothing, so it is not shown.
  if (base::mac::MacOSVersion() < 10'15'00) {
    media_image = false;
  }
#endif
  if (media_image) {
    index =
        menu_model_.GetIndexOfCommandId(IDC_CONTENT_CONTEXT_COPYIMAGELOCATION);
    DCHECK(index);
    menu_model_.InsertItemWithStringIdAt(
        index.value() + 1, IDC_CONTENT_CONTEXT_COPY_TEXT_FROM_IMAGE,
        IDS_CONTENT_CONTEXT_COPY_TEXT_FROM_IMAGE);
  }
#endif

  if (!params_.link_url.is_empty() && params_.link_url.SchemeIsHTTPOrHTTPS()) {
    std::optional<size_t> link_index =
        menu_model_.GetIndexOfCommandId(IDC_CONTENT_CONTEXT_COPYLINKLOCATION);
    if (link_index.has_value()) {
      menu_model_.InsertItemWithStringIdAt(
          link_index.value() + 1, IDC_COPY_CLEAN_LINK, IDS_COPY_CLEAN_LINK);
    }
  }
  if (GetSelectedURL(GetProfile(), params_.selection_text).has_value()) {
    std::optional<size_t> copy_index =
        menu_model_.GetIndexOfCommandId(IDC_CONTENT_CONTEXT_COPY);
    if (copy_index.has_value() &&
        !menu_model_.GetIndexOfCommandId(IDC_COPY_CLEAN_LINK).has_value()) {
      menu_model_.InsertItemWithStringIdAt(
          copy_index.value() + 1, IDC_COPY_CLEAN_LINK, IDS_COPY_CLEAN_LINK);
    }
  }
}

void RenderViewContextMenu::NotifyMenuShown() {
  auto* cb = BraveGetMenuShownCallback();
  if (!cb->is_null()) {
    std::move(*cb).Run(this);
  }
}

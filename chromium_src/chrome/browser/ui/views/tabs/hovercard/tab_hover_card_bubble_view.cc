/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * you can obtain one at http://mozilla.org/MPL/2.0/. */

#include <string>

#include "base/strings/strcat.h"
#include "base/strings/string_util.h"
#include "base/strings/utf_string_conversions.h"
#include "brave/browser/ui/tabs/brave_tab_prefs.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/browser/ui/views/tabs/hovercard/fade_label_view.h"
#include "chrome/browser/ui/views/tabs/hovercard/tab_hover_card_bubble_view.h"
#include "chrome/browser/ui/views/tabs/hovercard/tab_hover_card_controller.h"
#include "chrome/browser/ui/views/tabs/tab.h"
#include "chrome/browser/ui/views/tabs/tab_slot_controller.h"
#include "content/public/common/url_constants.h"
#include "ui/views/controls/label.h"

#define TabHoverCardBubbleView TabHoverCardBubbleView_ChromiumImpl
#include "src/chrome/browser/ui/views/tabs/hovercard/tab_hover_card_bubble_view.cc"
#undef TabHoverCardBubbleView

bool TabHoverCardBubbleView_ChromiumImpl::HasThumbnailView() const {
  return tab_card_view_->thumbnail_view() != nullptr;
}

void TabHoverCardBubbleView_ChromiumImpl::BraveUpdateCardContent(
    const HoverCardAnchorTarget* anchor_target) {
  TabHoverCardBubbleView_ChromiumImpl::UpdateCardContent(anchor_target);
  const std::u16string domain(tab_card_view_->domain_label()->GetText());
  const std::u16string kChromeUISchemeU16 =
      base::ASCIIToUTF16(base::StrCat({content::kChromeUIScheme, "://"}));
  // Replace chrome:// with flyweb://. Since this is purely in the UI we can
  // just do a sub-string replacement instead of parsing into GURL.
  if (base::StartsWith(domain, kChromeUISchemeU16,
                       base::CompareCase::INSENSITIVE_ASCII)) {
    std::u16string new_domain = domain;
    base::ReplaceFirstSubstringAfterOffset(
        &new_domain, 0ul, kChromeUISchemeU16,
        base::ASCIIToUTF16(base::StrCat({content::kBraveUIScheme, "://"})));
    tab_card_view_->domain_label()->SetData(
        {new_domain, /*is_filename*/ false});
  }
}

void TabHoverCardBubbleView::UpdateCardContent(
    const HoverCardAnchorTarget* anchor_target) {
  BraveUpdateCardContent(anchor_target);
}

void TabHoverCardBubbleView::SetTargetTabImage(gfx::ImageSkia preview_image) {
  if (!HasThumbnailView()) {
    return;
  }
  TabHoverCardBubbleView_ChromiumImpl::SetTargetTabImage(preview_image);
}

void TabHoverCardBubbleView::SetPlaceholderImage() {
  if (!HasThumbnailView()) {
    return;
  }
  TabHoverCardBubbleView_ChromiumImpl::SetPlaceholderImage();
}

/* Copyright (c) 2025 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "content/browser/renderer_host/navigation_entry_impl.h"

#include <string>

#include "base/check.h"
#include "content/public/common/url_constants.h"

#define SetVirtualURL SetVirtualURL_ChromiumImpl
#define GetTitleForDisplay GetTitleForDisplay_ChromiumImpl

#include "src/content/browser/renderer_host/navigation_entry_impl.cc"

#undef GetTitleForDisplay
#undef SetVirtualURL

namespace content {

const std::u16string& NavigationEntryImpl::GetTitleForDisplay() const {
  const std::u16string& result = GetTitleForDisplay_ChromiumImpl();
  // flyweb:// (content::kBraveUIScheme) is a display-only scheme that maps to
  // chrome:// internally. When the page title is empty, GetTitleForDisplay
  // falls back to formatting the virtual URL as the display title. Since
  // flyweb:// URLs are stored as chrome:// internally, that fallback shows
  // "chrome://host" instead of "flyweb://host". Convert it for display.
  if (title_.empty() && !cached_display_title_.empty()) {
    static constexpr char16_t kChromePrefix[] = u"chrome://";
    static constexpr char16_t kFlyWebPrefix[] = u"flyweb://";
    if (cached_display_title_.starts_with(kChromePrefix)) {
      cached_display_title_.replace(0, std::size(kChromePrefix) - 1,
                                    kFlyWebPrefix);
    }
  }
  return result;
}

// Virtual url should never be set to flyweb://
void NavigationEntryImpl::SetVirtualURL(const GURL& url) {
  DCHECK(!url.SchemeIs(kBraveUIScheme))
      << "Virtual URL should not use flyweb:// scheme";
  SetVirtualURL_ChromiumImpl(url);
}

}  // namespace content

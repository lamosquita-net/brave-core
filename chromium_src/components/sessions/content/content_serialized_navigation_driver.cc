/* Copyright (c) 2022 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "components/sessions/content/content_serialized_navigation_driver.h"

#include <string>

#include "components/sessions/core/serialized_navigation_entry.h"
#include "content/public/common/url_constants.h"
#include "url/gurl.h"

#define GetSanitizedPageStateForPickle \
  GetSanitizedPageStateForPickle_ChromiumImpl
#define Sanitize Sanitize_ChromiumImpl

#include "src/components/sessions/content/content_serialized_navigation_driver.cc"

#undef Sanitize
#undef GetSanitizedPageStateForPickle

namespace sessions {

namespace {

// FlyWeb: flyweb:// (content::kBraveUIScheme) is a display-only alias of
// chrome://. Sessions written by FlyWeb 2.x (Chromium 116/128) may carry it,
// or the legacy brave:// spelling, as the stored URL.
constexpr char kLegacyBraveUIScheme[] = "brave";

bool IsDisplayOnlyUIScheme(const GURL& url) {
  return url.SchemeIs(content::kBraveUIScheme) ||
         url.SchemeIs(kLegacyBraveUIScheme);
}

}  // namespace

std::string ContentSerializedNavigationDriver::GetSanitizedPageStateForPickle(
    const sessions::SerializedNavigationEntry* navigation) const {
  const auto& virtual_url = navigation->virtual_url();
  // Unlike upstream, we clear PageState for all chrome:// URLs: a stale
  // PageState can outlive a page's own URL rewrite/cleanup (extension NTP
  // override, or a page's history.replaceState()) and get restored verbatim,
  // reintroducing a URL the page already moved past.
  if (virtual_url.SchemeIs(content::kChromeUIScheme)) {
    // Empty PageState is synthesized by
    // ContentSerializedNavigationBuilder::ToNavigationEntry() from the entry's
    // current rewritten URL, so it always matches whatever virtual_url resolves
    // to today.
    return std::string();
  }

  return GetSanitizedPageStateForPickle_ChromiumImpl(navigation);
}

void ContentSerializedNavigationDriver::Sanitize(
    SerializedNavigationEntry* navigation) const {
  Sanitize_ChromiumImpl(navigation);

  // Restore previously saved urls with the flyweb:// (or brave://) scheme as
  // chrome://. The virtual URL must never use the display-only scheme.
  const auto& virtual_url = navigation->virtual_url();
  if (IsDisplayOnlyUIScheme(virtual_url)) {
    GURL::Replacements replacements;
    replacements.SetSchemeStr(content::kChromeUIScheme);
    navigation->set_virtual_url(virtual_url.ReplaceComponents(replacements));
    // FlyWeb: the stored PageState of such an entry may still point at the
    // display-only scheme and would be restored verbatim. Drop it, as we do on
    // save for every chrome:// URL; ToNavigationEntry() synthesizes a fresh
    // one from the rewritten chrome:// URL.
    navigation->set_encoded_page_state(std::string());
  }
}

}  // namespace sessions

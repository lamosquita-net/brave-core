/* Copyright (c) 2023 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "chrome/browser/ui/omnibox/omnibox_controller.h"

#include "brave/components/omnibox/browser/brave_omnibox_prefs.h"
#include "components/prefs/pref_service.h"

namespace {

// Used by patches/chrome-browser-ui-omnibox-omnibox_controller.cc.patch, which
// also turns StartZeroSuggestPrefetch() into a no-op (no zero-suggest
// prefetch requests to the search engine).
bool IsAutocompleteEnabled(const PrefService* prefs) {
  return prefs->GetBoolean(omnibox::kAutocompleteEnabled);
}

}  // namespace

#include "src/chrome/browser/ui/omnibox/omnibox_controller.cc"

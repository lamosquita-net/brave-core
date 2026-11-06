/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/browser/ui/omnibox/brave_omnibox_client_impl.h"

#include "brave/browser/autocomplete/brave_autocomplete_scheme_classifier.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/browser/ui/omnibox/chrome_omnibox_client.h"

BraveOmniboxClientImpl::BraveOmniboxClientImpl(LocationBar* location_bar,
                                               BrowserWindowInterface* browser,
                                               Profile* profile)
    : ChromeOmniboxClient(location_bar, browser, profile),
      scheme_classifier_(profile) {}

BraveOmniboxClientImpl::~BraveOmniboxClientImpl() = default;

const AutocompleteSchemeClassifier&
BraveOmniboxClientImpl::GetSchemeClassifier() const {
  return scheme_classifier_;
}

/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/browser/ui/omnibox/brave_omnibox_client_impl.h"

#include "base/values.h"
#include "brave/browser/autocomplete/brave_autocomplete_scheme_classifier.h"
#include "brave/components/constants/pref_names.h"
#include "brave/components/omnibox/browser/brave_omnibox_prefs.h"
#include "brave/components/p3a_utils/bucket.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/browser/ui/omnibox/chrome_omnibox_client.h"
#include "components/omnibox/browser/autocomplete_match.h"
#include "components/prefs/pref_registry_simple.h"
#include "components/prefs/pref_service.h"

namespace {

constexpr char kSearchCountPrefName[] = "brave.weekly_storage.search_count";

bool IsSearchEvent(const AutocompleteMatch& match) {
  switch (match.type) {
    case omnibox::AutocompleteMatchType::kSearchWhatYouTyped:
    case omnibox::AutocompleteMatchType::kSearchHistory:
    case omnibox::AutocompleteMatchType::kSearchSuggest:
    case omnibox::AutocompleteMatchType::kSearchSuggestEntity:
    case omnibox::AutocompleteMatchType::kSearchSuggestTail:
    case omnibox::AutocompleteMatchType::kSearchSuggestPersonalized:
    case omnibox::AutocompleteMatchType::kSearchSuggestProfile:
    case omnibox::AutocompleteMatchType::kSearchOtherEngine:
      return true;
    default:
      return false;
  }
}

void RecordSearchEventP3A(uint64_t number_of_searches) {
  p3a_utils::RecordToHistogramBucket("Brave.Omnibox.SearchCount.3",
                                     {0, 5, 10, 20, 50, 100, 500},
                                     number_of_searches);
}

}  // namespace

BraveOmniboxClientImpl::BraveOmniboxClientImpl(LocationBar* location_bar,
                                               BrowserWindowInterface* browser,
                                               Profile* profile)
    : ChromeOmniboxClient(location_bar, browser, profile),
      profile_(profile),
      scheme_classifier_(profile),
      search_storage_(profile_->GetPrefs(), kSearchCountPrefName) {
  // Record initial search count p3a value.
  RecordSearchEventP3A(search_storage_.GetWeeklySum());
}

BraveOmniboxClientImpl::~BraveOmniboxClientImpl() = default;

void BraveOmniboxClientImpl::RegisterProfilePrefs(
    PrefRegistrySimple* registry) {
  registry->RegisterListPref(kSearchCountPrefName);
}

const AutocompleteSchemeClassifier&
BraveOmniboxClientImpl::GetSchemeClassifier() const {
  return scheme_classifier_;
}

void BraveOmniboxClientImpl::OnAutocompleteAccept(
    const GURL& destination_url,
    TemplateURLRef::PostContent* post_content,
    WindowOpenDisposition disposition,
    ui::PageTransition transition,
    omnibox::AutocompleteMatchType match_type,
    base::TimeTicks match_selection_timestamp,
    bool destination_url_entered_without_scheme,
    bool destination_url_entered_with_http_scheme,
    const std::u16string& text,
    const AutocompleteMatch& match,
    const AutocompleteMatch& alternative_nav_match) {
  if (IsSearchEvent(match)) {
    // TODO(iefremov): Optimize this.
    search_storage_.AddDelta(1);
    RecordSearchEventP3A(search_storage_.GetWeeklySum());
  }
  ChromeOmniboxClient::OnAutocompleteAccept(
      destination_url, post_content, disposition, transition, match_type,
      match_selection_timestamp, destination_url_entered_without_scheme,
      destination_url_entered_with_http_scheme, text, match,
      alternative_nav_match);
}

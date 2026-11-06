/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_BROWSER_UI_OMNIBOX_BRAVE_OMNIBOX_CLIENT_IMPL_H_
#define BRAVE_BROWSER_UI_OMNIBOX_BRAVE_OMNIBOX_CLIENT_IMPL_H_

#include "base/memory/raw_ptr.h"
#include "brave/browser/autocomplete/brave_autocomplete_scheme_classifier.h"
#include "brave/components/time_period_storage/weekly_storage.h"
#include "chrome/browser/ui/omnibox/chrome_omnibox_client.h"

class BrowserWindowInterface;
class PrefRegistrySimple;
class Profile;

class BraveOmniboxClientImpl : public ChromeOmniboxClient {
 public:
  BraveOmniboxClientImpl(LocationBar* location_bar,
                         BrowserWindowInterface* browser,
                         Profile* profile);
  BraveOmniboxClientImpl(const BraveOmniboxClientImpl&) = delete;
  BraveOmniboxClientImpl& operator=(const BraveOmniboxClientImpl&) = delete;
  ~BraveOmniboxClientImpl() override;

  static void RegisterProfilePrefs(PrefRegistrySimple* prefs);

  const AutocompleteSchemeClassifier& GetSchemeClassifier() const override;

  void OnAutocompleteAccept(
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
      const AutocompleteMatch& alternative_nav_match) override;

 private:
  raw_ptr<Profile> profile_ = nullptr;
  BraveAutocompleteSchemeClassifier scheme_classifier_;
  WeeklyStorage search_storage_;
};

#endif  // BRAVE_BROWSER_UI_OMNIBOX_BRAVE_OMNIBOX_CLIENT_IMPL_H_

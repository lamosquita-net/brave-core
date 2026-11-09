// Copyright (c) 2022 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// you can obtain one at http://mozilla.org/MPL/2.0/.

#include "brave/browser/ui/webui/private_new_tab_page/brave_private_new_tab_page_handler.h"

#include <utility>

#include "base/strings/utf_string_conversions.h"
#include "brave/components/brave_private_new_tab_ui/common/pref_names.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/browser/search_engines/template_url_service_factory.h"
#include "chrome/browser/ui/browser.h"
#include "chrome/browser/ui/browser_window/public/browser_window_interface.h"
#include "components/prefs/pref_service.h"
#include "components/search_engines/template_url.h"
#include "components/search_engines/template_url_service.h"
#include "content/public/browser/page_navigator.h"
#include "content/public/browser/web_contents.h"

BravePrivateNewTabPageHandler::BravePrivateNewTabPageHandler(
    Profile* profile,
    content::WebContents* web_contents,
    mojo::PendingReceiver<brave_private_new_tab::mojom::PageHandler> receiver)
    : profile_(profile),
      web_contents_(web_contents),
      receiver_(this, std::move(receiver)) {}

BravePrivateNewTabPageHandler::~BravePrivateNewTabPageHandler() = default;

void BravePrivateNewTabPageHandler::SetClientPage(
    mojo::PendingRemote<brave_private_new_tab::mojom::PrivateTabPage> page) {
  page_.Bind(std::move(page));
}

void BravePrivateNewTabPageHandler::SetDisclaimerDismissed(bool dismissed) {
  DCHECK(profile_);

  profile_->GetOriginalProfile()->GetPrefs()->SetBoolean(
      brave_private_new_tab::prefs::kBravePrivateWindowDisclaimerDismissed,
      dismissed);
}

void BravePrivateNewTabPageHandler::GetDisclaimerDismissed(
    GetDisclaimerDismissedCallback callback) {
  DCHECK(profile_);

  bool dismissed = profile_->GetOriginalProfile()->GetPrefs()->GetBoolean(
      brave_private_new_tab::prefs::kBravePrivateWindowDisclaimerDismissed);
  std::move(callback).Run(dismissed);
}

void BravePrivateNewTabPageHandler::GoToSearch(const std::string& input,
                                               bool open_new_tab) {
  DCHECK(profile_);

  // FlyWeb: search with the user's default search engine, not a hardcoded Brave Search.
  // For an off-the-record profile the factory returns the original profile's service.
  TemplateURLService* template_url_service =
      TemplateURLServiceFactory::GetForProfile(profile_);
  if (!template_url_service) {
    return;
  }
  const TemplateURL* t_url = template_url_service->GetDefaultSearchProvider();
  if (!t_url || !t_url->url_ref().SupportsReplacement(
                    template_url_service->search_terms_data())) {
    return;
  }

  auto url = GURL(t_url->url_ref().ReplaceSearchTerms(
      TemplateURLRef::SearchTermsArgs(base::UTF8ToUTF16(input)),
      template_url_service->search_terms_data()));
  if (!url.is_valid()) {
    return;
  }

  auto window_open_disposition = WindowOpenDisposition::CURRENT_TAB;

  if (open_new_tab) {
    window_open_disposition = WindowOpenDisposition::NEW_BACKGROUND_TAB;
  }

  web_contents_->OpenURL(
      content::OpenURLParams(url, content::Referrer(), window_open_disposition,
                             ui::PageTransition::PAGE_TRANSITION_FORM_SUBMIT,
                             false),
      /*navigation_handle_callback=*/{});
}

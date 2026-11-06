// Copyright (c) 2024 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

#include "chrome/browser/ui/webui/cr_components/searchbox/searchbox_omnibox_client.h"

#include "brave/components/search_engines/brave_prepopulated_engines.h"
#include "components/omnibox/browser/autocomplete_match.h"
#include "components/search_engines/template_url_service.h"
#include "content/public/browser/page_navigator.h"
#include "net/base/url_util.h"

namespace {

// When `match` is a keyword search against Brave Search, replace
// `source=desktop` with `source=newtab` (same behaviour as FlyWeb 128's
// realbox_handler.cc override). Unlike Brave 1.98 this sends no first-run or
// stats data: only the value of the existing `source` parameter changes.
// The accompanying patch wraps the `OpenURL()` params in
// `OnAutocompleteAccept()` with this helper.
content::OpenURLParams MaybeOverrideURLParams(
    content::OpenURLParams params,
    const AutocompleteMatch& match,
    TemplateURLService* template_url_service) {
  if (match.keyword.empty() || !template_url_service) {
    return params;
  }

  const TemplateURL* template_url =
      template_url_service->GetTemplateURLForKeyword(match.keyword);
  if (template_url &&
      template_url->prepopulate_id() ==
          TemplateURLPrepopulateData::PREPOPULATED_ENGINE_ID_BRAVE) {
    params.url =
        net::AppendOrReplaceQueryParameter(params.url, "source", "newtab");
  }
  return params;
}

}  // namespace

#include "src/chrome/browser/ui/webui/cr_components/searchbox/searchbox_omnibox_client.cc"

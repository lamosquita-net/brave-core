/* Copyright (c) 2022 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "components/search_engines/template_url_starter_pack_data.h"

#include <string_view>

#include "base/strings/strcat.h"
#include "base/strings/string_util.h"

#define GetStarterPackEngines GetStarterPackEngines_ChromiumImpl
#define GetDataVersion GetDataVersion_ChromiumImpl

#include "src/components/search_engines/template_url_starter_pack_data.cc"

#undef GetDataVersion
#undef GetStarterPackEngines

namespace {

constexpr char kChromeSchema[] = "chrome://";
constexpr char kBraveSchema[] = "flyweb://";

}  // namespace

namespace template_url_starter_pack_data {

// Bump whenever the FlyWeb list below changes, so that existing profiles merge
// it again (engines no longer listed are removed unless the user edited them).
// 1: @page and @aimode (156) left out.
const int kCurrentFlyWebDataVersion = 1;

// As 1.98, only the starter packs that search local data, plus @gemini (kept
// by the T7 decision). Left out: @page (contextual search: sends the page to
// Google Lens / AI Mode) and @aimode (Google AI Mode). New upstream starter
// packs stay out until they are added here.
const StarterPackEngine* flyweb_engines[] = {&bookmarks, &history, &tabs,
                                             &gemini};

int GetDataVersion() {
  return GetDataVersion_ChromiumImpl() + kCurrentFlyWebDataVersion;
}

std::vector<std::unique_ptr<TemplateURLData>> GetStarterPackEngines() {
  std::vector<std::unique_ptr<TemplateURLData>> t_urls;
  for (const auto* engine : flyweb_engines) {
    auto t_url = TemplateURLDataFromStarterPackEngine(*engine);

    // It is necessary to correct urls for the flyweb schema.
    std::string_view url(t_url->url());
    if (base::StartsWith(url, kChromeSchema,
                         base::CompareCase::INSENSITIVE_ASCII)) {
      t_url->SetURL(base::StrCat(
          {kBraveSchema, url.substr(std::size(kChromeSchema) - 1)}));
    }

    t_urls.push_back(std::move(t_url));
  }
  return t_urls;
}

}  // namespace template_url_starter_pack_data

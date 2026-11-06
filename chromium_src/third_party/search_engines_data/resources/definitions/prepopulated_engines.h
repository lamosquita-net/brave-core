// Copyright (c) 2025 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

#ifndef BRAVE_CHROMIUM_SRC_THIRD_PARTY_SEARCH_ENGINES_DATA_RESOURCES_DEFINITIONS_PREPOPULATED_ENGINES_H_
#define BRAVE_CHROMIUM_SRC_THIRD_PARTY_SEARCH_ENGINES_DATA_RESOURCES_DEFINITIONS_PREPOPULATED_ENGINES_H_

#include "../gen/third_party/search_engines_data/resources/definitions/prepopulated_engines.h"  // IWYU pragma: export

// DuckDuckGo and Qwant are patched out of prepopulated_engines.json (see
// patches/third_party/search_engines_data/resources/) and defined in
// //brave/components/search_engines/brave_prepopulated_engines.cc with FlyWeb's
// URLs. Chromium's generated regional_settings.cc still lists them by name.
namespace TemplateURLPrepopulateData {
extern const PrepopulatedEngine duckduckgo;
extern const PrepopulatedEngine qwant;
}  // namespace TemplateURLPrepopulateData

#endif  // BRAVE_CHROMIUM_SRC_THIRD_PARTY_SEARCH_ENGINES_DATA_RESOURCES_DEFINITIONS_PREPOPULATED_ENGINES_H_

/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "chrome/browser/profiles/profile.h"

#include "components/search_engines/search_engine_choice/search_engine_choice_utils.h"

#define WipeSearchEngineChoicePrefs                                 \
  IsChoiceScreenFlagEnabled(search_engines::ChoicePromo::kAny);     \
  if (IsChoiceScreenFlagEnabled(search_engines::ChoicePromo::kAny)) \
  search_engines::WipeSearchEngineChoicePrefs
#include "src/chrome/browser/profiles/profile.cc"
#undef WipeSearchEngineChoicePrefs

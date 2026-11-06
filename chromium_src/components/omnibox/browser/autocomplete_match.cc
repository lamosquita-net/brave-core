// Copyright (c) 2023 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

#include "components/omnibox/browser/autocomplete_match.h"

// Handle Brave-specific OPEN_HERE action ID: upstream's
// GetOmniboxEventResultType() hits NOTREACHED() for UNKNOWN, and the switch
// has no default case.
#define STARTER_PACK_AI_MODE \
  OPEN_HERE:                 \
  break;                     \
  case OmniboxActionId::STARTER_PACK_AI_MODE

#include "src/components/omnibox/browser/autocomplete_match.cc"

#undef STARTER_PACK_AI_MODE

/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/components/translate/core/common/brave_translate_constants.h"

#define GetTranslateSecurityOrigin GetTranslateSecurityOrigin_Chromium
#include "src/components/translate/core/common/translate_util.cc"
#undef GetTranslateSecurityOrigin

namespace translate {

// FlyWeb: Google's own origin (Chromium's), never translate.brave.com.
GURL GetTranslateSecurityOrigin() {
  return GetTranslateSecurityOrigin_Chromium();
}

}  // namespace translate

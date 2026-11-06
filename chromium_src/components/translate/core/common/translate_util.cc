/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "components/translate/core/common/translate_util.h"

#define IsTFLiteLanguageDetectionEnabled \
  IsTFLiteLanguageDetectionEnabled_Chromium
#include "src/components/translate/core/common/translate_util.cc"
#undef IsTFLiteLanguageDetectionEnabled

namespace translate {

// The TFLite language detection model is downloaded through Google's
// optimization guide; keep using the bundled CLD3 detector instead.
bool IsTFLiteLanguageDetectionEnabled() {
  return false;
}

}  // namespace translate

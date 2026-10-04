/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "components/translate/core/browser/translate_script.h"

#include "base/functional/callback.h"
#include "base/strings/strcat.h"
#include "base/strings/stringprintf.h"
#include "base/task/sequenced_task_runner.h"
#include "brave/components/constants/brave_services_key.h"
#include "brave/components/translate/core/common/brave_translate_constants.h"
#include "brave/components/translate/core/common/brave_translate_features.h"
#include "components/grit/brave_components_resources.h"
#include "ui/base/resource/resource_bundle.h"

namespace translate {
namespace google_apis {
// FlyWeb: no key. Brave sent its services key to translate.brave.com; FlyWeb
// talks to Google directly and must not hand it FlyWeb's components key.
std::string GetAPIKey() {
  return std::string();
}
}  // namespace google_apis
}  // namespace translate

#define TranslateScript ChromiumTranslateScript
#include "src/components/translate/core/browser/translate_script.cc"
#undef TranslateScript

namespace translate {

// FlyWeb: the script comes from Google itself (no redirect to the Brave
// endpoints; UseGoogleTranslateEndpoint() is always true).
GURL ChromiumTranslateScript::AddHostLocaleToUrl(const GURL& url) {
  GURL result = ::translate::AddHostLocaleToUrl(url);
  if (translate::UseGoogleTranslateEndpoint()) {
    return result;
  }
  const GURL google_translate_script(kScriptURL);
  if (result.host_piece() == google_translate_script.host_piece()) {
    const GURL brave_translate_script(kBraveTranslateScriptURL);
    GURL::Replacements replaces;
    replaces.SetHostStr(brave_translate_script.host_piece());
    replaces.SetPathStr(brave_translate_script.path_piece());
    return result.ReplaceComponents(replaces);
  }
  return result;
}

void TranslateScript::Request(RequestCallback callback, bool is_incognito) {
  if (!IsBraveTranslateGoAvailable()) {
    base::SequencedTaskRunner::GetCurrentDefault()->PostTask(
        FROM_HERE, base::BindOnce(std::move(callback), false));
    return;
  }
  ChromiumTranslateScript::Request(std::move(callback), is_incognito);
}

void TranslateScript::OnScriptFetchComplete(bool success,
                                            const std::string& data) {
  const std::string new_data = base::StrCat(
      {base::StringPrintf(
           "const useGoogleTranslateEndpoint = %s;",
           translate::UseGoogleTranslateEndpoint() ? "true" : "false"),
       base::StringPrintf("const braveTranslateStaticPath = '%s';",
                          kBraveTranslateStaticPath),
       ui::ResourceBundle::GetSharedInstance().LoadDataResourceString(
           IDR_BRAVE_TRANSLATE_JS),
       data});
  ChromiumTranslateScript::OnScriptFetchComplete(success, new_data);
}

}  // namespace translate

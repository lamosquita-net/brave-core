/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/browser/ui/webui/brave_web_ui_controller_factory.h"

#include <memory>
#include <string>

#include "base/feature_list.h"
#include "base/memory/ptr_util.h"
#include "base/no_destructor.h"
#include "brave/browser/ui/webui/brave_adblock_internals_ui.h"
#include "brave/browser/ui/webui/brave_adblock_ui.h"
#include "brave/components/constants/pref_names.h"
#include "brave/components/constants/webui_url_constants.h"
#include "build/build_config.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/common/url_constants.h"
#include "components/optimization_guide/core/optimization_guide_features.h"
#include "components/optimization_guide/optimization_guide_internals/webui/url_constants.h"
#include "components/prefs/pref_service.h"
#include "content/public/browser/web_contents.h"
#include "content/public/common/url_utils.h"
#include "url/gurl.h"

#if !BUILDFLAG(IS_ANDROID)
#include "brave/browser/ui/webui/brave_settings_ui.h"
#include "brave/browser/ui/webui/new_tab_page/brave_new_tab_ui.h"
#include "brave/browser/ui/webui/private_new_tab_page/brave_private_new_tab_ui.h"
#include "brave/browser/ui/webui/welcome_page/brave_welcome_ui.h"
#include "brave/components/commands/common/features.h"
#endif

using content::WebUI;
using content::WebUIController;

namespace {

// A function for creating a new WebUI. The caller owns the return value, which
// may be NULL (for example, if the URL refers to an non-existent extension).
typedef WebUIController* (*WebUIFactoryFunction)(WebUI* web_ui,
                                                 const GURL& url);

WebUIController* NewWebUI(WebUI* web_ui, const GURL& url) {
  auto host = url.host();
  Profile* profile = Profile::FromBrowserContext(
      web_ui->GetWebContents()->GetBrowserContext());
  if (host == kAdblockHost) {
    return new BraveAdblockUI(web_ui, url.GetHost());
  } else if (host == kAdblockInternalsHost) {
    return new BraveAdblockInternalsUI(web_ui, url.GetHost());
#if !BUILDFLAG(IS_ANDROID)
  } else if (host == kWelcomeHost && !profile->IsGuestSession()) {
    return new BraveWelcomeUI(web_ui, url.GetHost());
  } else if (host == chrome::kChromeUISettingsHost) {
    // FlyWeb: normally created through BraveSettingsUIConfig (see
    // chrome_web_ui_configs.cc); kept so both paths build the same UI.
    return new BraveSettingsUI(web_ui);
  } else if (host == chrome::kChromeUINewTabHost) {
    if (profile->IsIncognitoProfile() || profile->IsGuestSession()) {
      return new BravePrivateNewTabUI(web_ui, url.GetHost());
    }
    return new BraveNewTabUI(web_ui, url.GetHost());
#endif  // !BUILDFLAG(IS_ANDROID)
  }

  return nullptr;
}

// Returns a function that can be used to create the right type of WebUI for a
// tab, based on its URL. Returns NULL if the URL doesn't have WebUI associated
// with it.
WebUIFactoryFunction GetWebUIFactoryFunction(WebUI* web_ui,
                                             Profile* profile,
                                             const GURL& url) {
  // This will get called a lot to check all URLs, so do a quick check of other
  // schemes to filter out most URLs.
  //
  // This has a narrow scoper scope than content::HasWebUIScheme(url) which also
  // allows both `chrome-untrusted` and `chrome-devtools`.
  if (!url.SchemeIs(content::kBraveUIScheme) &&
      !url.SchemeIs(content::kChromeUIScheme)) {
    return nullptr;
  }

  if (url.host() == kAdblockHost || url.host() == kAdblockInternalsHost ||
#if !BUILDFLAG(IS_ANDROID)
      // On Android New Tab is a native page implemented in Java, so no need
      // in WebUI.
      url.host() == chrome::kChromeUINewTabHost ||
      url.host() == chrome::kChromeUISettingsHost ||
      ((url.host() == kWelcomeHost ||
        url.host() == kWelcomeURL) &&
       !profile->IsGuestSession()) ||
#endif  // BUILDFLAG(IS_ANDROID)
      // FlyWeb: Brave Rewards WebUI removed (was the last term).
      false) {
    return &NewWebUI;
  }

  return nullptr;
}

}  // namespace

WebUI::TypeID BraveWebUIControllerFactory::GetWebUIType(
    content::BrowserContext* browser_context,
    const GURL& url) {
  // Early return to prevent upstream create its WebUI.
  if (url.host() == optimization_guide_internals::
                        kChromeUIOptimizationGuideInternalsHost &&
      !optimization_guide::features::IsOptimizationHintsEnabled()) {
    return WebUI::kNoWebUI;
  }

  Profile* profile = Profile::FromBrowserContext(browser_context);
  WebUIFactoryFunction function =
      GetWebUIFactoryFunction(nullptr, profile, url);
  if (function) {
    return reinterpret_cast<WebUI::TypeID>(function);
  }
  return ChromeWebUIControllerFactory::GetWebUIType(browser_context, url);
}

std::unique_ptr<WebUIController>
BraveWebUIControllerFactory::CreateWebUIControllerForURL(WebUI* web_ui,
                                                         const GURL& url) {
  Profile* profile = Profile::FromWebUI(web_ui);
  WebUIFactoryFunction function = GetWebUIFactoryFunction(web_ui, profile, url);
  if (!function) {
    return ChromeWebUIControllerFactory::CreateWebUIControllerForURL(web_ui,
                                                                     url);
  }

  return base::WrapUnique((*function)(web_ui, url));
}

// static
BraveWebUIControllerFactory* BraveWebUIControllerFactory::GetInstance() {
  static base::NoDestructor<BraveWebUIControllerFactory> instance;
  return instance.get();
}

BraveWebUIControllerFactory::BraveWebUIControllerFactory() = default;

BraveWebUIControllerFactory::~BraveWebUIControllerFactory() = default;

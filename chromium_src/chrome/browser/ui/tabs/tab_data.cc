/* Copyright (c) 2023 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "chrome/browser/ui/tabs/tab_data.h"

#include <optional>

#include "base/check.h"
#include "base/feature_list.h"
#include "brave/browser/ui/tabs/features.h"
#include "brave/browser/ui/tabs/shared_pinned_tab_service.h"
#include "brave/browser/ui/tabs/shared_pinned_tab_service_factory.h"
#include "brave/components/constants/webui_url_constants.h"
#include "chrome/browser/ui/browser_window/public/browser_window_interface.h"
#include "chrome/browser/ui/tabs/tab_strip_model.h"
#include "chrome/common/webui_url_constants.h"
#include "content/public/browser/web_contents.h"
#include "content/public/common/url_constants.h"
#include "url/gurl.h"

namespace {

// 156 replaced TabRendererData::FromTabInModel() with
// tabs::TabData::FromTabInterface() (as in Brave 1.98).
// Returns stored TabData if |tab| is a shared pinned dummy, nullopt otherwise.
std::optional<tabs::TabData> MaybeGetSharedPinnedTabData(
    tabs::TabInterface* tab) {
  if (!base::FeatureList::IsEnabled(tabs::features::kBraveSharedPinnedTabs)) {
    return std::nullopt;
  }
  // Note that in unit tests, bwi may be null.
  auto* const bwi = tab->GetBrowserWindowInterface();
  if (!bwi) {
    return std::nullopt;
  }
  TabStripModel* model = bwi->GetTabStripModel();
  const int index = model->GetIndexOfTab(tab);
  if (index >= model->IndexOfFirstNonPinnedTab()) {
    return std::nullopt;
  }
  auto* shared_pinned_tab_service =
      SharedPinnedTabServiceFactory::GetForProfile(model->profile());
  DCHECK(shared_pinned_tab_service);
  auto* contents = model->GetWebContentsAt(index);
  if (shared_pinned_tab_service->IsDummyContents(contents)) {
    if (const auto* data =
            shared_pinned_tab_service->GetTabDataForDummyContents(index,
                                                                  contents)) {
      return *data;
    }
  }
  return std::nullopt;
}

// Don't themify the favicon of the welcome WebUI, nor the new tab page's.
// FlyWeb: both show the colored fly (IDR_NTP_FAVICON is replaced with it in
// app/theme/*/common/favicon_ntp.png). Chromium themifies the NTP because its
// own icon is a monochrome glyph; with the fly, ThemeFavicon() would turn it
// into a flat silhouette on light tabs (contrast 2.2-2.9, threshold 3.0).
tabs::TabData MaybeApplyBraveTabDataOverrides(tabs::TabInterface* tab,
                                              tabs::TabData data) {
  if (data.should_themify_favicon) {
    const GURL& url = tab->GetContents()->GetVisibleURL();
    if (url.SchemeIs(content::kChromeUIScheme) &&
        (url.host() == kWelcomeHost ||
         url.host() == chrome::kChromeUINewTabHost ||
         url.host() == chrome::kChromeUINewTabPageHost)) {
      data.should_themify_favicon = false;
    }
  }
  return data;
}

}  // namespace

#include "src/chrome/browser/ui/tabs/tab_data.cc"

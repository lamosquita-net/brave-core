/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "extensions/common/manifest_handlers/manifest_url_handlers.h"

#define GetHomepageURL GetHomepageURL_Unused
#define GetWebStoreURL GetWebStoreURL_Unused

#include "src/extensions/common/manifest_handlers/manifest_url_handlers.cc"

#undef GetHomepageURL
#undef GetWebStoreURL

namespace extensions {

// We need to provide our own version of GetHomepageURL() as well to make sure
// that we get our own version of GetWebStoreURL() called when invoked via
// GetHomepageURL(). Otherwise the old version will still be called since the
// renaming of GetWebStoreURL() above does also modify the call point.
// static
GURL ManifestURL::GetHomepageURL(const Extension* extension) {
  const GURL& homepage_url = Get(extension, keys::kHomepageURL);
  return homepage_url.is_valid() ? homepage_url : GetWebStoreURL(extension);
}

// No link to the Chrome Web Store detail page for any extension (Brave 1.98
// only blanks it for Brave-hosted extensions; FlyWeb keeps the stricter one).
// static
GURL ManifestURL::GetWebStoreURL(const Extension* extension) {
  return GURL();
}

}  // namespace extensions

/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * you can obtain one at http://mozilla.org/MPL/2.0/. */

// Full replacement of the upstream file (no include of the original): the
// Ranker model is never downloaded from Google. In 156 RankerURLFetcher moved
// from components/assist_ranker to components/translate/core/browser.

#include "components/translate/core/browser/ranker_url_fetcher.h"

#include "services/network/public/cpp/simple_url_loader.h"

namespace translate {

RankerURLFetcher::RankerURLFetcher() = default;

RankerURLFetcher::~RankerURLFetcher() = default;

bool RankerURLFetcher::Request(
    const GURL& url,
    RankerURLFetcher::Callback callback,
    network::mojom::URLLoaderFactory* url_loader_factory) {
  // false = no request started and `callback` never runs; the header's
  // contract allows it and RankerModelLoaderImpl::StartLoadFromURL handles it.
  return false;
}

}  // namespace translate

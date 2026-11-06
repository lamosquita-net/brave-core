/* Copyright (c) 2023 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include <utility>
#include <vector>

#include "base/functional/bind.h"
#include "base/functional/callback.h"
#include "base/location.h"
#include "base/memory/weak_ptr.h"
#include "base/task/sequenced_task_runner.h"
#include "components/affiliations/core/browser/hash_affiliation_fetcher.h"
#include "net/base/net_errors.h"

namespace {

// Never contacts Google's affiliation service: every request completes
// asynchronously with an empty, successful result.
class BraveHashAffiliationFetcher
    : public affiliations::HashAffiliationFetcher {
 public:
  using HashAffiliationFetcher::HashAffiliationFetcher;

  void StartRequest(
      const std::vector<affiliations::FacetURI>& facet_uris,
      RequestInfo request_info,
      base::OnceCallback<void(FetchResult)> result_callback) override {
    base::SequencedTaskRunner::GetCurrentDefault()->PostTask(
        FROM_HERE,
        base::BindOnce(&BraveHashAffiliationFetcher::CompleteStubbedRequest,
                       weak_ptr_factory_.GetWeakPtr(),
                       std::move(result_callback)));
  }

 private:
  void CompleteStubbedRequest(
      base::OnceCallback<void(FetchResult)> result_callback) {
    FetchResult result;
    result.network_status = net::OK;
    std::move(result_callback).Run(std::move(result));
  }

  base::WeakPtrFactory<BraveHashAffiliationFetcher> weak_ptr_factory_{this};
};

}  // namespace

#define HashAffiliationFetcher BraveHashAffiliationFetcher
#include "src/components/affiliations/core/browser/affiliation_fetcher_factory_impl.cc"
#undef HashAffiliationFetcher

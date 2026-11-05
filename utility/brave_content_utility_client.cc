/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/utility/brave_content_utility_client.h"

#include <memory>
#include <utility>

#include "build/build_config.h"
#include "mojo/public/cpp/bindings/service_factory.h"

#if !BUILDFLAG(IS_ANDROID)
#include "brave/utility/importer/brave_profile_import_impl.h"
#endif

namespace {

#if !BUILDFLAG(IS_ANDROID)
auto RunBraveProfileImporter(
    mojo::PendingReceiver<brave::mojom::ProfileImport> receiver) {
  return std::make_unique<BraveProfileImportImpl>(std::move(receiver));
}
#endif

}  // namespace

BraveContentUtilityClient::BraveContentUtilityClient() = default;
BraveContentUtilityClient::~BraveContentUtilityClient() = default;

void BraveContentUtilityClient::RegisterMainThreadServices(
    mojo::ServiceFactory& services) {
#if !BUILDFLAG(IS_ANDROID)
  services.Add(RunBraveProfileImporter);
#endif

  return ChromeContentUtilityClient::RegisterMainThreadServices(services);
}

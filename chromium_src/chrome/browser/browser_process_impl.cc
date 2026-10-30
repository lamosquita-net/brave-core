/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "extensions/buildflags/buildflags.h"

#if BUILDFLAG(ENABLE_EXTENSIONS)
#include "brave/browser/extensions/brave_extensions_browser_client_impl.h"
#define ChromeExtensionsBrowserClient BraveExtensionsBrowserClientImpl
#endif
#include "build/build_config.h"

#if BUILDFLAG(IS_MAC)
#include "brave/components/flyweb_os_crypt/fw1_key_provider_mac.h"
// FlyWeb `fw1` (FS.25): AES-256-GCM key in the classic keychain.
#define BRAVE_BROWSER_PROCESS_IMPL_ADD_OS_CRYPT_ASYNC_PROVIDERS \
  providers.emplace_back(std::make_pair(                        \
      os_crypt_async::Fw1KeyProvider::kPrecedence,              \
      std::make_unique<os_crypt_async::Fw1KeyProvider>()));
#else
#define BRAVE_BROWSER_PROCESS_IMPL_ADD_OS_CRYPT_ASYNC_PROVIDERS
#endif

#include "src/chrome/browser/browser_process_impl.cc"
#undef BRAVE_BROWSER_PROCESS_IMPL_ADD_OS_CRYPT_ASYNC_PROVIDERS
#if BUILDFLAG(ENABLE_EXTENSIONS)
#undef ChromeExtensionsBrowserClient
#endif

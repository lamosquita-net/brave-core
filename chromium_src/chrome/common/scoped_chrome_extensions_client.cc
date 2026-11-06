// Copyright (c) 2021 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// you can obtain one at http://mozilla.org/MPL/2.0/.

// In 156 chrome/common/initialize_extensions_client.cc is gone; the client is
// now created in ScopedChromeExtensionsClient's constructor. The header is
// included before the define so the `client_` member type stays untouched.
#include "chrome/common/scoped_chrome_extensions_client.h"

#include "brave/common/extensions/brave_extensions_client.h"

#define ChromeExtensionsClient BraveExtensionsClient
#include "src/chrome/common/scoped_chrome_extensions_client.cc"
#undef ChromeExtensionsClient

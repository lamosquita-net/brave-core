/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "chrome/browser/devtools/url_constants.h"

// FlyWeb: no remote DevTools frontend (Brave proxies Google's at
// devtools.brave.com). The bundled frontend is used for local pages; only
// debugging other devices (flyweb://inspect) would fetch a remote frontend,
// and that now fails instead of contacting Brave.
const char kRemoteFrontendDomain[] = "flyweb.invalid";
const char kRemoteFrontendBase[] = "https://flyweb.invalid/";
const char kRemoteFrontendPath[] = "serve_file";

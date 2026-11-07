/* Copyright (c) 2023 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "third_party/blink/renderer/core/dom/document.h"

#include "third_party/blink/renderer/platform/weborigin/kurl.h"

namespace {

// Convert brave to chrome as early as possible for renderer side url checks
blink::KURL RewriteBraveToChrome(const blink::KURL& brave_url) {
  if (!brave_url.ProtocolIs("brave")) {
    return brave_url;
  }
  blink::KURL chrome_url(brave_url);
  chrome_url.SetProtocol("chrome");
  return chrome_url;
}

}  // namespace

#include "src/third_party/blink/renderer/core/dom/document.cc"

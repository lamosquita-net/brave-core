/* Copyright (c) 2025 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_CHROMIUM_SRC_CONTENT_BROWSER_RENDERER_HOST_NAVIGATION_ENTRY_IMPL_H_
#define BRAVE_CHROMIUM_SRC_CONTENT_BROWSER_RENDERER_HOST_NAVIGATION_ENTRY_IMPL_H_

#include <string>

#include "content/public/browser/navigation_entry.h"

// FlyWeb: 1.98 hooks GetSSL() and also declares the Containers
// storage-partition-key accessors there; Containers is not part of FlyWeb, so
// the two upstream methods are wrapped directly instead.
#define SetVirtualURL(...)                 \
  SetVirtualURL_ChromiumImpl(__VA_ARGS__); \
  void SetVirtualURL(__VA_ARGS__)

#define GetTitleForDisplay(...)                       \
  GetTitleForDisplay_ChromiumImpl(__VA_ARGS__) const; \
  const std::u16string& GetTitleForDisplay(__VA_ARGS__)

#include "src/content/browser/renderer_host/navigation_entry_impl.h"  // IWYU pragma: export

#undef GetTitleForDisplay
#undef SetVirtualURL

#endif  // BRAVE_CHROMIUM_SRC_CONTENT_BROWSER_RENDERER_HOST_NAVIGATION_ENTRY_IMPL_H_

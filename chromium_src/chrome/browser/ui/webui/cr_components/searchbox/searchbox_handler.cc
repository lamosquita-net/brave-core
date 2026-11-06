// Copyright (c) 2024 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

#include "brave/components/vector_icons/vector_icons.h"

namespace {
constexpr char kLeoWindowTabNewIconResourceName[] =
    "//resources/brave-icons/window-tab-new.svg";
}  // namespace

// The Leo icon mapping in |AutocompleteIconToResourceName| and the
// keyword-only autocomplete input are applied by
// patches/chrome-browser-ui-webui-cr_components-searchbox-searchbox_handler.cc.patch.
#include "src/chrome/browser/ui/webui/cr_components/searchbox/searchbox_handler.cc"

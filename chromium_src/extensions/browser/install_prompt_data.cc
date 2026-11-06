/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "extensions/browser/install_prompt_data.h"

#include "brave/grit/brave_generated_resources.h"
#include "extensions/strings/grit/extensions_strings.h"

// Extensions that FlyWeb did not vet get their own install prompt title. In
// 156 the title comes from InstallPromptData::GetDialogTitle(), which uses
// IDS_EXTENSION_INSTALL_PROMPT_TITLE only for INSTALL_PROMPT.
#undef IDS_EXTENSION_INSTALL_PROMPT_TITLE
#define IDS_EXTENSION_INSTALL_PROMPT_TITLE \
  IDS_UNVETTED_EXTENSION_INSTALL_PROMPT_TITLE

#include "src/extensions/browser/install_prompt_data.cc"

#undef IDS_EXTENSION_INSTALL_PROMPT_TITLE

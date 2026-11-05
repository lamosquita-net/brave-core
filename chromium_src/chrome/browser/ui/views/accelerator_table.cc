/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "chrome/browser/ui/views/accelerator_table.h"

#include "brave/app/brave_command_ids.h"
#include "build/build_config.h"

#define GetAcceleratorList GetAcceleratorList_ChromiumImpl
#include "src/chrome/browser/ui/views/accelerator_table.cc"
#undef GetAcceleratorList

namespace {

const AcceleratorMapping kBraveAcceleratorMap[] = {
    {ui::VKEY_M, ui::EF_CONTROL_DOWN, IDC_TOGGLE_TAB_MUTE},
};

}  // namespace

std::vector<AcceleratorMapping> GetAcceleratorList() {
  std::vector<AcceleratorMapping> accelerator_list(
      GetAcceleratorList_ChromiumImpl());

  accelerator_list.insert(
      accelerator_list.end(), kBraveAcceleratorMap,
      kBraveAcceleratorMap + std::size(kBraveAcceleratorMap));

  return accelerator_list;
}

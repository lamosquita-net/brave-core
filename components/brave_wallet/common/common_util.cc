/* Copyright (c) 2022 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "brave/components/brave_wallet/common/common_util.h"

#include "brave/components/brave_wallet/common/pref_names.h"
#include "build/build_config.h"
#include "components/prefs/pref_service.h"

namespace brave_wallet {

bool IsAllowed(PrefService* prefs) {
  // FlyWeb: Brave Wallet is always off (no wallet UI, no window.ethereum /
  // window.solana providers), with or without the BraveWalletDisabled policy.
  return false;
}

}  // namespace brave_wallet

/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/components/brave_sync/features.h"

#include "base/feature_list.h"

namespace brave_sync {
namespace features {

// FlyWeb: off. Its endpoint is inert (build.sh), so the Settings page and the
// menu entry would offer a service that cannot work. Disabled, Brave adds
// --disable-sync at startup (BraveBrowserMainParts::PreProfileInit).
BASE_FEATURE(kBraveSync, "BraveSync", base::FEATURE_DISABLED_BY_DEFAULT);

// When this feature is enabled through flyweb://flags it adds to history entry's
// title additional info for sync diagnostics:
// - whether history entry should be synced;
// - typed count;
// - page transition.
BASE_FEATURE(kBraveSyncHistoryDiagnostics,
             "BraveSyncHistoryDiagnostics",
             base::FEATURE_DISABLED_BY_DEFAULT);

// When this feature is enabled, Sync sends to remote server all the history
// entries, including page transition beyond typed url (link, bookmark, reload,
// etc).
BASE_FEATURE(kBraveSyncSendAllHistory,
             "BraveSyncSendAllHistory",
             base::FEATURE_DISABLED_BY_DEFAULT);

}  // namespace features
}  // namespace brave_sync

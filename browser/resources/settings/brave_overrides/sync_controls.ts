// Copyright (c) 2020 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// you can obtain one at https://mozilla.org/MPL/2.0/.

// FlyWeb: settings-sync-controls (data types of FlyWeb's sync, shown by
// settings-brave-sync-configure) is a Lit element in Chromium 156. Our sync
// does not offer payments, so hide its toggle with injected CSS (Brave 1.98
// removes it with lit_mangler).

import {injectStyle} from '//resources/brave/lit_overriding.js'
import {css} from '//resources/lit/v3_0/lit.rollup.js'

import {SettingsSyncControlsElement} from '../people_page/sync_controls.js'

injectStyle(SettingsSyncControlsElement, css`
  .list-item:has(#paymentsCheckbox) {
    display: none !important;
  }
`)

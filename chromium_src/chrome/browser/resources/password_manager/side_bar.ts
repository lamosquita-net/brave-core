// Copyright (c) 2023 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

// Replaces password_manager/side_bar.ts (upstream kept as side_bar-chromium.ts
// by //brave/chromium_src/tools/grit/preprocess_if_expr.py), as in Brave 1.98.

import {RegisterPolymerTemplateModifications, RegisterStyleOverride} from '//resources/brave/polymer_overriding.js'
import {html} from '//resources/polymer/v3_0/polymer/polymer_bundled.min.js'

RegisterStyleOverride(
  'password-manager-side-bar',
  html`
    <style>
      .cr-nav-menu-item {
        min-height: 20px !important;
        border-end-end-radius: 0px !important;
        border-start-end-radius: 0px !important;
        box-sizing: content-box !important;
      }

      .cr-nav-menu-item:hover {
        background: transparent !important;
      }

      .cr-nav-menu-item[selected] {
        --iron-icon-fill-color: var(--cr-link-color) !important;
        color: var(--cr-link-color) !important;
        background: transparent !important;
      }

      .cr-nav-menu-item cr-icon {
        display: none !important;
      }

      .cr-nav-menu-item cr-ripple {
        display: none !important;
      }
    </style>
  `
)

RegisterPolymerTemplateModifications({
  'password-manager-side-bar': (templateContent) => {
    // Checkup (compromised/reused/weak passwords) is not offered: its
    // compromised-credentials check is Google's bulk leak check service.
    const checkupMenuItem = templateContent.querySelector('#checkup')
    if (!checkupMenuItem) {
      console.error(
        `[Brave Password Manager Overrides] Could not find menu item 'checkup'`)
    } else {
      checkupMenuItem.remove()
    }
  }
})

export * from './side_bar-chromium.js'

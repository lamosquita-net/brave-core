// Copyright (c) 2026 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

// Replaces settings/people_page/people_page.ts (upstream kept as
// people_page-chromium.ts by //brave/chromium_src/tools/grit/
// preprocess_if_expr.py), as in Brave 1.98. FlyWeb shows this card in the
// "Get started" section for the local profile only: no Google sign-in or
// sync. Brave 1.98 removes the Google rows with lit_mangler; FlyWeb, which
// does not ship lit_mangler, hides them with CSS.

import {injectStyle} from '//resources/brave/lit_overriding.js'
import {css} from '//resources/lit/v3_0/lit.rollup.js'

import {
  SettingsPeoplePageElement as SettingsPeoplePageElementChromium
} from './people_page-chromium.js'

injectStyle(SettingsPeoplePageElementChromium, css`
  settings-sync-account-control,
  #profile-row,
  #account-subpage-row,
  #sync-setup,
  #google-services,
  #manage-google-account {
    display: none !important;
  }

  /* First visible row once the ones above are hidden. */
  #edit-profile {
    border-top: none;
  }
`)

class SettingsPeoplePageElement extends SettingsPeoplePageElementChromium {
  // We set signin.allowed to false (BraveProfileManager), which would
  // otherwise hide both #manage-google-account and #edit-profile ("Profile
  // name and icon"). #manage-google-account is hidden above; #edit-profile
  // needs to stay, so force this true.
  //
  // signinAllowed_ is `protected` on the upstream class; setting it in the
  // constructor instead of re-declaring it avoids giving it a new declaring
  // class (see Brave 1.98's copy of this file).
  constructor() {
    super()
    this.signinAllowed_ = true
  }
}

export {SettingsPeoplePageElement}
export * from './people_page-chromium.js'

// Register our subclass instead of the upstream class. The matching
// `customElements.define` in upstream people_page.ts is patched out.
customElements.define(
  SettingsPeoplePageElement.is, SettingsPeoplePageElement)

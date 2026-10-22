/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

document.addEventListener('DOMContentLoaded', function () {
  const flagsApp = document.querySelector('flags-app')
  if (flagsApp) {
    // FlyWeb has no beta or nightly channels: hide Chromium's channel promos.
    flagsApp.shadowRoot.querySelectorAll(
      '#channel-promo-beta, #channel-promo-dev').forEach((node) => {
       node.hidden = true
     })
    flagsApp.shadowRoot.querySelector('div.blurb-container').after(
      (() => {
        const p = document.createElement('p')
        p.textContent = '* Overridden by active variations or a command line.'
        return p
      })()
    )
  }
});

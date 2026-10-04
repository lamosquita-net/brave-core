/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

document.addEventListener('DOMContentLoaded', function () {
  // FlyWeb has no beta or nightly channels: hide Chromium's channel promos.
  [$('channel-promo-beta'), $('channel-promo-dev')].forEach((node) => {
    if (node) {
      node.hidden = true
    }
  })
});

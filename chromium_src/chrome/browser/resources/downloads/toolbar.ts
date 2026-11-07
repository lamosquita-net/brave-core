// Copyright (c) 2024 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

// Elements used by the upstream template, imported explicitly so that
// @webui-eslint/web-component-missing-deps sees them (same imports as the
// upstream file, now kept as -chromium.ts).
import 'chrome://resources/cr_elements/cr_button/cr_button.js';
import 'chrome://resources/cr_elements/cr_toolbar/cr_toolbar.js';
import 'chrome://resources/cr_elements/icons.html.js';
import 'chrome://resources/cr_elements/cr_shared_vars.css.js';
import 'chrome://resources/js/util.js';
import '/strings.m.js';
import 'chrome://resources/cr_elements/cr_toast/cr_toast_manager.js';
import 'chrome://resources/js/assert.js';
import 'chrome://resources/js/load_time_data.js';
import 'chrome://resources/lit/v3_0/lit.rollup.js';
import './downloads.mojom-webui.js';
import './search_service.js';

import { injectStyle } from '//resources/brave/lit_overriding.js'
import { css } from '//resources/lit/v3_0/lit.rollup.js'
import { DownloadsToolbarElement } from './toolbar-chromium.js'

// Our downloads page doesn't have a static area to display the "Clear All"
// button, so it ends up overlapping the scrollbar when the list of download
// items is long. Add some margin to the button to avoid that.
injectStyle(DownloadsToolbarElement, css`
  #clearAll {
    margin-right: 20px;
  }
`)

export * from './toolbar-chromium.js'

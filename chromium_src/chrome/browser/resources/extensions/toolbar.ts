// Copyright (c) 2024 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

// Elements used by the upstream template, imported explicitly so that
// @webui-eslint/web-component-missing-deps sees them (same imports as the
// upstream file, now kept as -chromium.ts).
import 'chrome://resources/cr_elements/cr_button/cr_button.js';
import 'chrome://resources/cr_elements/cr_toggle/cr_toggle.js';
import 'chrome://resources/cr_elements/cr_toolbar/cr_toolbar.js';
import 'chrome://resources/cr_elements/policy/cr_tooltip_icon.js';
import './pack_dialog.js';
import 'chrome://resources/cr_elements/cr_toast/cr_toast_manager.js';
import 'chrome://resources/cr_elements/i18n_mixin_lit.js';
import 'chrome://resources/js/util.js';
import 'chrome://resources/lit/v3_0/lit.rollup.js';
import './item_util.js';

import { injectStyle } from '//resources/brave/lit_overriding.js'
import { css } from '//resources/lit/v3_0/lit.rollup.js'
import { ExtensionsToolbarElement } from './toolbar-chromium.js'

injectStyle(ExtensionsToolbarElement, css`
  :host #devDrawer {
    overflow: hidden
  }`)

export * from './toolbar-chromium.js'

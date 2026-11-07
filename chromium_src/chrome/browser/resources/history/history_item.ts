// Copyright (c) 2024 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

// Elements used by the upstream template, imported explicitly so that
// @webui-eslint/web-component-missing-deps sees them (same imports as the
// upstream file, now kept as -chromium.ts).
// <if expr="_google_chrome">
import './internal/icons.html.js';
// </if>
import './searched_label.js';
import './shared_icons.html.js';
import '/strings.m.js';
import 'chrome://resources/cr_elements/cr_checkbox/cr_checkbox.js';
import 'chrome://resources/cr_elements/cr_collapse/cr_collapse.js';
import 'chrome://resources/cr_elements/cr_icon/cr_icon.js';
import 'chrome://resources/cr_elements/cr_icon_button/cr_icon_button.js';
import 'chrome://resources/cr_elements/icons.html.js';
import 'chrome://resources/cr_elements/policy/cr_tooltip_icon.js';
import 'chrome://resources/cr_components/history/constants.js';
import 'chrome://resources/cr_components/history/history.mojom-webui.js';
import 'chrome://resources/cr_elements/focus_row_mixin_lit.js';
import 'chrome://resources/js/event_tracker.js';
import 'chrome://resources/js/focus_without_ink.js';
import 'chrome://resources/js/icon.js';
import 'chrome://resources/js/load_time_data.js';
import 'chrome://resources/lit/v3_0/lit.rollup.js';
import './browser_proxy.js';

import {injectStyle} from '//resources/brave/lit_overriding.js'
import {css} from '//resources/lit/v3_0/lit.rollup.js'

import {HistoryItemElement} from './history_item-chromium.js'

injectStyle(HistoryItemElement, css`
  .website-title {
    font-size: 13px;
    font-weight: 400;
  }

  #menu-button {
    transform: rotate(90deg) !important;
  }
`)

export * from './history_item-chromium.js'

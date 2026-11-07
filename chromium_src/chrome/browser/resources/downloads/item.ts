// Copyright (c) 2023 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

// Elements used by the upstream template, imported explicitly so that
// @webui-eslint/web-component-missing-deps sees them (same imports as the
// upstream file, now kept as -chromium.ts).
import './icons.html.js';
// <if expr="_google_chrome">
import './internal/icons.html.js';
// </if>
import 'chrome://resources/cr_elements/cr_action_menu/cr_action_menu.js';
import 'chrome://resources/cr_elements/cr_button/cr_button.js';
import 'chrome://resources/cr_elements/cr_icon/cr_icon.js';
import 'chrome://resources/cr_elements/cr_icon_button/cr_icon_button.js';
// <if expr="_google_chrome">
import 'chrome://resources/cr_elements/cr_link_row/cr_link_row.js';
// </if>
import 'chrome://resources/cr_elements/cr_progress/cr_progress.js';
import 'chrome://resources/cr_elements/icons.html.js';
import 'chrome://resources/js/action_link.js';
import '/strings.m.js';
import 'chrome://resources/cr_elements/cr_a11y_announcer/cr_a11y_announcer.js';
import 'chrome://resources/cr_elements/cr_toast/cr_toast_manager.js';
import 'chrome://resources/cr_elements/focus_row_mixin_lit.js';
import 'chrome://resources/cr_elements/i18n_mixin_lit.js';
import 'chrome://resources/js/assert.js';
import 'chrome://resources/js/load_time_data.js';
import 'chrome://resources/js/parse_html_subset.js';
import 'chrome://resources/js/util.js';
import 'chrome://resources/lit/v3_0/lit.rollup.js';
import './downloads.mojom-webui.js';
import './icon_loader.js';

import { injectStyle } from '//resources/brave/lit_overriding.js'
import { css } from '//resources/lit/v3_0/lit.rollup.js'
import { DownloadsItemElement } from './item-chromium.js'

injectStyle(DownloadsItemElement, css`
    #progress {
        --cr-progress-active-color: var(--leo-color-icon-interactive) !important;
    }
    @media (prefers-color-scheme: light) {
        #content:not(.is-active) {
            /* Color for "filled cards" */
            background-color: var(--leo-color-container-highlight) !important;
        }
    }
`)

export * from './item-chromium.js'

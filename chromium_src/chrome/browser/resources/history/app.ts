// Copyright (c) 2024 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

// Elements used by the upstream template, imported explicitly so that
// @webui-eslint/web-component-missing-deps sees them (same imports as the
// upstream file, now kept as -chromium.ts).
import 'chrome://resources/cr_components/history_clusters/clusters.js';
import 'chrome://resources/cr_components/history_embeddings/filter_chips.js';
import 'chrome://resources/cr_components/history_embeddings/history_embeddings.js';
import 'chrome://resources/cr_elements/cr_drawer/cr_drawer.js';
import 'chrome://resources/cr_elements/cr_lazy_render/cr_lazy_render_lit.js';
import 'chrome://resources/cr_elements/cr_page_selector/cr_page_selector.js';
import 'chrome://resources/cr_elements/cr_tabs/cr_tabs.js';
import './history_embeddings_promo.js';
// <if expr="not is_chromeos">
import './history_cross_device_signin_promo.js';
// </if>
// <if expr="not is_chromeos">
import './history_sync_promo.js';
// </if>
import './history_list.js';
import './history_toolbar.js';
import './history_filter_chips.js';
import './query_manager.js';
import './router.js';
import './side_bar.js';
import './synced_device_manager.js';
import '/strings.m.js';
import 'chrome://resources/cr_components/color_change_listener/colors_css_updater.js';
import 'chrome://resources/cr_components/help_bubble/help_bubble_mixin_lit.js';
import 'chrome://resources/cr_components/history/constants.js';
import 'chrome://resources/cr_components/history/foreign_sessions.mojom-webui.js';
import 'chrome://resources/cr_elements/cr_a11y_announcer/cr_a11y_announcer.js';
import 'chrome://resources/cr_elements/find_shortcut_mixin_lit.js';
import 'chrome://resources/cr_elements/web_ui_listener_mixin_lit.js';
import 'chrome://resources/js/assert.js';
import 'chrome://resources/js/event_tracker.js';
import 'chrome://resources/js/load_time_data.js';
import 'chrome://resources/js/util.js';
import 'chrome://resources/lit/v3_0/lit.rollup.js';
import 'chrome://resources/mojo/components/user_education/webui/user_education.mojom-webui.js';
import './browser_proxy.js';
import './constants.js';

import {injectStyle} from '//resources/brave/lit_overriding.js'
import {css} from '//resources/lit/v3_0/lit.rollup.js'

import {HistoryAppElement} from './app-chromium.js'

injectStyle(HistoryAppElement, css`
        #tabs-content, #tabs-content>* {
            height: 100%;
        }
`)

export * from './app-chromium.js'

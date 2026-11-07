// Copyright (c) 2023 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

// Elements used by the upstream template, imported explicitly so that
// @webui-eslint/web-component-missing-deps sees them (same imports as the
// upstream file, now kept as -chromium.ts).
import 'chrome://resources/cr_elements/cr_icon/cr_icon.js';
import 'chrome://resources/cr_elements/cr_menu_selector/cr_menu_selector.js';
import 'chrome://resources/cr_elements/cr_ripple/cr_ripple.js';
import './icons.html.js';
import 'chrome://resources/cr_elements/i18n_mixin_lit.js';
import 'chrome://resources/js/assert.js';
import 'chrome://resources/js/load_time_data.js';
import 'chrome://resources/lit/v3_0/lit.rollup.js';
import './navigation_helper.js';

import { injectStyle } from '//resources/brave/lit_overriding.js'
import { css } from '//resources/lit/v3_0/lit.rollup.js'
import { ExtensionsSidebarElement } from './sidebar-chromium.js'

injectStyle(ExtensionsSidebarElement, css`
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

  .separator {
    display: none !important;
  }

  #moreExtensions cr-icon {
    display: none !important;
  }

  /* Hide the dev-mode "AI coding agents" docs promo. */
  .cr-nav-menu-item:has(#promo-message-text) {
    display: none !important;
  }
`)

export * from './sidebar-chromium.js'

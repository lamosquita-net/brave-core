/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

// FlyWeb: in Chromium 156 the sync section is a cr-view-manager plugin
// (brave_sync_page_index.ts); this is its parent view, the link row into the
// setup subpage (Brave 1.98's settings-brave-sync-manage-devices-page). The
// sync status handling lives in the index.

import 'chrome://resources/cr_elements/cr_link_row/cr_link_row.js';
import '../settings_page/settings_section.js';
import '../settings_shared.css.js';
import '../settings_vars.css.js';

import {PolymerElement} from 'chrome://resources/polymer/v3_0/polymer/polymer_bundled.min.js';

import {BaseMixin} from '../base_mixin.js'
import {routes} from '../route.js';
import {Router} from '../router.js';
import {SettingsViewMixin} from '../settings_page/settings_view_mixin.js';

import {getTemplate} from './brave_sync_page.html.js'

const SettingsBraveSyncPageElementBase =
  SettingsViewMixin(BaseMixin(PolymerElement))

export class SettingsBraveSyncPageElement extends SettingsBraveSyncPageElementBase {
  static get is() {
    return 'settings-brave-sync-page'
  }

  static get template() {
    return getTemplate()
  }

  static get properties() {
    return {
      label: String,
    };
  }

  declare label: string;

  onSyncTap_() {
    // Users can go to sync subpage regardless of sync status.
    Router.getInstance().navigateTo(routes.BRAVE_SYNC_SETUP);
  }

  override getAssociatedControlFor(childViewId: string): HTMLElement {
    switch (childViewId) {
      case 'setup':
        return this.shadowRoot!.querySelector<HTMLElement>(
          '#brave-sync-setup')!;
      default:
        throw new Error(`Unknown child view id: ${childViewId}`)
    }
  }
}

customElements.define(
  SettingsBraveSyncPageElement.is, SettingsBraveSyncPageElement)

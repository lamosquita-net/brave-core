/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

// 'settings-brave-sync-page-index' is the sync plugin of settings-main
// (Chromium 156 cr-view-manager model, as in Brave 1.98): FlyWeb's own sync
// (sync.flyweb.lamosquita.net), set up with a sync code.

import 'chrome://resources/cr_elements/cr_view_manager/cr_view_manager.js';
import '/shared/settings/prefs/prefs.js';
import '../settings_page/settings_subpage.js';
import '../settings_shared.css.js';
import '../settings_vars.css.js';
import './brave_sync_page.js';
import './brave_sync_subpage.js';

import {SyncBrowserProxy, SyncBrowserProxyImpl, SyncPrefs} from '/shared/settings/people_page/sync_browser_proxy.js';
import type {CrViewManagerElement} from 'chrome://resources/cr_elements/cr_view_manager/cr_view_manager.js';
import {I18nMixin} from 'chrome://resources/cr_elements/i18n_mixin.js'
import {WebUiListenerMixin} from 'chrome://resources/cr_elements/web_ui_listener_mixin.js';
import {PolymerElement} from 'chrome://resources/polymer/v3_0/polymer/polymer_bundled.min.js';

import {routes} from '../route.js';
import {RouteObserverMixin} from '../router.js';
import type {Route} from '../router.js';
import type {SettingsPlugin} from '../settings_main/settings_plugin.js';
import {SearchableViewContainerMixin} from '../settings_page/searchable_view_container_mixin.js';

import {BraveSyncBrowserProxy, BraveSyncStatus} from './brave_sync_browser_proxy.js';
import {getTemplate} from './brave_sync_page_index.html.js'

export interface SettingsBraveSyncPageIndexElement {
  $: {
    viewManager: CrViewManagerElement,
  };
}

const SettingsBraveSyncPageIndexElementBase = SearchableViewContainerMixin(
  RouteObserverMixin(I18nMixin(WebUiListenerMixin(PolymerElement))))

export class SettingsBraveSyncPageIndexElement extends
  SettingsBraveSyncPageIndexElementBase implements SettingsPlugin {
  static get is() {
    return 'settings-brave-sync-page-index'
  }

  static get template() {
    return getTemplate()
  }

  static get properties() {
    return {
      prefs: Object,
      /**
       * The current sync status, supplied by BraveSyncBrowserProxy.
       */
      syncStatus_: Object,
      isEncryptionSet_: {
        type: Boolean,
        value: false
      },
      syncLabel_: {
        type: String,
        computed: 'computeSyncLabel_(syncStatus_)'
      },
    };
  }

  declare prefs: Record<string, unknown>;
  declare private syncStatus_: BraveSyncStatus;
  declare private isEncryptionSet_: boolean;
  declare private syncLabel_: string;

  private browserProxy_: SyncBrowserProxy = SyncBrowserProxyImpl.getInstance();
  private braveBrowserProxy_: BraveSyncBrowserProxy =
    BraveSyncBrowserProxy.getInstance();

  private computeSyncLabel_() {
    if (this.syncStatus_ !== undefined &&
        this.syncStatus_.hasSyncWordsDecryptionError) {
        return this.i18n('braveSyncCouldNotSyncActionLabel');
    }
    const isAlreadySetup = this.syncStatus_ !== undefined &&
        !this.syncStatus_.firstSetupInProgress;
    const key = isAlreadySetup ? 'braveSyncManageActionLabel' : 'braveSyncSetupActionLabel';
    return this.i18n(key);
  }

  override connectedCallback() {
    super.connectedCallback()
    const onSyncStatus = this.handleSyncStatus_.bind(this)
    this.braveBrowserProxy_.getSyncStatus().then(
        (status: BraveSyncStatus) => onSyncStatus(status));
    this.addWebUiListener(
      'sync-prefs-changed', this.handleSyncPrefsChanged_.bind(this));
    this.addWebUiListener('sync-status-changed', onSyncStatus);
  }

  /**
   * Handler for when the sync state is pushed from the browser.
   */
  private handleSyncStatus_(syncStatus: BraveSyncStatus) {
    this.syncStatus_ = syncStatus;
  }

  /**
   * Handler for when the sync preferences are updated.
   */
  private async handleSyncPrefsChanged_(syncPrefs: SyncPrefs) {
    if (this.syncStatus_ && !this.syncStatus_.firstSetupInProgress) {
      const pureSyncCode = await this.braveBrowserProxy_.getPureSyncCode()
      if (syncPrefs.passphraseRequired) {
        await this.browserProxy_.setDecryptionPassphrase(pureSyncCode);
      } else if (!this.isEncryptionSet_) {
        this.browserProxy_.setEncryptionPassphrase(pureSyncCode)
        .then(successfullySet => {
          this.isEncryptionSet_ = successfullySet
        })
      }
    }
  }

  override currentRouteChanged(newRoute: Route, oldRoute?: Route) {
    super.currentRouteChanged(newRoute, oldRoute);

    // Need to wait for currentRouteChanged observers on child views to run
    // first, before switching views.
    queueMicrotask(() => {
      switch (newRoute) {
        case routes.BRAVE_SYNC:
        case routes.BASIC:
          this.$.viewManager.switchView(
              'braveSync', 'no-animation', 'no-animation');
          break;
        case routes.BRAVE_SYNC_SETUP:
          this.$.viewManager.switchView(
              'setup', 'no-animation', 'no-animation');
          break;
        default:
          break;
      }
    });
  }
}

declare global {
  interface HTMLElementTagNameMap {
    'settings-brave-sync-page-index': SettingsBraveSyncPageIndexElement;
  }
}

customElements.define(
  SettingsBraveSyncPageIndexElement.is, SettingsBraveSyncPageIndexElement)

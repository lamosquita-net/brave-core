// Copyright (c) 2025 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

// 'settings-brave-system-page-index' replaces the System plugin of
// settings-main (Chromium 156 cr-view-manager model, as in Brave 1.98): the
// upstream system page, Brave's system toggles, the memory and battery cards
// (FlyWeb has no separate Performance section) and the keyboard shortcuts
// subpage.

import 'chrome://resources/cr_elements/cr_view_manager/cr_view_manager.js'
import '/shared/settings/prefs/prefs.js'
import '../performance_page/battery_page.js'
import '../performance_page/memory_page.js'
import '../settings_page/settings_subpage.js'
import '../settings_shared.css.js'
import '../shortcuts_page/shortcuts_page.js'
import '../system_page/system_page.js'
import './brave_system_extras.js'

import type {CrViewManagerElement} from 'chrome://resources/cr_elements/cr_view_manager/cr_view_manager.js'
import {WebUiListenerMixin} from 'chrome://resources/cr_elements/web_ui_listener_mixin.js'
import {PolymerElement} from 'chrome://resources/polymer/v3_0/polymer/polymer_bundled.min.js'

import {loadTimeData} from '../i18n_setup.js'
import {PerformanceBrowserProxyImpl} from '../performance_page/performance_browser_proxy.js'
import {routes} from '../route.js'
import {RouteObserverMixin} from '../router.js'
import type {Route} from '../router.js'
import type {SettingsPlugin} from '../settings_main/settings_plugin.js'
import {SearchableViewContainerMixin} from '../settings_page/searchable_view_container_mixin.js'

import {getTemplate} from './brave_system_page_index.html.js'

export interface SettingsBraveSystemPageIndexElement {
  $: {
    viewManager: CrViewManagerElement,
  }
}

const SettingsBraveSystemPageIndexElementBase = SearchableViewContainerMixin(
  RouteObserverMixin(WebUiListenerMixin(PolymerElement)))

export class SettingsBraveSystemPageIndexElement extends
  SettingsBraveSystemPageIndexElementBase implements SettingsPlugin {
  static get is() {
    return 'settings-brave-system-page-index'
  }

  static get template() {
    return getTemplate()
  }

  static get properties() {
    return {
      prefs: Object,
      // Used to hide the battery card if the device has no battery.
      showBatterySettings_: {
        type: Boolean,
        value: false,
      },
      showShortcutsPage_: {
        type: Boolean,
        value: () => loadTimeData.getBoolean('areShortcutsSupported'),
      },
    }
  }

  declare prefs: Record<string, unknown>
  declare private showBatterySettings_: boolean
  declare private showShortcutsPage_: boolean

  override connectedCallback() {
    super.connectedCallback()

    this.addWebUiListener(
      'device-has-battery-changed', this.onDeviceHasBatteryChanged_.bind(this))
    PerformanceBrowserProxyImpl.getInstance().getDeviceHasBattery().then(
      this.onDeviceHasBatteryChanged_.bind(this))
  }

  private onDeviceHasBatteryChanged_(hasBattery: boolean) {
    this.showBatterySettings_ = hasBattery
  }

  private showDefaultViews_() {
    this.$.viewManager.switchViews(
      ['system', 'braveSystem', 'memory', 'battery'], 'no-animation',
      'no-animation')
  }

  override currentRouteChanged(newRoute: Route, oldRoute?: Route) {
    super.currentRouteChanged(newRoute, oldRoute)

    // Need to wait for currentRouteChanged observers on child views to run
    // first, before switching views.
    queueMicrotask(() => {
      switch (newRoute) {
        case routes.SYSTEM:
        case routes.BASIC:
          this.showDefaultViews_()
          break
        case routes.SHORTCUTS:
          // The subpage is stamped by a dom-if; wait for it to render.
          setTimeout(() => {
            this.$.viewManager.switchView(
              'shortcuts', 'no-animation', 'no-animation')
          })
          break
        default:
          break
      }
    })
  }
}

declare global {
  interface HTMLElementTagNameMap {
    'settings-brave-system-page-index': SettingsBraveSystemPageIndexElement
  }
}

customElements.define(
  SettingsBraveSystemPageIndexElement.is, SettingsBraveSystemPageIndexElement)

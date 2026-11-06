// Copyright (c) 2025 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

// 'settings-getting-started-page-index' is the "Get started" plugin of
// settings-main (Chromium 156 cr-view-manager model, as in Brave 1.98). The
// upstream profile, default browser and on-startup cards, which FlyWeb hides
// as top-level sections (page_visibility.ts), are shown here.

import 'chrome://resources/cr_elements/cr_view_manager/cr_view_manager.js'
import '/shared/settings/prefs/prefs.js'
import '../brave_new_tab_page/brave_new_tab_page.js'
import '../default_browser_page/default_browser_page.js'
import '../on_startup_page/on_startup_page.js'
import '../people_page/manage_profile.js'
import '../people_page/people_page.js'
import '../settings_page/settings_section.js'
import '../settings_shared.css.js'
// <if expr="enable_pin_shortcut">
import '../pin_shortcut_page/pin_shortcut_page.js'
// </if>

import type {CrViewManagerElement} from 'chrome://resources/cr_elements/cr_view_manager/cr_view_manager.js'
import {PolymerElement} from 'chrome://resources/polymer/v3_0/polymer/polymer_bundled.min.js'

import {routes} from '../route.js'
import {RouteObserverMixin} from '../router.js'
import type {Route} from '../router.js'
import type {SettingsPlugin} from '../settings_main/settings_plugin.js'
import {SearchableViewContainerMixin} from '../settings_page/searchable_view_container_mixin.js'

import {getTemplate} from './getting_started_page_index.html.js'

export interface SettingsGettingStartedPageIndexElement {
  $: {
    viewManager: CrViewManagerElement,
  }
}

const SettingsGettingStartedPageIndexElementBase =
  SearchableViewContainerMixin(RouteObserverMixin(PolymerElement))

export class SettingsGettingStartedPageIndexElement extends
  SettingsGettingStartedPageIndexElementBase implements SettingsPlugin {
  static get is() {
    return 'settings-getting-started-page-index'
  }

  static get template() {
    return getTemplate()
  }

  static get properties() {
    return {
      prefs: Object,
    }
  }

  declare prefs: Record<string, unknown>

  private showDefaultViews_() {
    const views = ['people', 'defaultBrowser']
    // <if expr="enable_pin_shortcut">
    views.push('pinShortcut')
    // </if>
    views.push('onStartup', 'newTab')
    this.$.viewManager.switchViews(views, 'no-animation', 'no-animation')
  }

  override currentRouteChanged(newRoute: Route, oldRoute?: Route) {
    super.currentRouteChanged(newRoute, oldRoute)

    // Need to wait for currentRouteChanged observers on child views to run
    // first, before switching views.
    queueMicrotask(() => {
      switch (newRoute) {
        case routes.GET_STARTED:
        case routes.IMPORT_DATA:
        case routes.BRAVE_NEW_TAB:
        case routes.BASIC:
          // BASIC: switch back to the default views in case they are part of
          // search results.
          this.showDefaultViews_()
          break
        case routes.MANAGE_PROFILE:
          this.$.viewManager.switchView(
            'manageProfile', 'no-animation', 'no-animation')
          break
        default:
          // Nothing to do. Other parent elements are responsible for updating
          // the displayed contents.
          break
      }
    })
  }
}

declare global {
  interface HTMLElementTagNameMap {
    'settings-getting-started-page-index':
      SettingsGettingStartedPageIndexElement
  }
}

customElements.define(
  SettingsGettingStartedPageIndexElement.is,
  SettingsGettingStartedPageIndexElement)

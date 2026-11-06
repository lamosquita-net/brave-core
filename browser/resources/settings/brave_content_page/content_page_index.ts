// Copyright (c) 2025 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

// 'settings-brave-content-page-index' is the "Content" plugin of
// settings-main (Chromium 156 cr-view-manager model, as in Brave 1.98).

import 'chrome://resources/cr_elements/cr_view_manager/cr_view_manager.js'
import '/shared/settings/prefs/prefs.js'
import '../appearance_page/appearance_fonts_page.js'
import '../settings_shared.css.js'
import './content.js'

import type {CrViewManagerElement} from 'chrome://resources/cr_elements/cr_view_manager/cr_view_manager.js'
import {PolymerElement} from 'chrome://resources/polymer/v3_0/polymer/polymer_bundled.min.js'

import {routes} from '../route.js'
import {RouteObserverMixin} from '../router.js'
import type {Route} from '../router.js'
import type {SettingsPlugin} from '../settings_main/settings_plugin.js'
import {SearchableViewContainerMixin} from '../settings_page/searchable_view_container_mixin.js'

import {getTemplate} from './content_page_index.html.js'

export interface SettingsBraveContentPageIndexElement {
  $: {
    viewManager: CrViewManagerElement,
  }
}

const SettingsBraveContentPageIndexElementBase =
  SearchableViewContainerMixin(RouteObserverMixin(PolymerElement))

export class SettingsBraveContentPageIndexElement extends
  SettingsBraveContentPageIndexElementBase implements SettingsPlugin {
  static get is() {
    return 'settings-brave-content-page-index'
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

  override currentRouteChanged(newRoute: Route, oldRoute?: Route) {
    super.currentRouteChanged(newRoute, oldRoute)

    // Need to wait for currentRouteChanged observers on child views to run
    // first, before switching views.
    queueMicrotask(() => {
      switch (newRoute) {
        case routes.BRAVE_CONTENT:
        case routes.BASIC:
          this.$.viewManager.switchView(
            'parent', 'no-animation', 'no-animation')
          break
        case routes.FONTS:
          this.$.viewManager.switchView(
            'fonts', 'no-animation', 'no-animation')
          break
        default:
          break
      }
    })
  }
}

declare global {
  interface HTMLElementTagNameMap {
    'settings-brave-content-page-index': SettingsBraveContentPageIndexElement
  }
}

customElements.define(
  SettingsBraveContentPageIndexElement.is,
  SettingsBraveContentPageIndexElement)

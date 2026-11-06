// Copyright (c) 2026 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

// FlyWeb: Brave's search engine settings (private window search engine,
// suggestions, other search engines). In Chromium 156 settings-search-page
// is a Lit element (Brave 1.98 inserts settings-brave-search-page into it
// with lit_mangler and bridges prefs by hand), so it lives in this second
// plugin, placed by brave_overrides/settings_main.ts in settings-main's
// search view next to the upstream one, with two-way pref bindings.

import 'chrome://resources/cr_elements/cr_view_manager/cr_view_manager.js'
import '/shared/settings/prefs/prefs.js'
import '../settings_page/settings_section.js'
import '../settings_shared.css.js'
import './brave_search_engines_page.js'

import type {CrViewManagerElement} from 'chrome://resources/cr_elements/cr_view_manager/cr_view_manager.js'
import {PolymerElement} from 'chrome://resources/polymer/v3_0/polymer/polymer_bundled.min.js'

import {routes} from '../route.js'
import {RouteObserverMixin} from '../router.js'
import type {Route} from '../router.js'
import type {SettingsPlugin} from '../settings_main/settings_plugin.js'
import {SearchableViewContainerMixin} from '../settings_page/searchable_view_container_mixin.js'

import {getTemplate} from './brave_search_extras.html.js'

export interface SettingsBraveSearchExtrasElement {
  $: {
    viewManager: CrViewManagerElement,
  }
}

const SettingsBraveSearchExtrasElementBase =
  SearchableViewContainerMixin(RouteObserverMixin(PolymerElement))

export class SettingsBraveSearchExtrasElement extends
  SettingsBraveSearchExtrasElementBase implements SettingsPlugin {
  static get is() {
    return 'settings-brave-search-extras'
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

    // Only on the search engine page itself, not on its subpages.
    const show = newRoute === routes.SEARCH || newRoute === routes.BASIC
    this.hidden = !show
    if (show) {
      queueMicrotask(() => {
        this.$.viewManager.switchView(
          'braveSearch', 'no-animation', 'no-animation')
      })
    }
  }
}

declare global {
  interface HTMLElementTagNameMap {
    'settings-brave-search-extras': SettingsBraveSearchExtrasElement
  }
}

customElements.define(
  SettingsBraveSearchExtrasElement.is, SettingsBraveSearchExtrasElement)

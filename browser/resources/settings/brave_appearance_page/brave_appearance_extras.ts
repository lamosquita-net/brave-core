// Copyright (c) 2026 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

// FlyWeb: Brave's Toolbar and Tabs cards of the Appearance section. In
// Chromium 156 settings-appearance-page-index is a Lit element (Brave 1.98
// adds these views into it with lit_mangler, which FlyWeb does not ship), so
// they live in this second plugin, placed by brave_overrides/settings_main.ts
// in settings-main's appearance view next to the upstream one. Being Polymer
// and bound to settings-main's prefs, it keeps two-way pref bindings.

import 'chrome://resources/cr_elements/cr_view_manager/cr_view_manager.js'
import '/shared/settings/prefs/prefs.js'
import '../settings_page/settings_section.js'
import '../settings_shared.css.js'
import './tabs.js'
import './toolbar.js'

import type {CrViewManagerElement} from 'chrome://resources/cr_elements/cr_view_manager/cr_view_manager.js'
import {PolymerElement} from 'chrome://resources/polymer/v3_0/polymer/polymer_bundled.min.js'

import {routes} from '../route.js'
import {RouteObserverMixin} from '../router.js'
import type {Route} from '../router.js'
import type {SettingsPlugin} from '../settings_main/settings_plugin.js'
import {SearchableViewContainerMixin} from '../settings_page/searchable_view_container_mixin.js'

import {getTemplate} from './brave_appearance_extras.html.js'

export interface SettingsBraveAppearanceExtrasElement {
  $: {
    viewManager: CrViewManagerElement,
  }
}

const SettingsBraveAppearanceExtrasElementBase =
  SearchableViewContainerMixin(RouteObserverMixin(PolymerElement))

export class SettingsBraveAppearanceExtrasElement extends
  SettingsBraveAppearanceExtrasElementBase implements SettingsPlugin {
  static get is() {
    return 'settings-brave-appearance-extras'
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

    // Only on the Appearance page itself, not on its subpages.
    const show = newRoute === routes.APPEARANCE || newRoute === routes.BASIC
    this.hidden = !show
    if (show) {
      queueMicrotask(() => {
        this.$.viewManager.switchViews(
          ['toolbar', 'tabs'], 'no-animation', 'no-animation')
      })
    }
  }
}

declare global {
  interface HTMLElementTagNameMap {
    'settings-brave-appearance-extras': SettingsBraveAppearanceExtrasElement
  }
}

customElements.define(
  SettingsBraveAppearanceExtrasElement.is,
  SettingsBraveAppearanceExtrasElement)

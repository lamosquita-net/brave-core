// Copyright (c) 2026 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

// FlyWeb: Brave's System settings (keyboard shortcuts link, closing last
// tab, warn before closing window/quitting, fullscreen reminder). In
// Chromium 156 settings-system-page is a Lit element (Brave 1.98 adds these
// rows to it with lit_mangler, which FlyWeb does not ship), so they are a
// card of their own in settings-brave-system-page-index.

import 'chrome://resources/cr_elements/cr_link_row/cr_link_row.js'
import '../controls/settings_toggle_button.js'
import '../settings_page/settings_section.js'
import '../settings_shared.css.js'

import {PolymerElement} from 'chrome://resources/polymer/v3_0/polymer/polymer_bundled.min.js'

import {loadTimeData} from '../i18n_setup.js'
import {routes} from '../route.js'
import {Router} from '../router.js'
import {SettingsViewMixin} from '../settings_page/settings_view_mixin.js'

import {getTemplate} from './brave_system_extras.html.js'

const SettingsBraveSystemExtrasElementBase = SettingsViewMixin(PolymerElement)

export class SettingsBraveSystemExtrasElement extends
  SettingsBraveSystemExtrasElementBase {
  static get is() {
    return 'settings-brave-system-extras'
  }

  static get template() {
    return getTemplate()
  }

  static get properties() {
    return {
      prefs: {
        type: Object,
        notify: true,
      },
      shortcutsSupported_: {
        type: Boolean,
        value: () => loadTimeData.getBoolean('areShortcutsSupported'),
      },
    }
  }

  declare prefs: Record<string, unknown>
  declare private shortcutsSupported_: boolean

  private onShortcutsClick_() {
    Router.getInstance().navigateTo(routes.SHORTCUTS)
  }

  override getAssociatedControlFor(childViewId: string): HTMLElement {
    switch (childViewId) {
      case 'shortcuts':
        return this.shadowRoot!.querySelector<HTMLElement>('#shortcutsButton')!
      default:
        throw new Error(`Unknown child view id: ${childViewId}`)
    }
  }
}

declare global {
  interface HTMLElementTagNameMap {
    'settings-brave-system-extras': SettingsBraveSystemExtrasElement
  }
}

customElements.define(
  SettingsBraveSystemExtrasElement.is, SettingsBraveSystemExtrasElement)

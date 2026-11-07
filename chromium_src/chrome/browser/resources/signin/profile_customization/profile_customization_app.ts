// Copyright (c) 2026 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

// Elements used by the upstream template, imported explicitly so that
// @webui-eslint/web-component-missing-deps sees them (same imports as the
// upstream file, now kept as -chromium.ts).
import 'chrome://resources/cr_components/theme_color_picker/theme_color_picker.js';
import 'chrome://resources/cr_elements/cr_button/cr_button.js';
import 'chrome://resources/cr_elements/cr_icon/cr_icon.js';
import 'chrome://resources/cr_elements/cr_icon_button/cr_icon_button.js';
import 'chrome://resources/cr_elements/cr_input/cr_input.js';
import 'chrome://resources/cr_elements/cr_profile_avatar_selector/cr_profile_avatar_selector.js';
import 'chrome://resources/cr_elements/cr_tooltip/cr_tooltip.js';
import 'chrome://resources/cr_elements/cr_view_manager/cr_view_manager.js';
import 'chrome://resources/cr_elements/icons.html.js';
import 'chrome://resources/cr_elements/policy/cr_policy_indicator.js';
import '/strings.m.js';
import 'chrome://resources/cr_components/color_change_listener/colors_css_updater.js';
import 'chrome://resources/cr_elements/i18n_mixin_lit.js';
import 'chrome://resources/cr_elements/web_ui_listener_mixin_lit.js';
import 'chrome://resources/js/assert.js';
import 'chrome://resources/js/load_time_data.js';
import 'chrome://resources/lit/v3_0/lit.rollup.js';
import './profile_customization_browser_proxy.js';

import {
  ProfileCustomizationAppElement as ProfileCustomizationAppElementChromium,
} from './profile_customization_app-chromium.js'

// Merge onto the upstream type so getHtml.bind(this) in the chromium class
// still type-checks against the HTML template's ProfileCustomizationAppElement.
declare module './profile_customization_app-chromium.js' {
  interface ProfileCustomizationAppElement {
    onNameInputKeydown_(e: KeyboardEvent): void
  }
}

// Subclass so Enter-to-confirm is wired in firstUpdated on the registered
// element. Upstream's customElements.define is patched out; we register this
// subclass instead. Listen on inputElement (the real <input> in the shadow
// root) so the key event is received reliably.
class ProfileCustomizationAppElement extends
    ProfileCustomizationAppElementChromium {
  override firstUpdated() {
    super.firstUpdated()
    this.$.nameInput.inputElement.addEventListener(
      'keydown',
      (e: KeyboardEvent) => this.onNameInputKeydown_(e),
    )
  }

  override onNameInputKeydown_(e: KeyboardEvent) {
    if (e.key !== 'Enter' || this.$.doneButton.disabled) {
      return
    }
    e.preventDefault()
    this.$.doneButton.click()
  }
}

export { ProfileCustomizationAppElement }
export * from './profile_customization_app-chromium.js'

customElements.define(
  ProfileCustomizationAppElement.is,
  ProfileCustomizationAppElement,
)

// Copyright (c) 2020 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// you can obtain one at https://mozilla.org/MPL/2.0/.

// FlyWeb: settings-about-page is a Lit element in Chromium 156; as in Brave
// 1.98, style it with injectStyle and edit its rendered DOM once, after the
// first update (Polymer template modifications no longer apply).

import {injectStyle} from '//resources/brave/lit_overriding.js'
import {css} from '//resources/lit/v3_0/lit.rollup.js'
import type {PropertyValues} from '//resources/lit/v3_0/lit.rollup.js'

import {SettingsAboutPageElement} from '../about_page/about_page.js'

injectStyle(SettingsAboutPageElement, css`
  #release-notes {
    display: block;
    margin-inline-start: unset;
  }
`)

const modifyAboutPage = (root: ShadowRoot) => {
  if (!root.querySelector('a#release-notes')) {
    const version = root.querySelector('#updateStatusMessage ~ .secondary')
    if (!version) {
      console.error('[Settings] Could not find version div')
    } else {
      // Remove the class from the version, so we take the link styling.
      version.removeAttribute('class')

      const wrapper = document.createElement('a')
      wrapper.setAttribute('id', 'release-notes')
      wrapper.setAttribute('target', '_blank')
      wrapper.setAttribute('rel', 'noopener noreferrer')
      wrapper.setAttribute(
        'href', 'https://flyweb.lamosquita.net/descargas/#novedades')
      version.parentNode?.replaceChild(wrapper, version)
      wrapper.appendChild(version)
    }
  }

  // Help link shown if update fails
  const updateStatusMessageLink =
    root.querySelector<HTMLAnchorElement>('#updateStatusMessage a')
  if (updateStatusMessageLink) {
    // FlyWeb: our help page (macOS only).
    updateStatusMessageLink.href =
      'https://flyweb.lamosquita.net/ayuda/#actualizaciones'
  }
}

// `firstUpdated` is `protected` on ReactiveElement, so reach it through an
// untyped view of the prototype to patch it from outside the class hierarchy.
const proto = SettingsAboutPageElement.prototype as unknown as {
  firstUpdated?: (changedProperties: PropertyValues) => void
}
const originalFirstUpdated = proto.firstUpdated
proto.firstUpdated = function(
    this: SettingsAboutPageElement, changedProperties: PropertyValues) {
  originalFirstUpdated?.call(this, changedProperties)
  if (this.shadowRoot) {
    modifyAboutPage(this.shadowRoot)
  }
}

// Copyright (c) 2020 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// you can obtain one at https://mozilla.org/MPL/2.0/.

// FlyWeb: in Chromium 156 settings-appearance-page is a Lit element, so the
// Polymer template modifications this file used to register never ran. As in
// Brave 1.98, the upstream controls Brave moves elsewhere (or does not offer)
// are hidden with CSS injected into the element's styles; hiding instead of
// removing also keeps the nodes the element itself references ($ cache,
// focus config). Brave 1.98 inserts <settings-brave-appearance-theme> into
// the template with a lit_mangler script; FlyWeb has no such mechanism, so it
// is inserted into the rendered DOM once, after the first render (as
// about_page.ts does), as the first row of the Appearance card, where Brave
// shows it.

import '../brave_appearance_page/brave_theme.js'

import {injectStyle} from '//resources/brave/lit_overriding.js'
import {css} from '//resources/lit/v3_0/lit.rollup.js'
import type {PropertyValues} from '//resources/lit/v3_0/lit.rollup.js'

import {SettingsAppearancePageElement} from '../appearance_page/appearance_page.js'
import {SettingsAppearancePageIndexElement} from '../appearance_page/appearance_page_index.js'

injectStyle(SettingsAppearancePageElement, css`
  /* Colour scheme: Brave's own theme setting replaces it. */
  #colorSchemeModeRow,

  /* Home button: moved into the address bar section. */
  settings-toggle-button[pref-key="browser.show_home_button"],
  #home-button-options,

  /* Font size, custom fonts and page zoom. */
  .cr-row:has(#defaultFontSize),
  #customize-fonts-subpage-trigger,
  .cr-row:has(#pageZoom),

  /* macOS: confirm to quit and tabs to links live elsewhere in Brave. */
  settings-toggle-button[pref-key="browser.confirm_to_quit"],
  settings-toggle-button[pref-key="webkit.webprefs.tabs_to_links"],

  /* Hover card toggles: already in the Tabs settings. */
  div.hr:has(+ #hoverCardMemoryUsageToggle),
  #hoverCardMemoryUsageToggle,
  .cr-row:has(+ .list-frame #hoverCardImagesToggle),
  .list-frame:has(#hoverCardImagesToggle),

  /* Side panel position: Brave has its own side panel section. */
  .cr-row:has(+ .list-frame #sidePanelPosition),
  .list-frame:has(#sidePanelPosition) {
    display: none !important;
  }

  /* Brave's theme row, first in the card (inserted below). */
  settings-brave-appearance-theme {
    display: block;
  }

  /* #themeRow is no longer the first row: give it its separator back. */
  settings-brave-appearance-theme + #themeRow {
    border-top: var(--cr-separator-line);
  }
`)

// FlyWeb: in 156 the Appearance card is a view of the index's cr-view-manager,
// which positions views absolutely, so settings-appearance-page-index took no
// height and Brave's Toolbar and Tabs cards (settings_main.ts puts them right
// after it, in the same view) were painted over the top of the Appearance
// card: the theme row, "Themes" and "Customize toolbar" were hidden and the
// titles overlapped. Keep the visible view in the flow, as
// brave_appearance_extras.html already does for Brave's own cards.
injectStyle(SettingsAppearancePageIndexElement, css`
  cr-view-manager [slot=view]:not(.closing) {
    position: initial;
  }
`)

// FlyWeb: Brave's theme row (light / dark / same as macOS), first row of the
// Appearance card. It writes Brave's own dark mode setting through
// BraveAppearanceHandler (setBraveThemeType). #themeRow is the first static
// node of the 156 template's default route (not inside a conditional part),
// so Lit never re-creates it and the inserted sibling stays. The row is a
// Polymer element with its own data, so it needs no properties from the page.
function insertBraveThemeRow(root: ShadowRoot) {
  if (root.querySelector('settings-brave-appearance-theme')) {
    return
  }
  const themeRow = root.querySelector('#themeRow')
  if (!themeRow) {
    console.error('[FlyWeb] appearance page: #themeRow not found')
    return
  }
  themeRow.before(document.createElement('settings-brave-appearance-theme'))
}

// `firstUpdated` is `protected` on ReactiveElement, so reach it through an
// untyped view of the prototype to patch it from outside the class hierarchy.
const proto = SettingsAppearancePageElement.prototype as unknown as {
  firstUpdated?: (changedProperties: PropertyValues) => void
}
const originalFirstUpdated = proto.firstUpdated
proto.firstUpdated = function(
    this: SettingsAppearancePageElement, changedProperties: PropertyValues) {
  originalFirstUpdated?.call(this, changedProperties)
  if (this.shadowRoot) {
    insertBraveThemeRow(this.shadowRoot)
  }
}

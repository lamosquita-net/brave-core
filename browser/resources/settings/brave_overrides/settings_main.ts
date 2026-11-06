// Copyright (c) 2025 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

// Chromium 156 shows each top-level section as a "plugin" view of
// settings-main's cr-view-manager (settings-basic-page is gone). As in Brave
// 1.98, add FlyWeb's sections as more plugin views. Every plugin element
// implements SettingsPlugin (searchContents) through
// SearchableViewContainerMixin.

import {
  html,
  RegisterPolymerTemplateModifications
} from 'chrome://resources/brave/polymer_overriding.js'

import '../brave_appearance_page/brave_appearance_extras.js'
import '../brave_content_page/content_page_index.js'
import '../brave_default_extensions_page/brave_extensions_page_index.js'
import '../brave_search_engines_page/brave_search_extras.js'
import '../brave_sync_page/brave_sync_page_index.js'
import '../brave_system_page/brave_system_page_index.js'
import '../default_brave_shields_page/shields_page_index.js'
import '../getting_started_page/getting_started_page_index.js'

RegisterPolymerTemplateModifications({
  'settings-main': (templateContent) => {
    const switcher = templateContent.querySelector('#switcher')
    if (!switcher) {
      console.error('[Settings] Missing #switcher on settings-main')
      return
    }

    // Remove the performance page: its route is deleted (brave_routes.ts)
    // and its contents live in the system section.
    const templatePerformancePageSlot = templateContent.querySelector(
      'template[is=dom-if][if="[[showPage_(pageVisibility_.performance)]]"]')
    if (templatePerformancePageSlot) {
      templatePerformancePageSlot.remove()
    } else {
      console.error('[Settings] Missing template for performance page slot')
    }

    // Get started (people/profile, new tab page, default browser, startup).
    switcher.appendChild(html`
      <template is="dom-if" if="[[showPage_(pageVisibility_.getStarted)]]">
        <div slot="view" id="getStarted">
          <template is="dom-if" if="[[renderPlugin_(
              routes_.GET_STARTED, lastRoute_, inSearchMode_)]]">
            <settings-getting-started-page-index
                prefs="{{prefs}}"
                in-search-mode="[[inSearchMode_]]">
            </settings-getting-started-page-index>
          </template>
        </div>
      </template>
    `)

    // Content (fonts, zoom, tab cycling).
    switcher.appendChild(html`
      <template is="dom-if" if="[[showPage_(pageVisibility_.content)]]">
        <div slot="view" id="content">
          <template is="dom-if" if="[[renderPlugin_(
              routes_.BRAVE_CONTENT, lastRoute_, inSearchMode_)]]">
            <settings-brave-content-page-index
                prefs="{{prefs}}"
                in-search-mode="[[inSearchMode_]]">
            </settings-brave-content-page-index>
          </template>
        </div>
      </template>
    `)

    // Shields (defaults, social media blocking, content filters).
    switcher.appendChild(html`
      <template is="dom-if" if="[[showPage_(pageVisibility_.shields)]]">
        <div slot="view" id="shields">
          <template is="dom-if" if="[[renderPlugin_(
              routes_.SHIELDS, lastRoute_, inSearchMode_)]]">
            <settings-shields-page-index
                prefs="{{prefs}}"
                in-search-mode="[[inSearchMode_]]">
            </settings-shields-page-index>
          </template>
        </div>
      </template>
    `)

    // Sync (FlyWeb's own sync server).
    switcher.appendChild(html`
      <template is="dom-if" if="[[showPage_(pageVisibility_.braveSync)]]">
        <div slot="view" id="braveSync">
          <template is="dom-if" if="[[renderPlugin_(
              routes_.BRAVE_SYNC, lastRoute_, inSearchMode_)]]">
            <settings-brave-sync-page-index
                prefs="{{prefs}}"
                in-search-mode="[[inSearchMode_]]">
            </settings-brave-sync-page-index>
          </template>
        </div>
      </template>
    `)

    // Extensions (Google login for extensions, media router, Widevine...).
    switcher.appendChild(html`
      <template is="dom-if" if="[[showPage_(pageVisibility_.extensions)]]">
        <div slot="view" id="extensions">
          <template is="dom-if" if="[[renderPlugin_(
              routes_.EXTENSIONS, lastRoute_, inSearchMode_)]]">
            <settings-brave-extensions-page-index
                prefs="{{prefs}}"
                in-search-mode="[[inSearchMode_]]">
            </settings-brave-extensions-page-index>
          </template>
        </div>
      </template>
    `)

    // Appearance: Brave's Toolbar and Tabs cards, after the upstream
    // (Lit) appearance plugin, in the same view.
    const appearanceView = templateContent.querySelector<HTMLTemplateElement>(
      'template[is=dom-if][if="[[showPage_(pageVisibility_.appearance)]]"]')
    const appearanceDiv = appearanceView?.content.querySelector('#appearance')
    if (!appearanceDiv) {
      console.error('[Settings] Missing appearance view on settings-main')
    }
    appearanceDiv?.appendChild(html`
      <template is="dom-if" if="[[renderPlugin_(
          routes_.APPEARANCE, lastRoute_, inSearchMode_)]]">
        <settings-brave-appearance-extras
            prefs="{{prefs}}"
            in-search-mode="[[inSearchMode_]]">
        </settings-brave-appearance-extras>
      </template>
    `)

    // Search engine: Brave's private window search engine, suggestions and
    // "other search engines" toggles, after the upstream (Lit) plugin.
    const searchDiv = templateContent.querySelector('#switcher > #search')
    if (!searchDiv) {
      console.error('[Settings] Missing search view on settings-main')
    }
    searchDiv?.appendChild(html`
      <template is="dom-if" if="[[renderPlugin_(
          routes_.SEARCH, lastRoute_, inSearchMode_)]]">
        <settings-brave-search-extras
            prefs="{{prefs}}"
            in-search-mode="[[inSearchMode_]]">
        </settings-brave-search-extras>
      </template>
    `)

    // System: upstream settings-system-page plus Brave's toggles, memory and
    // battery (the performance section is removed) and keyboard shortcuts.
    const templateSystemPageSlot = templateContent.querySelector(
      'template[is=dom-if][if="[[showPage_(pageVisibility_.system)]]"]')
    if (!templateSystemPageSlot) {
      console.error('[Settings] Missing template for system page slot')
      return
    }
    templateSystemPageSlot.replaceWith(html`
      <template is="dom-if" if="[[showPage_(pageVisibility_.system)]]">
        <div slot="view" id="system">
          <template is="dom-if" if="[[renderPlugin_(
              routes_.SYSTEM, lastRoute_, inSearchMode_)]]">
            <settings-brave-system-page-index
                prefs="{{prefs}}"
                in-search-mode="[[inSearchMode_]]">
            </settings-brave-system-page-index>
          </template>
        </div>
      </template>
    `)
  }
})

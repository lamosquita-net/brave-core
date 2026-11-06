/* Copyright (c) 2025 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

// Chromium 156 privacy plugin (settings-privacy-page-index): as in Brave
// 1.98, hide the privacy guide promo and add FlyWeb's data collection card
// (crash reports, usage ping) after the privacy card.

import '../brave_data_collection_page/brave_data_collection_page.js'
import '../settings_page/settings_section.js'

import {
  html,
  RegisterPolymerPrototypeModification,
  RegisterPolymerTemplateModifications
} from 'chrome://resources/brave/polymer_overriding.js'

import {loadTimeData} from '../i18n_setup.js'

RegisterPolymerPrototypeModification({
  'settings-privacy-page-index': (prototype) => {
    const oldGetDefaultViews = prototype.getDefaultViews_
    prototype.getDefaultViews_ = function () {
      const views = oldGetDefaultViews.call(this)
        // Hide the privacy guide promo view.
        .filter((v: string) => v !== 'privacyGuidePromo')
      // Add the data collection view after the privacy view.
      views.splice(1, 0, 'dataCollection')
      return views
    }
  }
})

RegisterPolymerTemplateModifications({
  'settings-privacy-page-index': (templateContent) => {
    const viewManager = templateContent.querySelector('#viewManager')
    if (!viewManager) {
      console.error(
        '[Settings] Missing view manager on settings-privacy-page-index')
      return
    }

    // Hide the privacy guide promo
    const privacyGuidePromoTemplate = templateContent.querySelector(
      'template[is=dom-if][if="[[isPrivacyGuideAvailable]]"]')
    if (privacyGuidePromoTemplate) {
      privacyGuidePromoTemplate.remove()
    } else {
      console.error('[Settings] Missing privacyGuidePromoTemplate')
    }

    viewManager.appendChild(html`
      <settings-section slot="view" id="dataCollection"
          page-title="${loadTimeData.getString('braveDataCollection')}">
        <settings-brave-data-collection-subpage prefs="{{prefs}}">
        </settings-brave-data-collection-subpage>
      </settings-section>
    `)
  }
})

/* Copyright (c) 2025 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

// Chromium 156 privacy plugin (settings-privacy-page-index): as in Brave
// 1.98, hide the privacy guide promo, add FlyWeb's data collection card
// (crash reports, usage ping) after the privacy card and add the site
// settings subpages FlyWeb keeps (autoplay, Shields status) as views.
//
// Every view of a privacy child route must carry slot="view" and a
// route-path equal to the route's path: getViewIdsForRoute_() looks the view
// up by route-path and asserts that it exists before calling switchViews().

import '../brave_data_collection_page/brave_data_collection_page.js'
import '../settings_page/settings_section.js'
import '../site_settings/site_settings_autoplay.js'
import '../site_settings/site_settings_shields.js'

import {
  html,
  RegisterPolymerPrototypeModification,
  RegisterPolymerTemplateModifications
} from 'chrome://resources/brave/polymer_overriding.js'

import {loadTimeData} from '../i18n_setup.js'
import {ContentSettingsTypes} from '../site_settings/constants.js'

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

    // FlyWeb: Brave's site settings subpages, as in Brave 1.98 (its Google
    // sign-in subpage is not kept: in Chromium 156 FedCM and Storage Access
    // cover it; nor the localhost access one, whose permission is gone).
    viewManager.appendChild(html`
      <template is="dom-if" if="[[renderView_(
          routes_.SITE_SETTINGS_SHIELDS_STATUS, currentRoute,
          inSearchMode)]]" update-when-false>
        <site-settings-shields-page
            id="${ContentSettingsTypes.BRAVE_SHIELDS}"
            route-path$="[[routes_.SITE_SETTINGS_SHIELDS_STATUS.path]]"
            data-parent-view-id="privacy"
            slot="view">
        </site-settings-shields-page>
      </template>
    `)

    viewManager.appendChild(html`
      <template is="dom-if" if="[[renderView_(
          routes_.SITE_SETTINGS_AUTOPLAY, currentRoute,
          inSearchMode)]]" update-when-false>
        <site-settings-autoplay-page
            id="${ContentSettingsTypes.AUTOPLAY}"
            route-path$="[[routes_.SITE_SETTINGS_AUTOPLAY.path]]"
            data-parent-view-id="privacy"
            slot="view">
        </site-settings-autoplay-page>
      </template>
    `)
  }
})

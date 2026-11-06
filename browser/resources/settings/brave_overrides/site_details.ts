// Copyright (c) 2021 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// you can obtain one at https://mozilla.org/MPL/2.0/.

// @ts-nocheck TODO(petemill): Define types and remove ts-nocheck

import {RegisterPolymerTemplateModifications} from 'chrome://resources/brave/polymer_overriding.js'
import {getTrustedHTML} from 'chrome://resources/js/static_types.js'

import {loadTimeData} from '../i18n_setup.js'

import 'chrome://resources/brave/leo.bundle.js'

RegisterPolymerTemplateModifications({
  'site-details': (templateContent) => {
    if (!loadTimeData.getBoolean('isIdleDetectionFeatureEnabled')) {
      const idleDetectionItem = templateContent.querySelector('[category="[[contentSettingsTypesEnum_.IDLE_DETECTION]]"]')
      if (!idleDetectionItem) {
        console.error(`[Brave Settings Overrides] Couldn't find idle detection item`)
      } else {
        idleDetectionItem.hidden = true
      }
    }
    const adsItem = templateContent.querySelector('[category="[[contentSettingsTypesEnum_.ADS]]"]')
    if (!adsItem) {
      console.error(`[Brave Settings Overrides] Couldn't find ads item`)
    } else {
      adsItem.hidden = true
    }
    const firstPermissionItem = templateContent.querySelector('div.list-frame > site-details-permission:nth-child(1)')
    if (!firstPermissionItem) {
      console.error(`[Brave Settings Overrides] Couldn't find first permission item`)
    } else {
      firstPermissionItem.insertAdjacentHTML(
        'beforebegin',
        getTrustedHTML`
          <site-details-permission
            category="[[contentSettingsTypesEnum_.AUTOPLAY]]"
            icon="autoplay-on">
          </site-details-permission>
        `)
      const curChild = 1
      const autoplaySettings = templateContent.
        querySelector(`div.list-frame > site-details-permission:nth-child(${curChild})`)
      if (!autoplaySettings) {
        console.error(
          '[Brave Settings Overrides] Couldn\'t find autoplay settings')
      }
      else {
        autoplaySettings.setAttribute(
          'label', loadTimeData.getString('siteSettingsAutoplay'))
      }
      // FlyWeb: no Google sign-in permission (in Chromium 156 FedCM and
      // Storage Access cover it) and no localhost access one (gone).
    }

    // As Brave 1.98: no ad personalization line in the clear storage
    // dialog (FlyWeb has no Google ad personalization).
    const adPersonalization =
      templateContent.querySelector('#adPersonalization')
    if (!adPersonalization) {
      console.error(
        '[Brave Settings Overrides] Could not find adPersonalization element')
    } else {
      adPersonalization.remove()
    }

    // In Chromium, the VR and AR icons are the same but we want to have separate ones.
    templateContent.querySelector('site-details-permission[icon="settings:vr-headset"]')?.setAttribute('icon', 'smartphone-hand')
  }
})

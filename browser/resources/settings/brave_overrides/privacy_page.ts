/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

import {
  RegisterPolymerPrototypeModification,
  RegisterPolymerTemplateModifications
} from 'chrome://resources/brave/polymer_overriding.js'
import {getTrustedHTML} from 'chrome://resources/js/static_types.js'

import {ContentSettingsTypes} from '../site_settings/constants.js'

// FlyWeb: Brave 1.98's version for Chromium 156 (settings-privacy-page is a
// single settings-section now). The Brave site settings subpages (autoplay,
// Shields status) are views of settings-privacy-page-index
// (privacy_page_index.ts).

// Registers <settings-brave-personalization-options>, which is inserted into
// the template below.
import '../brave_privacy_page/brave_personalization_options.js'

RegisterPolymerTemplateModifications({
  'settings-privacy-page': (templateContent) => {
    const section = templateContent.querySelector('settings-section')
    if (!section) {
      console.error(
        `[Settings] Couldn't find privacy_page settings-section`)
      return
    }

    const siteSettingsLinkRow =
      templateContent.getElementById('siteSettingsLinkRow')
    if (!siteSettingsLinkRow) {
      console.error(
        '[Brave Settings Overrides] Couldn\'t find siteSettingsLinkRow')
    } else {
      siteSettingsLinkRow.insertAdjacentHTML(
        'afterend',
        getTrustedHTML`
          <settings-brave-personalization-options prefs="{{prefs}}">
          </settings-brave-personalization-options>
        `)
    }
    const thirdPartyCookiesLinkRow =
      templateContent.getElementById('thirdPartyCookiesLinkRow')
    if (!thirdPartyCookiesLinkRow) {
      console.error(
        '[Brave Settings Overrides] Could not find ' +
        'thirdPartyCookiesLinkRow id on privacy page.')
    } else {
      thirdPartyCookiesLinkRow.setAttribute('hidden', 'true')
    }

    const showPrivacyGuideEntryPointTemplate =
      templateContent.querySelector(`template[if*='isPrivacyGuideAvailable']`)
    if (!showPrivacyGuideEntryPointTemplate) {
      console.error(
        '[Brave Settings Overrides] Could not find template with' +
        ' if*=isPrivacyGuideAvailable on privacy page.')
    } else {
      const privacyGuideLinkRow = showPrivacyGuideEntryPointTemplate.content.
        getElementById('privacyGuideLinkRow')
      if (!privacyGuideLinkRow) {
        console.error(
          '[Brave Settings Overrides] Could not find privacyGuideLinkRow id' +
          ' on privacy page.')
      } else {
        privacyGuideLinkRow.setAttribute('hidden', 'true')
      }
    }
  }
})

// FlyWeb: the site settings subpages added in privacy_page_index.ts name
// "privacy" as their parent view. When a search hits one of them,
// SearchableViewContainerMixin asks this page for the control that leads to
// it; upstream only knows its own ids and would hit assertNotReached().
const BRAVE_SITE_SETTINGS_VIEW_IDS: string[] = [
  ContentSettingsTypes.AUTOPLAY,
  ContentSettingsTypes.BRAVE_SHIELDS,
  ContentSettingsTypes.GOOGLE_SIGN_IN,
]

RegisterPolymerPrototypeModification({
  'settings-privacy-page': (prototype) => {
    const oldGetAssociatedControlFor = prototype.getAssociatedControlFor
    prototype.getAssociatedControlFor = function(childViewId: string) {
      if (BRAVE_SITE_SETTINGS_VIEW_IDS.includes(childViewId)) {
        const control =
          this.shadowRoot.querySelector('#siteSettingsLinkRow')
        if (control) {
          return control
        }
      }
      return oldGetAssociatedControlFor.call(this, childViewId)
    }
  }
})

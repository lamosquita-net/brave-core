// Copyright (c) 2020 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

import '../brave_appearance_page/tabs.js'
import '../brave_appearance_page/toolbar.js'
import '../brave_content_page/content.js'
import '../brave_data_collection_page/brave_data_collection_page.js'
import '../brave_default_extensions_page/brave_default_extensions_page.js'
import '../brave_new_tab_page/brave_new_tab_page.js'
import '../brave_search_engines_page/brave_search_engines_page.js'
import '../brave_sync_page/brave_sync_page.js'
import '../default_brave_shields_page/default_brave_shields_page.js'
import '../getting_started_page/getting_started.js'
import '../social_blocking_page/social_blocking_page.js'

import {html, RegisterPolymerTemplateModifications, RegisterStyleOverride} from 'chrome://resources/brave/polymer_overriding.js'

import {loadTimeData} from '../i18n_setup.js'

const isGuest = loadTimeData.getBoolean('isGuest')

export function getSectionElement (
  templateContent: HTMLTemplateElement,
  sectionName: string)
{
  const sectionEl = templateContent.querySelector(`template[if*='showPage_(pageVisibility.${sectionName}']`) ||
    templateContent.querySelector(`template[if*='pageVisibility.${sectionName}']`) ||
    templateContent.querySelector(`settings-section[section="${sectionName}"]`)
  if (!sectionEl) {
    console.error(`[Brave Settings Overrides] Could not find section '${sectionName}'`)
  }
  return sectionEl
}

/**
 * Creates a settings-section element with a single child and returns it.
 * @param {string} sectionName - value of the section attribute
 * @param {string} titleName - loadTimeData key for page-title
 * @param {string} childName - name of child element
 * @param {Object} childAttributes - key-value pairs of child element attributes
 * @returns {Element}
 */
function createSectionElement (
  sectionName: string,
  titleName: string,
  childName: string,
  childAttributes: Record<string, string>)
{
  const childAttributesString = Object.keys(childAttributes).map(attribute =>
      `${attribute}="${childAttributes[attribute]}"`)
    .join(' ')
  // This needs to be inside a template so that our components do not get created immediately.
  // Otherwise the polymer bindings won't be setup correctly at first.
  return html`
    <settings-section page-title="${loadTimeData.getString(titleName)}" section="${sectionName}">
      <${childName}
        ${childAttributesString}
      >
      </${childName}>
    </settings-section>
  `
}

function createNestedSectionElement (
  sectionName: string,
  nestedUnder: string,
  titleName: string,
  childName: string,
  childAttributes: Record<string, string>)
{
  const childAttributesString = Object.keys(childAttributes).map(attribute =>
    `${attribute}="${childAttributes[attribute]}"`)
    .join(' ')
  // This needs to be inside a template so that our components do not get created immediately.
  // Otherwise the polymer bindings won't be setup correctly at first.
  return html`
    <settings-section id='${sectionName}-section' page-title="${loadTimeData.getString(titleName)}" section="${sectionName}" nest-under-section="${nestedUnder}">
      <${childName}
        ${childAttributesString}
      >
      </${childName}>
    </settings-section>
  `
}

RegisterStyleOverride(
  'settings-basic-page',
  html`
    <style>
      :host {
        min-width: 544px !important;
      }
    </style>
  ` as HTMLTemplateElement
)

RegisterPolymerTemplateModifications({
  'settings-basic-page': (templateContent) => {
    // Add 'Getting Started' section
    // Entire content is wrapped in another conditional template
    const actualTemplate = templateContent.querySelector('template')
    if (!actualTemplate) {
      console.error('[Brave Settings Overrides] Could not find basic-page template')
      return
    }
    const basicPageEl = actualTemplate.content.querySelector('#basicPage')
    if (!basicPageEl) {
      console.error('[Brave Settings Overrides] Could not find basicPage element to insert Getting Started section')
    } else {
      const privacyGuidePromoSection = actualTemplate.content.querySelector('#privacyGuidePromoSection')
      if (!privacyGuidePromoSection) {
        console.error('[Brave Settings Overrides] Could not find privacyGuidePromoSection element to hide')
      } else {
        privacyGuidePromoSection.remove()
      }
      const sectionGetStarted = document.createElement('template')
      sectionGetStarted.setAttribute('is', 'dom-if')
      sectionGetStarted.setAttribute('restamp', 'true')
      sectionGetStarted.setAttribute('if', '[[showPage_(pageVisibility.getStarted)]]')
      sectionGetStarted.content.appendChild(createSectionElement(
        'getStarted',
        'braveGetStartedTitle',
        'brave-settings-getting-started',
        {
          prefs: '{{prefs}}',
          'page-visibility': '[[pageVisibility]]'
        }
      ))
      const sectionToolbar = document.createElement('template')
      sectionToolbar.setAttribute('is', 'dom-if')
      sectionToolbar.setAttribute('restamp', 'true')
      sectionToolbar.setAttribute('if', '[[showPage_(pageVisibility.appearance)]]')
      sectionToolbar.content.appendChild(createNestedSectionElement(
        'toolbar',
        'appearance',
        'appearanceSettingsToolbarSection',
        'settings-brave-appearance-toolbar',
        {
          prefs: '{{prefs}}',
          'page-visibility': '[[pageVisibility]]'
        }
      ))
      const sectionTabs = document.createElement('template')
      sectionTabs.setAttribute('is', 'dom-if')
      sectionTabs.setAttribute('restamp', 'true')
      sectionTabs.setAttribute('if', '[[showPage_(pageVisibility.appearance)]]')
      sectionTabs.content.appendChild(createNestedSectionElement(
        'tabs',
        'appearance',
        'appearanceSettingsTabsSection',
        'settings-brave-appearance-tabs',
        {
          prefs: '{{prefs}}'
        }
      ))
      const sectionExtensions = document.createElement('template')
      sectionExtensions.setAttribute('is', 'dom-if')
      sectionExtensions.setAttribute('restamp', 'true')
      sectionExtensions.setAttribute('if', '[[showPage_(pageVisibility.extensions)]]')
      sectionExtensions.content.appendChild(createSectionElement(
        'extensions',
        'braveDefaultExtensions',
        'settings-brave-default-extensions-page',
        {
          prefs: '{{prefs}}'
        }
      ))
      const sectionDataCollection = document.createElement('template')
      sectionDataCollection.setAttribute('is', 'dom-if')
      sectionDataCollection.setAttribute('restamp', 'true')
      sectionDataCollection.
        setAttribute('if', '[[showPage_(pageVisibility.braveDataCollection)]]')
      sectionDataCollection.content.appendChild(createNestedSectionElement(
        'dataCollection',
        'privacy',
        'braveDataCollection',
        'settings-brave-data-collection-subpage',
        {
          prefs: '{{prefs}}'
        }
      ))
      const sectionSync = document.createElement('template')
      sectionSync.setAttribute('is', 'dom-if')
      sectionSync.setAttribute('restamp', 'true')
      sectionSync.setAttribute('if', '[[showPage_(pageVisibility.braveSync)]]')
      sectionSync.content.appendChild(createSectionElement(
        'braveSync',
        'braveSync',
        'settings-brave-sync-page',
        {}
      ))
      const sectionShields = document.createElement('template')
      sectionShields.setAttribute('is', 'dom-if')
      sectionShields.setAttribute('restamp', 'true')
      sectionShields.setAttribute('if', '[[showPage_(pageVisibility.shields)]]')
      sectionShields.content.appendChild(createSectionElement(
        'shields',
        'braveShieldsTitle',
        'settings-default-brave-shields-page',
        {
          prefs: '{{prefs}}'
        }
      ))
      const sectionSocialBlocking = document.createElement('template')
      sectionSocialBlocking.setAttribute('is', 'dom-if')
      sectionSocialBlocking.setAttribute('restamp', 'true')
      sectionSocialBlocking.setAttribute('if', '[[showPage_(pageVisibility.socialBlocking)]]')
      sectionSocialBlocking.content.appendChild(createNestedSectionElement(
        'socialBlocking',
        'shields',
        'socialBlocking',
        'settings-social-blocking-page',
        {
          prefs: '{{prefs}}'
        }
      ))
      const sectionContent = document.createElement('template')
      sectionContent.setAttribute('is', 'dom-if')
      sectionContent.setAttribute('restamp', 'true')
      sectionContent.setAttribute('if', '[[showPage_(pageVisibility.content)]]')
      sectionContent.content.appendChild(createNestedSectionElement(
        'content',
        'content',
        'contentSettingsContentSection',
        'settings-brave-content-content',
        {
          prefs: '{{prefs}}',
          'page-visibility': '[[pageVisibility]]'
        }
      ))

      const sectionNewTab = document.createElement('template')
      sectionNewTab.setAttribute('is', 'dom-if')
      sectionNewTab.setAttribute('restamp', 'true')
      sectionNewTab.setAttribute('if', '[[showPage_(pageVisibility.newTab)]]')
      sectionNewTab.content.appendChild(createNestedSectionElement(
        'newTab',
        'getStarted',
        'braveNewTab',
        'settings-brave-new-tab-page',
        {
          prefs: '{{prefs}}'
        }
      ))

      // Remove all hidden performance options from basic page.
      // We moved performance elements into system settings.
      const performanceTemplate = actualTemplate.content.querySelector(
        'template[if="[[showPerformancePage_(pageVisibility.performance)]]"]')
      if (performanceTemplate) {
        performanceTemplate.remove()
      }

      const batteryTemplate = actualTemplate.content.querySelector(
        'template[if="[[showBatteryPage_(pageVisibility.performance)]]"]')
      if (batteryTemplate) {
        batteryTemplate.remove()
      }

      const speedTemplate = actualTemplate.content.querySelector(
        'template[if="[[showSpeedPage_(pageVisibility.performance)]]"]')
      if (speedTemplate) {
        speedTemplate.remove()
      }

      // Get Started at top
      let last = basicPageEl.insertAdjacentElement('afterbegin',
        sectionGetStarted)
      // Insert New Tab
      last = last.insertAdjacentElement('afterend', sectionNewTab)
      // Move Appearance item
      const sectionAppearance = getSectionElement(actualTemplate.content,
        'appearance')
      last = last.insertAdjacentElement('afterend', sectionAppearance)
      // Insert nested Toolbar, Tabs under 'Appearance' menu
      last = last.insertAdjacentElement('afterend', sectionToolbar)
      last = last.insertAdjacentElement('afterend', sectionTabs)
      // Insert Content
      last = last.insertAdjacentElement('afterend', sectionContent)
      // Insert shields
      last = last.insertAdjacentElement('afterend', sectionShields)
      // Insert nested Social Blocking under shields
      last = last.insertAdjacentElement('afterend', sectionSocialBlocking)
      // Move privacy section to after shields
      const sectionPrivacy = getSectionElement(actualTemplate.content, 'privacy')
      last = last.insertAdjacentElement('afterend', sectionPrivacy)
      // Insert sync
      last = last.insertAdjacentElement('afterend', sectionSync)
      // Move search
      const sectionSearch = getSectionElement(actualTemplate.content, 'search')
      last = last.insertAdjacentElement('afterend', sectionSearch)
      // Insert extensions
      last = last.insertAdjacentElement('afterend', sectionExtensions)
      // Insert Data collection
      last.insertAdjacentElement('afterend', sectionDataCollection)

      // Advanced
      const advancedTemplate = templateContent.querySelector('template[if="[[showAdvancedSettings_(pageVisibility.advancedSettings)]]"]')
      if (!advancedTemplate) {
        console.error('[Brave Settings Overrides] Could not find advanced section')
      }
      const advancedSubSectionsTemplate = advancedTemplate.content.querySelector('settings-idle-load template')
      if (!advancedSubSectionsTemplate) {
        console.error('[Brave Settings Overrides] Could not find advanced sub-sections container')
      }
      // Move autofill to before languages
      const sectionAutofill = getSectionElement(actualTemplate.content, 'autofill')
      if (sectionAutofill) {
        const sectionLanguages =
          getSectionElement(advancedSubSectionsTemplate.content, 'languages')
        if (sectionLanguages) {
          sectionLanguages.insertAdjacentElement('beforebegin', sectionAutofill)
        }
      }
    }
  }
})

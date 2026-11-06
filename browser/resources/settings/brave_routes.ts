// Copyright (c) 2021 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// you can obtain one at https://mozilla.org/MPL/2.0/.

// @ts-nocheck TODO(petemill): Define types and remove ts-nocheck

import {pageVisibility} from './brave_overrides/page_visibility.js'
import {loadTimeData} from './i18n_setup.js'
import {SettingsRoutes} from 'chrome://settings/settings.js'

export default function addBraveRoutes(r: Partial<SettingsRoutes>) {
  const isGuest = loadTimeData.getBoolean('isGuest')
  if (!r.BASIC) {
    console.error('[Brave Settings Overrides] Routes: could not find BASIC page')
    return
  }
  if (pageVisibility.getStarted) {
    r.GET_STARTED = r.BASIC.createSection('/getStarted', 'getStarted')
    // bring back people's /manageProfile (now in getStarted)
    r.MANAGE_PROFILE = r.GET_STARTED.createChild('/manageProfile')
    // We re-section people page into getStarted section (see people_page Brave
    // override), so we need to adjust the route accordingly in order for the
    // direct navigation to flyweb://settings/importData to work.
    // FlyWeb: in Chromium 156 settings-main only renders the plugin whose
    // route contains the current one, so reparent it as well.
    // Without the people section (page_visibility.ts) Chromium 156 does not
    // create the route at all, so create it here as Brave 1.98 does.
    if (r.IMPORT_DATA) {
      r.IMPORT_DATA.section = 'getStarted'
      r.IMPORT_DATA.parent = r.GET_STARTED
      r.IMPORT_DATA.depth = r.GET_STARTED.depth + 1
    } else {
      r.IMPORT_DATA = r.GET_STARTED.createChild('/importData')
      r.IMPORT_DATA.isNavigableDialog = true
    }
    // FlyWeb: the new tab page card is a view of the getStarted plugin.
    r.BRAVE_NEW_TAB = r.GET_STARTED.createChild('/newTab')
  }
  r.SHIELDS = r.BASIC.createSection('/shields', 'shields')
  r.SHIELDS_ADBLOCK = r.SHIELDS.createChild('/shields/filters')
  if (loadTimeData.getBoolean('areShortcutsSupported')) {
    if (r.SYSTEM) {
      r.SHORTCUTS = r.SYSTEM.createChild('/system/shortcuts')
    } else if (!isGuest) {
      console.error('[Brave Settings Overrides] Routes: could not find SYSTEM page')
    }
  }
  r.EXTENSIONS = r.BASIC.createSection('/extensions', 'extensions')
  if (pageVisibility.braveSync) {
    r.BRAVE_SYNC = r.BASIC.createSection('/braveSync', 'braveSync')
    r.BRAVE_SYNC_SETUP = r.BRAVE_SYNC.createChild('/braveSync/setup')
  }
  if (pageVisibility.braveWeb3) {
    r.BRAVE_WEB3 = r.BASIC.createSection('/web3', 'web3')
  }

  if (pageVisibility.content) {
    r.BRAVE_CONTENT = r.BASIC.createSection('/braveContent', 'content')
    // Move fonts from APPEARANCE to BRAVE_CONTENT
    if (r.FONTS) {
        delete r.FONTS
    }
    r.FONTS = r.BRAVE_CONTENT.createChild('/fonts');
  }
  if (r.SITE_SETTINGS) {
    // FlyWeb: each of these routes needs its view in
    // settings-privacy-page-index (brave_overrides/privacy_page_index.ts),
    // with the same route-path. No localhost access route (the permission is
    // gone).
    r.SITE_SETTINGS_AUTOPLAY = r.SITE_SETTINGS.createChild('autoplay')
    if (loadTimeData.getBoolean('isGoogleSignInFeatureEnabled')) {
      r.SITE_SETTINGS_GOOGLE_SIGN_IN =
        r.SITE_SETTINGS.createChild('googleSignIn')
    }
    r.SITE_SETTINGS_SHIELDS_STATUS = r.SITE_SETTINGS.createChild('braveShields')
    if (r.SITE_SETTINGS_ADS) {
      delete r.SITE_SETTINGS_ADS
    } else {
      console.error('[Brave Settings Overrides] could not find expected route site_settings_ads')
    }
    // As Brave 1.98: no auto-verify (anti-abuse) subpage. Its only setting
    // gates Private State Tokens, which FlyWeb ships disabled (the
    // PrivateStateTokens runtime feature and its origin trial). Its view in
    // settings-privacy-page-index is removed in privacy_page_index.ts.
    if (r.SITE_SETTINGS_AUTO_VERIFY) {
      delete r.SITE_SETTINGS_AUTO_VERIFY
    } else {
      console.error('[Brave Settings Overrides] could not find expected route site_settings_auto_verify')
    }
  } else if (!isGuest) {
    console.error('[Brave Settings Overrides] Routes: could not find SITE_SETTINGS page')
  }
  // Autofill route is moved to advanced,
  // otherwise its sections won't show up when opened.
  if (r.AUTOFILL && r.ADVANCED) {
    r.AUTOFILL.parent = r.ADVANCED
  } else if (!isGuest) {
    console.error('[Brave Settings Overrides] Could not move autofill route to advanced route', r)
  }
  // Delete performance menu - system menu includes it instead.
  if (r.PERFORMANCE) {
    delete r.PERFORMANCE
  }
  // FlyWeb: unlike Brave, the Storage Access subpage stays (Chromium 156's
  // own route and view in settings-privacy-page-index).
  // Google sync setup (as Brave 1.98): its people plugin is not shown, and
  // FlyWeb's own sync lives in BRAVE_SYNC.
  if (r.SYNC) {
    delete r.SYNC
  }
  if (r.SYNC_ADVANCED) {
    delete r.SYNC_ADVANCED
  }
}

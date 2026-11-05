// Copyright (c) 2020 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

// Utils
import { loadTimeData } from '../../common/loadTimeData'

// FlyWeb: the page keeps nothing in localStorage any more (this key only held
// the state of the Rewards and Talk cards); clear what 2.7.0 left there.
const obsoleteKeyName = 'new-tab-data'

export const defaultState: NewTab.State = {
  initialDataLoaded: false,
  textDirection: loadTimeData.getString('textdirection'),
  featureCustomBackgroundEnabled: loadTimeData.getBoolean('featureCustomBackgroundEnabled'),
  searchPromotionEnabled: false,
  showBackgroundImage: false,
  showStats: false,
  showClock: false,
  clockFormat: '',
  showTopSites: false,
  customLinksEnabled: false,
  customLinksNum: 0,
  showSearchBox: true,
  promptEnableSearchSuggestions: true,
  searchSuggestionsEnabled: false,
  showEmptyPage: false,
  isIncognito: chrome.extension.inIncognitoContext,
  useAlternativePrivateSearchEngine: false,
  showAlternativePrivateSearchEngineToggle: false,
  stats: {
    adsBlockedStat: 0,
    javascriptBlockedStat: 0,
    bandwidthSavedStat: 0,
    httpsUpgradesStat: 0,
    fingerprintingBlockedStat: 0
  },
  customImageBackgrounds: []
}

export const load = (): NewTab.State => {
  if (window.localStorage.getItem(obsoleteKeyName)) {
    window.localStorage.removeItem(obsoleteKeyName)
  }
  return defaultState
}

// Copyright (c) 2019 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// you can obtain one at https://mozilla.org/MPL/2.0/.

declare namespace NewTab {
  // Custom background with solid color or gradation
  export type ColorBackground = {
    type: 'color'
    wallpaperColor: string
    random?: boolean
  }

  // Backgrounds based on image. Custom image background or Brave background.
  export type ImageBackground = {
    type: 'image'
    wallpaperImageUrl: string
    random?: boolean
  }

  export type BraveBackground = Omit<ImageBackground, 'type'> & {
    type: 'brave'
    author: string
    link?: string
    originalUrl?: string
    license?: string

    // Picked by our rotating algorithm. When it's false, it means that the user
    // has picked this background.
    random?: boolean

    // FlyWeb: our bundled backgrounds carry the colours of the texts on them.
    flyweb?: FlyWebColores
  }

  // FlyWeb: colour of the labels (counters, top sites) and of the photo credit.
  export type FlyWebColores = {
    texto: string
    pie: string
  }

  export type BackgroundWallpaper = ColorBackground | ImageBackground | BraveBackground

  // FlyWeb: only the user's own background comes from the browser.
  export interface Wallpaper {
    backgroundWallpaper?: BackgroundWallpaper
  }

  export interface ApplicationState {
    newTabData: State | undefined
    gridSitesData: GridSitesState | undefined
  }

  export interface Site {
    id: string
    url: string
    title: string
    favicon: string
    letter: string
    pinnedIndex: number | undefined
  }

  export interface Stats {
    adsBlockedStat: number
    httpsUpgradesStat: number
    javascriptBlockedStat: number
    bandwidthSavedStat: number
    fingerprintingBlockedStat: number
  }

  export interface Bookmark {
    dateAdded: number
    id: string
    index: number
    parentId: string
    title: string
    url: string
  }

  export interface GridSitesState {
    removedSites: Site[]
    gridSites: Site[]
    shouldShowSiteRemovedNotification: boolean
  }

  export interface PageState {
    showEmptyPage: boolean
  }

  export interface PersistentState {
    showEmptyPage: boolean
  }

  export type Preferences = {
    showBackgroundImage: boolean
    showStats: boolean
    showClock: boolean
    clockFormat: string
    showTopSites: boolean
    showSearchBox: boolean
    promptEnableSearchSuggestions: boolean
    searchSuggestionsEnabled: boolean
  }

  export type EphemeralState = Preferences & {
    initialDataLoaded: boolean
    textDirection: string
    searchPromotionEnabled: boolean
    featureCustomBackgroundEnabled: boolean
    isIncognito: boolean
    useAlternativePrivateSearchEngine: boolean
    showAlternativePrivateSearchEngineToggle: boolean
    gridLayoutSize?: 'small'
    showGridSiteRemovedNotification?: boolean
    showBackgroundImage: boolean
    customLinksEnabled: boolean
    customLinksNum: number
    stats: Stats,
    backgroundWallpaper?: BackgroundWallpaper
    customImageBackgrounds: ImageBackground[]
  }

  interface StorybookStateExtras {
    forceSettingsTab?: string // SettingsTabType
  }

  // In-memory state is a superset of PersistentState
  export type State = PersistentState & EphemeralState & StorybookStateExtras
}

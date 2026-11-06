// Copyright (c) 2018 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

declare namespace chrome {
  function getVariableValue (variable: string): string
  function setVariableValue (variable: string, value: any): void
  function send (stat: string, args?: any[]): void
}

declare namespace chrome.dns {
  function resolve (hostname: string, callback: any): void
}

declare namespace chrome.settingsPrivate {
  // See chromium definition at
  // https://chromium.googlesource.com/chromium/src.git/+/master/chrome/common/extensions/api/settings_private.idl
  enum PrefType {
    BOOLEAN = 'BOOLEAN',
    NUMBER = 'NUMBER',
    STRING = 'STRING',
    URL = 'URL',
    LIST = 'LIST',
    DICTIONARY = 'DICTIONARY'
  }

  type PrefBooleanValue = {
    type: PrefType.BOOLEAN,
    value: boolean
  }
  type SettingsNumberValue = {
    type: PrefType.NUMBER,
    value: number
  }
  type SettingsStringValue = {
    type: PrefType.STRING,
    value: string
  }
  type PrefDictValue = {
    type: PrefType.DICTIONARY
    value: Object
  }
  // TODO(petemill): implement other types as needed

  type PrefObject = {
    key: string
  } & (PrefBooleanValue | PrefDictValue | SettingsNumberValue | SettingsStringValue)

  type GetPrefCallback = (pref: PrefObject) => void
  function getPref (key: string, callback: GetPrefCallback): void

  type SetPrefCallback = (success: boolean) => void
  function setPref (key: string, value: any, pageId?: string | null, callback?: SetPrefCallback): void
  function setPref (key: string, value: any, callback?: SetPrefCallback): void

  type GetAllPrefsCallback = (prefs: PrefObject[]) => void
  function getAllPrefs (callback: GetAllPrefsCallback): void

  type GetDefaultZoomCallback = (zoom: number) => void
  function getDefaultZoom (callback: GetDefaultZoomCallback): void

  type SetDefaultZoomCallback = (success: boolean) => void
  function setDefaultZoom (zoom: number, callback?: SetDefaultZoomCallback): void

  const onPrefsChanged: {
    addListener: (callback: (prefs: PrefObject[]) => void) => void
  }
}

declare namespace chrome.greaselion {
  const isGreaselionExtension: (id: string, callback: (valid: boolean) => void) => {}
}

type BlockTypes = 'shieldsAds' | 'trackers' | 'httpUpgradableResources' | 'javascript' | 'fingerprinting'

declare namespace chrome.tabs {
  const setAsync: any
  const getAsync: any
}

declare namespace chrome.windows {
  const getAllAsync: any
}

declare namespace chrome.braveShields {
  const addSiteCosmeticFilter: (origin: string, cssSelector: string) => void
  const openFilterManagementPage: () => void
}

declare namespace chrome.test {
  const sendMessage: (message: string) => {}
}

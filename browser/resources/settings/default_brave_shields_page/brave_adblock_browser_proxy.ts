/* Copyright (c) 2022 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

// @ts-nocheck TODO(petemill): Define types and remove ts-nocheck

import { sendWithPromise, addWebUiListener } from 'chrome://resources/js/cr.js';

// Types as in Brave 1.98 (plus last_updated_pretty_text, which our handler
// still sends).
export interface FilterList {
  uuid: string
  url: string
  title: string
  desc: string
  support_url: string
  component_id: string
  base64_public_key: string
  enabled: boolean
}

export interface SubscriptionInfo {
  subscription_url: string
  last_update_attempt: number
  last_successful_update_attempt: number
  last_updated_pretty_text?: string
  enabled: boolean
  title?: string
  homepage?: string
}

export interface BraveAdblockBrowserProxy {
  getRegionalLists(): Promise<FilterList[]>
  enableFilterList(uuid: string, enabled: boolean)
  updateFilterList(uuid: string): Promise<boolean>
  getListSubscriptions(): Promise<SubscriptionInfo[]>
  getCustomFilters(): Promise<string>
  setSubscriptionEnabled(url: string, enabled: boolean)
  addSubscription(url: string)
  addWebUiListener(eventName: string, callback: Function)
  updateSubscription(url: string)
  deleteSubscription(url: string)
  viewSubscription(url: string)
}

export class BraveAdblockBrowserProxyImpl implements BraveAdblockBrowserProxy {
  static getInstance() {
    return instance || (instance = new BraveAdblockBrowserProxyImpl());
  }

  /** @returns {Promise} */
  getRegionalLists () {
    return sendWithPromise<FilterList[]>('brave_adblock.getRegionalLists')
  }

  /** @returns {Promise} */
  getListSubscriptions () {
    return sendWithPromise<SubscriptionInfo[]>(
        'brave_adblock.getListSubscriptions')
  }

  /** @returns {Promise} */
  getCustomFilters () {
    return sendWithPromise<string>('brave_adblock.getCustomFilters')
  }

  enableFilterList (uuid, enabled) {
    chrome.send('brave_adblock.enableFilterList', [uuid, enabled])
  }

  /** @returns {Promise<boolean>} */
  updateFilterLists () {
    return sendWithPromise<boolean>('brave_adblock.updateFilterLists')
  }

  setSubscriptionEnabled (url, enabled) {
    chrome.send('brave_adblock.setSubscriptionEnabled', [url, enabled])
  }

  addSubscription (url) {
    chrome.send('brave_adblock.addSubscription', [url])
  }

  updateSubscription (url) {
    chrome.send('brave_adblock.updateSubscription', [url])
  }

  updateCustomFilters (value) {
    chrome.send('brave_adblock.updateCustomFilters', [value])
  }

  deleteSubscription (url) {
    chrome.send('brave_adblock.deleteSubscription', [url])
  }

  viewSubscription (url) {
    chrome.send('brave_adblock.viewSubscription', [url])
  }

  addWebUiListener (event_name, callback) {
    addWebUiListener(event_name, callback)
  }
}

let instance: BraveAdblockBrowserProxy|null = null

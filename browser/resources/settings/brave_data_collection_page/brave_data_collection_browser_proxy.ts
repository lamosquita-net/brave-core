/* Copyright (c) 2024 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

import {MetricsReporting} from '/shared/settings/privacy_page/privacy_page_browser_proxy.js'
import {sendWithPromise} from 'chrome://resources/js/cr.js'

export interface BraveDataCollectionBrowserProxy {
  setMetricsReportingEnabled: (enabled: boolean) => void
  getMetricsReporting: () => Promise<MetricsReporting>
}

export class BraveDataCollectionBrowserProxyImpl
implements BraveDataCollectionBrowserProxy
{
  static getInstance() {
    return instance || (instance = new BraveDataCollectionBrowserProxyImpl())
  }

  setMetricsReportingEnabled(enabled: boolean) {
    chrome.send('setMetricsReportingEnabled', [enabled])
  }

  getMetricsReporting() {
    return sendWithPromise<MetricsReporting>('getMetricsReporting')
  }
}

let instance: BraveDataCollectionBrowserProxy|null = null

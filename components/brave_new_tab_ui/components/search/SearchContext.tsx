// Copyright (c) 2024 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

import usePromise from '$web-common/usePromise';
import { AutocompleteResult, FreState, InputKeywordModel, InputMethod, OmniboxPopupSelection, PageHandlerFactory, PageHandlerRemote, PageInterface, PageReceiver, SelectedFileInfo, SelectionDirection, SelectionStep, TabInfo } from 'chrome://resources/mojo/components/omnibox/browser/searchbox.mojom-webui.js';
import { SuggestInventory } from 'chrome://resources/mojo/components/omnibox/browser/fusebox_action.mojom-webui.js';
import { ContextUploadErrorType, ContextUploadStatus, InputState } from 'chrome://resources/mojo/components/omnibox/composebox/composebox_query.mojom-webui.js';
import { WindowOpenDisposition } from 'chrome://resources/mojo/ui/base/mojom/window_open_disposition.mojom-webui.js';
import { Size } from 'chrome://resources/mojo/ui/gfx/geometry/mojom/geometry.mojom-webui.js';
import { Url } from 'chrome://resources/mojo/url/mojom/url.mojom-webui.js';
import * as React from 'react';
import getNTPBrowserAPI, { SearchEngineInfo } from '../../api/background';
import { getDefaultSearchEngine, isSearchEngineEnabled, setDefaultSearchEngine } from './config';

interface Context {
  open: boolean,
  setOpen: (open: boolean) => void,
  query: string,
  setQuery: (query: string) => void
  searchEngine?: SearchEngineInfo,
  setSearchEngine: (searchEngine: SearchEngineInfo | string) => void
  searchEngines: SearchEngineInfo[]
  filteredSearchEngines: SearchEngineInfo[]
}

const Context = React.createContext<Context>({
  open: false,
  setOpen: () => {},
  query: '',
  setQuery: () => { },
  searchEngine: undefined,
  setSearchEngine: () => { },
  searchEngines: [],
  filteredSearchEngines: []
})

export const searchEnginesPromise = getNTPBrowserAPI().pageHandler.getSearchEngines().then(r => r.searchEngines)

export const omniboxController: PageHandlerRemote = new PageHandlerRemote();
(window as any).omnibox = omniboxController;

// Tracks the latest query sent for autocompletion, to tell stale results.
let activeQueryId = 0;

class SearchPage implements PageInterface {
  private receiver = new PageReceiver(this)
  private result: AutocompleteResult | undefined
  private resultListeners: Array<(result?: AutocompleteResult) => void> = []
  private selectionListeners: Array<(selection: OmniboxPopupSelection) => void> = []

  constructor() {
    PageHandlerFactory.getRemote().createPageHandler(
      this.receiver.$.bindNewPipeAndPassRemote(),
      omniboxController.$.bindNewPipeAndPassReceiver(),
    )
  }

  addResultListener(listener: (result?: AutocompleteResult) => void) {
    this.resultListeners.push(listener)
    if (this.result) listener(this.result)
  }

  removeResultListener(listener: (result?: AutocompleteResult) => void) {
    this.resultListeners = this.resultListeners.filter(r => r !== listener)
  }

  addSelectionListener(listener: (selection: OmniboxPopupSelection) => void) {
    this.selectionListeners.push(listener)
  }

  removeSelectionListener(listener: (selection: OmniboxPopupSelection) => void) {
    this.selectionListeners = this.selectionListeners.filter(s => s !== listener)
  }

  autocompleteResultChanged(result: AutocompleteResult) {
    this.result = result;
    for (const listener of this.resultListeners) listener(result)
  }

  updateSelection(oldSelection: OmniboxPopupSelection, selection: OmniboxPopupSelection) {
    for (const listener of this.selectionListeners) listener(selection)
  }

  // Composebox, AIM, Lens and tab-context hooks that the FlyWeb search
  // widget does not use.
  setFreState(state: FreState) { }
  stepSelection(direction: SelectionDirection, step: SelectionStep) { }
  openCurrentSelection(disposition: WindowOpenDisposition) { }
  setAimButtonVisible(visible: boolean) { }
  setAimButtonConfig(text: string, tooltip: string, a11yLabel: string, iconUrl: Url) { }
  setInputText(inputText: string) { }
  setKeywordSpaceTriggeringEnabled(enabled: boolean) { }
  setAvailableKeywordModels(models: InputKeywordModel[], defaultSearchProviderKeyword: string) { }
  setThumbnail(thumbnailUrl: string, isDeletable: boolean) { }
  onContextualInputStatusChanged(token: unknown, status: ContextUploadStatus, errorType: ContextUploadErrorType | null) { }
  onTabStripChanged() { }
  onInputStateChanged(inputState: InputState) { }
  addFileContext(token: unknown, fileInfo: SelectedFileInfo) { }
  updateAutoSuggestedTabContext(tab: TabInfo | null, invocationSource: string | null) { }
  updateLensSearchEligibility(eligible: boolean) { }
  updateContentSharingPolicy(enabled: boolean) { }
  updateAimPopupEligibility(eligible: boolean) { }
  updateSmartTabSharingActive(active: boolean) { }
  onPermissionPromptChanged(isShowing: boolean, promptSize: Size) { }
  setRestoredTabIds(ids: number[]) { }
  setAimThreadRestoredTabs(tabs: TabInfo[]) { }
  onScreenshotMenuClosed() { }
  updateProfileInfo(avatarUrl: Url, name: string, email: string) { }
}

export const search = new SearchPage()

export function SearchContext(props: React.PropsWithChildren<{}>) {
  const [open, setOpen] = React.useState(false)
  const [searchEngine, setSearchEngineInternal] = React.useState<SearchEngineInfo>()
  const [query, setQuery] = React.useState('')
  const { result: searchEngines = [] } = usePromise(() => searchEnginesPromise, [])
  const filteredSearchEngines = searchEngines.filter(isSearchEngineEnabled)

  const setSearchEngine = React.useCallback((engine: SearchEngineInfo | string) => {
    if (typeof engine === 'string') {
      engine = searchEngines.find(e => e.host === engine || e.keyword === engine)!
    }

    if (!engine) return

    setDefaultSearchEngine(engine)
    setSearchEngineInternal(engine)
  }, [searchEngines]);

  // When we receive search engines, use the first one as our keyword, if we don't have a match.
  React.useEffect(() => {
    if (!searchEngines.length) return

    const match = filteredSearchEngines.find(s => s.host === getDefaultSearchEngine())
      ?? searchEngines[0]
    setSearchEngine(match)
  }, [filteredSearchEngines])

  // When the query changes, notify the browser side.
  React.useEffect(() => {
    if (query) {
      const keywordQuery = `${searchEngine?.keyword} ${query}`
      omniboxController.queryAutocomplete(activeQueryId++, null, keywordQuery, false, keywordQuery.length, SuggestInventory.kDefault, false, '', InputMethod.kKeyboard);
    } else {
      omniboxController.stopAutocomplete(true)
    }
  }, [query, searchEngine])

  const setQueryExternal = React.useCallback((query: string) => {
    setOpen(true)
    setQuery(query)
  }, [])

  const context = React.useMemo(() => ({
    open,
    setOpen,
    searchEngine,
    setSearchEngine,
    query,
    setQuery: setQueryExternal,
    searchEngines,
    filteredSearchEngines
  }), [searchEngine, setSearchEngine, filteredSearchEngines, query, searchEngines, open])

  return <Context.Provider value={context}>
    {props.children}
  </Context.Provider>
}

export function useSearchContext() {
  return React.useContext(Context)
}

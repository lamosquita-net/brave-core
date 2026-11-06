/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

// Reducer
import { newTabReducers } from '../../../brave_new_tab_ui/reducers'

// API
import * as storage from '../../../brave_new_tab_ui/storage/new_tab_storage'

describe('newTabReducer', () => {
  describe('initial state', () => {
    it('loads initial data', () => {
      const expectedState = storage.load()
      const returnedState = newTabReducers(undefined, { type: {} })
      expect(returnedState).toEqual(expectedState)
    })
  })

  describe('NEW_TAB_SET_INITIAL_DATA', () => {
    // TODO
  })
  describe('NEW_TAB_STATS_UPDATED', () => {
    // TODO
  })
  describe('NEW_TAB_PRIVATE_TAB_DATA_UPDATED', () => {
    // TODO
  })
  describe('NEW_TAB_PREFERENCES_UPDATED', () => {
    // TODO
  })
})

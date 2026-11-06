// Copyright (c) 2020 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.
import * as newTabActions from '../actions/new_tab_actions'
import * as gridSitesActions from '../actions/grid_sites_actions'

export const enum types {
  NEW_TAB_STATS_UPDATED = '@@newtab/NEW_TAB_STATS_UPDATED',
  NEW_TAB_PRIVATE_TAB_DATA_UPDATED = '@@newtab/NEW_TAB_PRIVATE_TAB_DATA_UPDATED',
  NEW_TAB_TOR_TAB_DATA_UPDATED = '@@newtab/NEW_TAB_TOR_TAB_DATA_UPDATED',
  NEW_TAB_PREFERENCES_UPDATED = '@@newtab/NEW_TAB_PREFERENCES_UPDATED',
  NEW_TAB_SET_INITIAL_DATA = '@@newtab/NEW_TAB_SET_INITIAL_DATA',
  SET_MOST_VISITED_SITES = '@@newtab/SET_MOST_VISITED_SITES',
  TOP_SITES_STATE_UPDATED = '@@newtab/TOP_SITES_STATE_UPDATED',
  BACKGROUND_UPDATED = '@@newtab/_BACKGROUND_UPDATED',
  CUSTOM_IMAGE_BACKGROUNDS_UPDATED = '@@newtab/CUSTOM_IMAGE_BACKGROUNDS_UPDATED',
}

export type NewTabActions =
  typeof newTabActions &
  typeof gridSitesActions

// Copyright (c) 2020 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

import styled from 'styled-components'

import { familia } from '../flyweb/estilos'

export const StyledClock = styled('div')<{}>`
  color: var(--flyweb-tinta);
  box-sizing: border-box;
  line-height: 1;
  user-select: none;
  display: flex;
  -webkit-font-smoothing: antialiased;
  font-family: ${familia};
  letter-spacing: -1px;
`

export const StyledTime = styled('span')<{}>`
  box-sizing: border-box;
  font-size: 72px;
  font-weight: 700;
  color: inherit;
  display: inline-flex;
`

export const StyledTimeSeparator = styled('span')<{}>`
  box-sizing: border-box;
  color: inherit;
  font-size: inherit;
  font-weight: inherit;
  /* center colon vertically in the text-content line */
  margin-top: -0.1em;
`

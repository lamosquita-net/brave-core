/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

import styled from 'styled-components'

import { colores } from '../flyweb/recursos'
import { familia } from '../flyweb/estilos'

export const StyledStatsItemContainer = styled('ul')<{}>`
  -webkit-font-smoothing: antialiased;
  display: inline-flex;
  flex-wrap: wrap;
  justify-content: var(--ntp-item-justify, start);
  align-items: start;
  font-weight: 400;
  margin: 0;
  padding: 0;
  color: inherit;
  font-size: inherit;
  font-family: inherit;
`

export const StyledStatsItem = styled('li')<{}>`
  list-style-type: none;
  font-size: inherit;
  font-family: inherit;
  margin: 10px 16px;
  /* FlyWeb: the project's colours, one per counter, on any background. */
  &:first-child { color: ${colores.naranja}; }
  &:nth-child(2) { color: ${colores.rojo}; }
  &:last-child {
    color: ${colores.morado};
    margin-right: 0;
  }
`

export const StyledStatsItemCounter = styled('span')<{}>`
  color: inherit;
  font-family: ${familia};
  font-size: 34px;
  font-weight: 700;
  line-height: 1;
  width: 7ch;
  text-overflow: ellipsis;
  white-space: nowrap;
  overflow: hidden;
  word-wrap: none;
`

export const StyledStatsItemText = styled('span')<{}>`
  /* FlyWeb: the unit ("B", "segundos") in bold like its number (mockup). */
  font-size: 21px;
  font-weight: 700;
  font-family: ${familia};
  margin-left: 2px;
  display: inline;
  letter-spacing: 0;
`

export const StyledStatsItemDescription = styled('div')<{}>`
  /* FlyWeb: 16px, a little over the mockup's 14 (human's decision, 03/10). */
  font-size: 16px;
  font-weight: 400;
  color: var(--flyweb-texto);
  margin-top: 8px;
  font-family: ${familia};
`

// Copyright (c) 2026 lamosquita. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

// FlyWeb: D-DIN typeface and the colours that depend on the background.
// On one of our backgrounds the labels and the credit take that background's
// colours and the clock, "customize" and the bottom icons are black. On any
// other background (the user's own image or a colour) Brave's readability
// colour is kept.

import { createGlobalStyle, css } from 'styled-components'

import { fuentes } from './recursos'

export const familia = "'D-DIN', sans-serif"

export const EstiloFlyWeb = createGlobalStyle<{ colores?: NewTab.FlyWebColores }>`
  @font-face {
    font-family: 'D-DIN';
    src: url(${fuentes.regular}) format('woff2');
    font-weight: 400;
    font-display: block;
  }
  @font-face {
    font-family: 'D-DIN';
    src: url(${fuentes.bold}) format('woff2');
    font-weight: 700;
    font-display: block;
  }
  :root {
    ${p => p.colores ? css`
      --flyweb-texto: ${p.colores.texto};
      --flyweb-pie: ${p.colores.pie};
      --flyweb-tinta: #000;
    ` : css`
      --flyweb-texto: var(--override-readability-color, #fff);
      --flyweb-pie: var(--override-readability-color, rgba(255, 255, 255, 0.6));
      --flyweb-tinta: var(--override-readability-color, #fff);
    `}
  }
`

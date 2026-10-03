// Copyright (c) 2026 lamosquita. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

// FlyWeb: first run with the human's design (softmac
// FlyWeb/docs/prototipo-ntp/firstboot.html), keeping Brave's steps: our
// photograph behind, D-DIN in black, and the welcome card in translucent white.

import { createGlobalStyle } from 'styled-components'

import dDinRegular from '../../../flyweb_ntp/resources/fuentes/D-DIN.woff2'
import dDinBold from '../../../flyweb_ntp/resources/fuentes/D-DIN-Bold.woff2'
import fondoBoot from '../../../flyweb_ntp/resources/fondos/fondo-boot.jpg'

export { fondoBoot }

export const EstiloFlyWeb = createGlobalStyle`
  @font-face {
    font-family: 'D-DIN';
    src: url(${dDinRegular}) format('woff2');
    font-weight: 400;
    font-display: block;
  }
  @font-face {
    font-family: 'D-DIN';
    src: url(${dDinBold}) format('woff2');
    font-weight: 700;
    font-display: block;
  }

  /* Each step's root sets white text in Brave's heading font. */
  .content-box > * {
    color: #000 !important;
    font-family: 'D-DIN', sans-serif !important;
    -webkit-font-smoothing: antialiased;
  }

  /* The welcome card: translucent white, slightly blurred. */
  .view-backdrop {
    background: rgba(255, 255, 255, 0.62) !important;
    backdrop-filter: blur(6px) !important;
    border-radius: 14px !important;
  }
`

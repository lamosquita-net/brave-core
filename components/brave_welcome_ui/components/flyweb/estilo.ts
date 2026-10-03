// Copyright (c) 2026 lamosquita. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

// FlyWeb: first run with the human's design (softmac
// FlyWeb/docs/prototipo-ntp/firstboot.html), keeping Brave's steps: our
// photograph behind, D-DIN in black, and the welcome card in translucent white.
//
// The text colour, the typeface and the card live in the components' own
// styles (background/style.ts, welcome/style.ts): a styled-components global
// style never took effect on the built page. The fonts load through the
// FontFace API.

import * as React from 'react'
import { loadTimeData } from '$web-common/loadTimeData'

import dDinRegular from '../../../flyweb_ntp/resources/fuentes/D-DIN.woff2'
import dDinBold from '../../../flyweb_ntp/resources/fuentes/D-DIN-Bold.woff2'
import fondoBoot from '../../../flyweb_ntp/resources/fondos/fondo-boot.jpg'

export { fondoBoot }

export const familia = "'D-DIN', sans-serif"

// Buttons in the colour of this channel's app icon (FlyWeb/branding): purple
// with white text for the development builds, orange with black text for the
// rest. Black on that purple would not be readable (contrast 2.7:1).
// flywebDesarrollo: brave_welcome_ui.cc.
const desarrollo = loadTimeData.getBoolean('flywebDesarrollo')
export const boton = desarrollo
  ? { fondo: 'rgb(149, 27, 129)', encima: 'rgb(125, 20, 108)', texto: '#fff' }
  : { fondo: 'rgb(255, 153, 0)', encima: 'rgb(235, 138, 0)', texto: '#000' }

let fuentesCargadas = false
function cargarFuentes () {
  if (fuentesCargadas) return
  fuentesCargadas = true
  for (const [url, weight] of [[dDinRegular, '400'], [dDinBold, '700']]) {
    const f = new FontFace('D-DIN', `url("${url}") format("woff2")`, { weight, display: 'block' })
    // FontFaceSet is set-like; TypeScript 4.9's lib.dom lacks its add().
    ;(document.fonts as unknown as Set<FontFace>).add(f)
    f.load().catch(() => console.error('FlyWeb: no se pudo cargar D-DIN', url))
  }
}

export function EstiloFlyWeb () {
  React.useEffect(cargarFuentes, [])
  return null
}

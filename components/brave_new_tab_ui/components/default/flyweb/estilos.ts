// Copyright (c) 2026 lamosquita. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

// FlyWeb: D-DIN typeface and the colours that depend on the background.
// On one of our backgrounds the labels and the credit take that background's
// colours and the clock, "customize" and the bottom icons are black. On any
// other background (the user's own image or a colour) Brave's readability
// colour is kept.
//
// Applied from script, not with a styled-components global style: on the
// built page that global style never took effect (the variables were not
// defined and everything fell back to the page's grey #3b3b3b), while the
// same code works in a plain page. The fonts load through the FontFace API
// and the colours are set as custom properties on <html>, which the styled
// components read with var().

import * as React from 'react'

import { fuentes } from './recursos'

export const familia = "'D-DIN', sans-serif"

let fuentesCargadas = false
export function cargarFuentes () {
  if (fuentesCargadas) return
  fuentesCargadas = true
  for (const [url, weight] of [[fuentes.regular, '400'], [fuentes.bold, '700']]) {
    const f = new FontFace('D-DIN', `url("${url}") format("woff2")`, { weight, display: 'block' })
    // FontFaceSet is set-like; TypeScript 4.9's lib.dom lacks its add().
    ;(document.fonts as unknown as Set<FontFace>).add(f)
    f.load().catch(() => console.error('FlyWeb: no se pudo cargar D-DIN', url))
  }
}

export function EstiloFlyWeb ({ colores }: { colores?: NewTab.FlyWebColores }) {
  React.useEffect(cargarFuentes, [])
  React.useEffect(() => {
    const s = document.documentElement.style
    s.setProperty('--flyweb-texto', colores ? colores.texto : 'var(--override-readability-color, #fff)')
    s.setProperty('--flyweb-pie', colores ? colores.pie : 'var(--override-readability-color, rgba(255, 255, 255, 0.6))')
    s.setProperty('--flyweb-tinta', colores ? '#000' : 'var(--override-readability-color, #fff)')
  }, [colores?.texto, colores?.pie])
  return null
}

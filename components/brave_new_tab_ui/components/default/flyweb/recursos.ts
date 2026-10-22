// Copyright (c) 2026 lamosquita. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

// FlyWeb: resources of the new tab page (components/flyweb_ntp/resources).
// Behaviour reference: softmac FlyWeb/docs/prototipo-ntp/newtab.html.

import dDinRegular from '../../../../flyweb_ntp/resources/fuentes/D-DIN.woff2'
import dDinBold from '../../../../flyweb_ntp/resources/fuentes/D-DIN-Bold.woff2'
import fondo1 from '../../../../flyweb_ntp/resources/fondos/fondo-1.jpg'
import fondo2 from '../../../../flyweb_ntp/resources/fondos/fondo-2.jpg'
import fondo3 from '../../../../flyweb_ntp/resources/fondos/fondo-3.jpg'
import fondo4 from '../../../../flyweb_ntp/resources/fondos/fondo-4.jpg'
import fondo5 from '../../../../flyweb_ntp/resources/fondos/fondo-5.jpg'
import fondo6 from '../../../../flyweb_ntp/resources/fondos/fondo-6.jpg'
import fondo7 from '../../../../flyweb_ntp/resources/fondos/fondo-7.jpg'

export const fuentes = { regular: dDinRegular, bold: dDinBold }

// Project colours, unchanged (one per Shields counter).
export const colores = {
  naranja: 'rgb(255, 153, 0)', // the icon's
  rojo: 'rgb(210, 10, 17)', // lamosquita
  morado: 'rgb(149, 27, 129)' // development
}

export const credito = '© fotografía @lamosquita'

// One background per mockup, with the colour of the texts that lie on it
// (labels of the counters and top sites) and of the photo credit. The clock,
// "customize" and the bottom icons are always black on these backgrounds.
export const fondos: NewTab.BraveBackground[] = [
  { imagen: fondo1, texto: '#000', pie: '#000' },
  { imagen: fondo2, texto: '#000', pie: '#000' },
  { imagen: fondo3, texto: '#fff', pie: '#fff' },
  { imagen: fondo4, texto: '#000', pie: '#fff' },
  { imagen: fondo5, texto: '#000', pie: '#000' },
  { imagen: fondo6, texto: '#fff', pie: '#fff' },
  { imagen: fondo7, texto: '#000', pie: '#000' }
].map(f => ({
  type: 'brave' as const,
  wallpaperImageUrl: f.imagen,
  author: '@lamosquita',
  flyweb: { texto: f.texto, pie: f.pie }
}))

export function esFondoFlyWeb (
  fondo?: NewTab.BackgroundWallpaper
): fondo is NewTab.BraveBackground & { flyweb: NewTab.FlyWebColores } {
  return fondo?.type === 'brave' && !!fondo.flyweb
}

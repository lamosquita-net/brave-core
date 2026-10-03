// Copyright (c) 2026 lamosquita. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

// FlyWeb: turns our own fixed fly SVG (moscaSvg.ts) into React elements.
// WebUI pages enforce Trusted Types, so `innerHTML` / `dangerouslySetInnerHTML`
// with a plain string throws ("This document requires 'TrustedHTML'
// assignment") and the whole page fails to mount. Only what our SVGs use:
// <svg>, <style>, <g>, <path>, <rect>, <circle>, <ellipse>, with their
// attributes; `class` becomes `className`.

import * as React from 'react'

const ETIQUETA = /<(\/?)([a-zA-Z]+)([^>]*?)(\/?)>|([^<]+)/g
const ATRIBUTO = /([a-zA-Z_:][-a-zA-Z0-9_:.]*)="([^"]*)"/g
const VALIDAS = new Set(['svg', 'style', 'g', 'path', 'rect', 'circle', 'ellipse'])

interface Nodo { etiqueta: string, atributos: { [k: string]: string }, hijos: Array<Nodo | string> }

function analizar (svg: string): Nodo {
  const raiz: Nodo = { etiqueta: '', atributos: {}, hijos: [] }
  const pila: Nodo[] = [raiz]
  let m: RegExpExecArray | null
  ETIQUETA.lastIndex = 0
  while ((m = ETIQUETA.exec(svg)) !== null) {
    const actual = pila[pila.length - 1]
    if (m[5] !== undefined) {
      if (actual.etiqueta === 'style') actual.hijos.push(m[5])
      continue
    }
    const [, cierre, etiqueta, resto, sola] = m
    if (cierre) { pila.pop(); continue }
    const atributos: { [k: string]: string } = {}
    let a: RegExpExecArray | null
    ATRIBUTO.lastIndex = 0
    while ((a = ATRIBUTO.exec(resto)) !== null) atributos[a[1]] = a[2]
    const nodo: Nodo = { etiqueta, atributos, hijos: [] }
    actual.hijos.push(nodo)
    if (!sola) pila.push(nodo)
  }
  return raiz.hijos[0] as Nodo
}

function aReact (n: Nodo | string, clave: number): React.ReactNode {
  if (typeof n === 'string') return n
  if (!VALIDAS.has(n.etiqueta)) return null
  const props: { [k: string]: string | number | object } = { key: clave }
  // The drawing relies on the default black fill; WebUI pages set `fill` on
  // every <svg> (currentColor: grey on the new tab, white on the welcome page).
  if (n.etiqueta === 'svg') props.style = { fill: '#000' }
  for (const [k, v] of Object.entries(n.atributos)) {
    if (k === 'class') props.className = v
    else if (k === 'xmlns' || k.startsWith('data-')) continue
    else props[k] = v
  }
  return React.createElement(n.etiqueta, props, ...n.hijos.map(aReact))
}

export default function svgEnReact (svg: string): React.ReactNode {
  return aReact(analizar(svg), 0)
}

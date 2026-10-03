// Copyright (c) 2026 lamosquita. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

// FlyWeb: the fly that flies over the new tab page. Port of the prototype
// (softmac FlyWeb/docs/prototipo-ntp/newtab.html), reviewed with the human on
// the MacPro6,1. A requestAnimationFrame loop that only changes `transform`.
//
// No `will-change` on the fly: on the 6,1 (FirePro D500, Mojave) with the
// display in scaled mode its own compositing layer draws a flickering line
// across the whole screen (softmac docs/TAREAS.md F5.3).

import * as React from 'react'
import styled from 'styled-components'

import moscaSvg from './moscaSvg'
import svgEnReact from './svgEnReact'

// Built once: React elements, not an HTML string (Trusted Types, see svgEnReact.tsx).
const dibujo = svgEnReact(moscaSvg)

// What to touch to tune the flight.
const AJUSTES = {
  velocidad: 105, // px/s in flight at 1280 px wide (scales with the window)
  variacion: 0.18, // each leg between 18 % slower and 18 % faster
  ondulacion: 10, // px up and down over the route while flying
  temblor: 2.5, // quick tremble while flying
  posado: [2.5, 7], // seconds on each perch (random between both)
  saltarse: 0.2, // probability of flying past a perch
  giroPosado: [35, 150], // on landing it turns this many degrees, either side
  rumbo: -53, // where the drawing faces (0 = right; head up and right)
  calma: { velocidad: 0.35, amplitud: 0.3 } // with "reduce motion"
}

// Closed route through these points (0..1 of the window). A lying, uneven
// figure eight (human's mockup, 03/10) that avoids the counters, top sites,
// clock and footer; it crosses itself and stops at the crossing on only one of
// the two passes. `posa` marks a perch.
const RUTA = [
  { x: 0.539, y: 0.625 }, { x: 0.508, y: 0.500 }, { x: 0.453, y: 0.412 },
  { x: 0.352, y: 0.375 }, { x: 0.234, y: 0.369 }, { x: 0.133, y: 0.381, posa: true },
  { x: 0.086, y: 0.438 }, { x: 0.047, y: 0.537 }, { x: 0.035, y: 0.650 },
  { x: 0.055, y: 0.775 }, { x: 0.109, y: 0.875 }, { x: 0.195, y: 0.925 },
  { x: 0.297, y: 0.931 }, { x: 0.406, y: 0.900 }, { x: 0.484, y: 0.838 },
  { x: 0.531, y: 0.750 }, { x: 0.539, y: 0.562 }, { x: 0.547, y: 0.412 },
  { x: 0.578, y: 0.312 }, { x: 0.648, y: 0.237 }, { x: 0.734, y: 0.215 },
  { x: 0.812, y: 0.231 }, { x: 0.852, y: 0.275 }, { x: 0.859, y: 0.331 },
  { x: 0.845, y: 0.412, posa: true }, { x: 0.867, y: 0.481 }, { x: 0.906, y: 0.562 },
  { x: 0.928, y: 0.675 }, { x: 0.914, y: 0.775 }, { x: 0.859, y: 0.850 },
  { x: 0.781, y: 0.887 }, { x: 0.711, y: 0.894 }, { x: 0.648, y: 0.850 },
  { x: 0.602, y: 0.775 }, { x: 0.566, y: 0.700, posa: true }
]

const TAMANO = 76

const Contenedor = styled('div')`
  position: fixed;
  left: 0;
  top: 0;
  width: ${TAMANO}px;
  height: ${TAMANO}px;
  /* "Flies are annoying": it may cross the icons, but clicks go through. */
  pointer-events: none;
  z-index: 3;

  svg {
    width: 100%;
    height: 100%;
    overflow: visible;
  }
  .ala {
    transition: opacity .25s;
  }
  &.vuela .ala {
    animation: flyweb-aleteo .09s steps(2, jump-none) infinite alternate;
  }
  @keyframes flyweb-aleteo {
    from { opacity: .4; }
    to { opacity: .12; }
  }
`

interface Punto { x: number, y: number }
interface Muestra extends Punto { d: number }

// Point of a Catmull-Rom segment.
function punto (p0: Punto, p1: Punto, p2: Punto, p3: Punto, t: number): Punto {
  const t2 = t * t
  const t3 = t2 * t
  const c = (a: number, b: number, c_: number, d: number) =>
    0.5 * (2 * b + (-a + c_) * t + (2 * a - 5 * b + 4 * c_ - d) * t2 + (-a + 3 * b - 3 * c_ + d) * t3)
  return { x: c(p0.x, p1.x, p2.x, p3.x), y: c(p0.y, p1.y, p2.y, p3.y) }
}

// The flight state lives outside React: it changes every frame and only
// drives the element's transform.
function volarSobre (mosca: HTMLDivElement): () => void {
  const reducir = window.matchMedia('(prefers-reduced-motion: reduce)').matches
  const CALMA = reducir ? AJUSTES.calma.velocidad : 1
  const AMPLIA = reducir ? AJUSTES.calma.amplitud : 1

  let tabla: Muestra[] = []
  let largo = 0
  let posaderos: number[] = []

  // Closed curve through RUTA, sampled and measured so that the fly keeps a
  // constant speed along it.
  function medir () {
    const W = innerWidth
    const H = innerHeight
    const n = RUTA.length
    const PASOS = 60
    const P = RUTA.map(r => ({ x: r.x * W, y: r.y * H }))
    tabla = []
    posaderos = []
    largo = 0
    let prev: Punto | null = null
    for (let i = 0; i < n; i++) {
      if (RUTA[i].posa) posaderos.push(largo)
      for (let k = 0; k < PASOS; k++) {
        const q = punto(P[(i - 1 + n) % n], P[i], P[(i + 1) % n], P[(i + 2) % n], k / PASOS)
        if (prev) largo += Math.hypot(q.x - prev.x, q.y - prev.y)
        tabla.push({ d: largo, x: q.x, y: q.y })
        prev = q
      }
    }
    if (prev) largo += Math.hypot(tabla[0].x - prev.x, tabla[0].y - prev.y)
  }

  // Point of the route at distance d.
  function en (d: number): Punto {
    d = ((d % largo) + largo) % largo
    let lo = 0
    let hi = tabla.length - 1
    while (lo < hi) {
      const mid = (lo + hi + 1) >> 1
      if (tabla[mid].d <= d) lo = mid
      else hi = mid - 1
    }
    const a = tabla[lo]
    const b = tabla[(lo + 1) % tabla.length]
    const tramo = (b.d > a.d ? b.d : largo) - a.d
    const f = tramo ? (d - a.d) / tramo : 0
    return { x: a.x + (b.x - a.x) * f, y: a.y + (b.y - a.y) * f }
  }

  let enderezaDesde = -9
  let giroIni = 0
  let giroFin = 0
  let girandoDesde = 0
  let d = 0
  let destino = 0
  let desde = 0
  let inicioTramo = 0
  let duracion = 1
  let posadoHasta = 0
  let volando = false
  let angulo = 0

  function siguientePosadero (): number {
    // The next perch ahead; sometimes it skips it and goes to the following.
    const orden = posaderos
      .map(p => { const x = p - (d % largo); return x <= 1 ? x + largo : x })
      .sort((a, b) => a - b)
    let salto = orden[0]
    if (orden.length > 1 && Math.random() < AJUSTES.saltarse) salto = orden[1]
    return salto
  }

  function despegar (ahora: number) {
    const tramo = siguientePosadero()
    const escala = innerWidth / 1280
    const vel = AJUSTES.velocidad * escala * CALMA * (1 + (Math.random() * 2 - 1) * AJUSTES.variacion)
    angulo += giroFin
    giroFin = 0
    enderezaDesde = ahora
    desde = d
    destino = d + tramo
    inicioTramo = ahora
    duracion = tramo / vel
    volando = true
    mosca.classList.add('vuela')
  }

  function posar (ahora: number) {
    volando = false
    d = destino
    const g = AJUSTES.giroPosado
    const lado = Math.random() < 0.5 ? -1 : 1
    giroIni = 0
    giroFin = lado * (g[0] + Math.random() * (g[1] - g[0])) * AMPLIA
    girandoDesde = ahora
    const r = AJUSTES.posado
    posadoHasta = ahora + (r[0] + Math.random() * (r[1] - r[0])) / CALMA
    mosca.classList.remove('vuela')
  }

  let arranque: number | null = null
  let previa = 0
  let peticion = 0

  function volar (marca: number) {
    if (arranque === null) {
      arranque = previa = marca
      d = posaderos[Math.floor(Math.random() * posaderos.length)]
      destino = d
      posar(0)
      posadoHasta = 1.5
    }
    const hueco = marca - previa
    if (hueco > 800) arranque += hueco - 16 // hidden tab: no jumps
    previa = marca
    const t = (marca - arranque) / 1000

    let x: number
    let y: number
    let giro: number
    if (volando) {
      const f = Math.min(1, (t - inicioTramo) / duracion)
      const s = f * f * (3 - 2 * f) // smooth start and stop
      d = desde + (destino - desde) * s
      const p = en(d)
      const q = en(d + 30)
      const sube = Math.sin(f * Math.PI) * Math.sin(t * 2.1) * AJUSTES.ondulacion * AMPLIA
      x = p.x + Math.sin(t * 23) * AJUSTES.temblor * AMPLIA
      y = p.y + sube + Math.cos(t * 19) * AJUSTES.temblor * AMPLIA
      const rumboVuelo = Math.atan2(q.y - p.y, q.x - p.x) * 180 / Math.PI
      const fe = Math.min(1, (t - enderezaDesde) / 0.4) // straightens in 0.4 s on take-off
      const dif = ((rumboVuelo - angulo + 540) % 360) - 180 // the short way round
      angulo = fe < 1 ? angulo + dif * fe : rumboVuelo
      giro = angulo - AJUSTES.rumbo
      if (f >= 1) posar(t)
    } else {
      // Perched: turns in 0.6 s, slowing down, then sways a little.
      const p2 = en(d)
      x = p2.x
      y = p2.y
      const fg = Math.min(1, (t - girandoDesde) / 0.6)
      const eg = 1 - Math.pow(1 - fg, 3)
      giro = angulo - AJUSTES.rumbo + giroIni + (giroFin - giroIni) * eg +
        Math.sin(t * 1.3) * 6 * AMPLIA + (Math.sin(t * 7) > 0.97 ? 4 : 0)
      if (t > posadoHasta) despegar(t)
    }
    const medio = TAMANO / 2
    mosca.style.transform = `translate(${x - medio}px, ${y - medio}px) rotate(${giro}deg)`
    peticion = requestAnimationFrame(volar)
  }

  let espera = 0
  const alCambiarTamano = () => {
    clearTimeout(espera)
    espera = window.setTimeout(medir, 150)
  }
  addEventListener('resize', alCambiarTamano)
  medir()
  peticion = requestAnimationFrame(volar)

  return () => {
    cancelAnimationFrame(peticion)
    clearTimeout(espera)
    removeEventListener('resize', alCambiarTamano)
  }
}

export default function Mosca () {
  const ref = React.useRef<HTMLDivElement>(null)
  React.useEffect(() => {
    if (!ref.current) return
    return volarSobre(ref.current)
  }, [])
  return (
    <Contenedor
      ref={ref}
      aria-hidden='true'
    >
      {dibujo}
    </Contenedor>
  )
}

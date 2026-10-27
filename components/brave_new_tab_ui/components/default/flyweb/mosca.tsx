// Copyright (c) 2026 lamosquita. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

// FlyWeb: the fly that flies over the new tab page. Port of the prototype
// (softmac FlyWeb/docs/prototipo-ntp/newtab.html), reviewed with the human on
// the MacPro6,1.
//
// The flight runs on the compositor (softmac FM.43): each leg, a flight
// between two perches or a stay on one, is worked out once with the formulas
// of the requestAnimationFrame loop of 1.15, sampled at its 60 Hz, and handed
// over as a Web Animation of `transform`; the wings beat with an `opacity`
// animation of their own layer. The main thread only wakes up between legs.
// Paused while the page is hidden; with "reduce motion" the fly stays still
// on a perch (softmac FlyWeb/docs/disenos.md §3).
//
// No `will-change` on the fly: on the 6,1 (FirePro D500, Mojave) with the
// display in scaled mode its own compositing layer drew a flickering line
// across the whole screen in 1.x (softmac docs/TAREAS.md F5.3). The animations
// give it a layer too; if the line comes back, the 1.15 loop is in git.

import * as React from 'react'
import styled from 'styled-components'

import moscaSvg from './moscaSvg'
import svgEnReact from './svgEnReact'

// The drawing in three layers, in its own order: what is under the wings, the
// wings (class "ala") and what is over them. Built once: React elements, not
// an HTML string (Trusted Types, see svgEnReact.tsx).
const capas = (() => {
  const [, apertura, trazos, cierre] = /^(<svg[^>]*>(?:<style>[^<]*<\/style>)?)([\s\S]*)(<\/svg>)$/.exec(moscaSvg)!
  const lista = trazos.match(/<path[^>]*\/>/g) ?? []
  const esAla = (t: string) => / ala"/.test(t)
  const primera = lista.findIndex(esAla)
  const ultima = lista.length - 1 - [...lista].reverse().findIndex(esAla)
  const svg = (partes: string[]) => svgEnReact(apertura + partes.join('') + cierre)
  return {
    debajo: svg(lista.slice(0, primera)),
    alas: svg(lista.slice(primera, ultima + 1)),
    encima: svg(lista.slice(ultima + 1))
  }
})()

// What to touch to tune the flight.
const AJUSTES = {
  velocidad: 105, // px/s in flight at 1280 px wide (scales with the window)
  variacion: 0.18, // each leg between 18 % slower and 18 % faster
  ondulacion: 10, // px up and down over the route while flying
  temblor: 2.5, // quick tremble while flying
  posado: [2.5, 7], // seconds on each perch (random between both)
  saltarse: 0.2, // probability of flying past a perch
  giroPosado: [35, 150], // on landing it turns this many degrees, either side
  rumbo: -53 // where the drawing faces (0 = right; head up and right)
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

// Seconds between two samples of a leg: the frame rate the 1.15 loop ran at.
const PASO = 1 / 60
// Opacity of the wings layer at the low point of a beat: .12 over their .4.
const ALETEO = 0.3

const Contenedor = styled('div')`
  position: fixed;
  left: 0;
  top: 0;
  width: ${TAMANO}px;
  height: ${TAMANO}px;
  /* "Flies are annoying": it may cross the icons, but clicks go through. */
  pointer-events: none;
  z-index: 3;

  svg, .alas {
    position: absolute;
    left: 0;
    top: 0;
    width: 100%;
    height: 100%;
  }
  svg {
    overflow: visible;
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

// The flight state lives outside React and only drives the animations.
function volarSobre (mosca: HTMLDivElement, alas: HTMLDivElement, quieta: boolean): () => void {
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

  // Everything a leg starts from, so that a resize can work it out again.
  const e = {
    enderezaDesde: -9,
    giroIni: 0,
    giroFin: 0,
    girandoDesde: 0,
    d: 0,
    destino: 0,
    desde: 0,
    inicioTramo: 0,
    duracion: 1,
    posadoHasta: 0,
    volando: false,
    angulo: 0,
    giroPrevio: null as number | null
  }

  function siguientePosadero (): number {
    // The next perch ahead; sometimes it skips it and goes to the following.
    const orden = posaderos
      .map(p => { const x = p - (e.d % largo); return x <= 1 ? x + largo : x })
      .sort((a, b) => a - b)
    let salto = orden[0]
    if (orden.length > 1 && Math.random() < AJUSTES.saltarse) salto = orden[1]
    return salto
  }

  let aleteo: Animation | null = null

  function despegar (ahora: number) {
    const tramo = siguientePosadero()
    const escala = innerWidth / 1280
    const vel = AJUSTES.velocidad * escala * (1 + (Math.random() * 2 - 1) * AJUSTES.variacion)
    e.angulo += e.giroFin
    e.giroFin = 0
    e.enderezaDesde = ahora
    e.desde = e.d
    e.destino = e.d + tramo
    e.inicioTramo = ahora
    e.duracion = tramo / vel
    e.volando = true
    aleteo = alas.animate([{ opacity: 1 }, { opacity: ALETEO }], {
      duration: 90,
      easing: 'steps(2, jump-none)',
      iterations: Infinity,
      direction: 'alternate'
    })
  }

  function posar (ahora: number) {
    e.volando = false
    e.d = e.destino
    const g = AJUSTES.giroPosado
    const lado = Math.random() < 0.5 ? -1 : 1
    e.giroIni = 0
    e.giroFin = lado * (g[0] + Math.random() * (g[1] - g[0]))
    e.girandoDesde = ahora
    const r = AJUSTES.posado
    e.posadoHasta = ahora + r[0] + Math.random() * (r[1] - r[0])
    if (aleteo) {
      // The wings settle in .25 s from wherever the beat left them.
      const ahoraAlas = Number(getComputedStyle(alas).opacity)
      aleteo.cancel()
      aleteo = null
      if (ahoraAlas < 1) alas.animate([{ opacity: ahoraAlas }, { opacity: 1 }], { duration: 250, easing: 'ease' })
    }
  }

  // Pose at time t while flying (it also turns the fly towards its course).
  function enVuelo (t: number): Punto & { giro: number } {
    const f = Math.min(1, (t - e.inicioTramo) / e.duracion)
    const s = f * f * (3 - 2 * f) // smooth start and stop
    e.d = e.desde + (e.destino - e.desde) * s
    const p = en(e.d)
    const q = en(e.d + 30)
    const sube = Math.sin(f * Math.PI) * Math.sin(t * 2.1) * AJUSTES.ondulacion
    const rumboVuelo = Math.atan2(q.y - p.y, q.x - p.x) * 180 / Math.PI
    const fe = Math.min(1, (t - e.enderezaDesde) / 0.4) // straightens in 0.4 s on take-off
    const dif = ((rumboVuelo - e.angulo + 540) % 360) - 180 // the short way round
    e.angulo = fe < 1 ? e.angulo + dif * fe : rumboVuelo
    return {
      x: p.x + Math.sin(t * 23) * AJUSTES.temblor,
      y: p.y + sube + Math.cos(t * 19) * AJUSTES.temblor,
      giro: e.angulo - AJUSTES.rumbo
    }
  }

  // Pose at time t while perched: turns in 0.6 s, slowing down, then sways a little.
  function posada (t: number): Punto & { giro: number } {
    const fg = Math.min(1, (t - e.girandoDesde) / 0.6)
    const eg = 1 - Math.pow(1 - fg, 3)
    return {
      ...en(e.d),
      giro: e.angulo - AJUSTES.rumbo + e.giroIni + (e.giroFin - e.giroIni) * eg +
        Math.sin(t * 1.3) * 6 + (Math.sin(t * 7) > 0.97 ? 4 : 0)
    }
  }

  const medio = TAMANO / 2
  function transformar (p: Punto & { giro: number }): string {
    // Keyframes are interpolated: the angle must not jump a whole turn.
    let giro = p.giro
    if (e.giroPrevio !== null) giro -= 360 * Math.round((giro - e.giroPrevio) / 360)
    e.giroPrevio = giro
    return `translate(${p.x - medio}px, ${p.y - medio}px) rotate(${giro}deg)`
  }

  // Keyframes of the current leg, from t0 to its end.
  function marcos (t0: number): Keyframe[] {
    const duracion = e.volando ? e.duracion : e.posadoHasta - t0
    const n = Math.max(2, Math.ceil(duracion / PASO) + 1)
    const lista: Keyframe[] = []
    for (let i = 0; i < n; i++) {
      const t = t0 + duracion * i / (n - 1)
      lista.push({ transform: transformar(e.volando ? enVuelo(t) : posada(t)) })
    }
    return lista
  }

  let vuelo: Animation | null = null
  let inicio = { ...e }
  let t0 = 0

  function seguir (desde: number) {
    inicio = { ...e }
    t0 = desde
    const duracion = e.volando ? e.duracion : e.posadoHasta - desde
    const anterior = vuelo
    const actual = mosca.animate(marcos(desde), { duration: duracion * 1000, fill: 'forwards' })
    anterior?.cancel()
    if (document.hidden) {
      actual.pause()
      aleteo?.pause()
    }
    actual.onfinish = () => {
      const fin = desde + duracion
      if (e.volando) posar(fin)
      else despegar(fin)
      seguir(fin)
    }
    vuelo = actual
  }

  // Still on a perch, with "reduce motion".
  let sitio = 0
  let giroQuieta = 0
  function colocarQuieta () {
    const p = en(posaderos[sitio])
    mosca.style.transform = `translate(${p.x - medio}px, ${p.y - medio}px) rotate(${giroQuieta}deg)`
  }

  let espera = 0
  const alCambiarTamano = () => {
    clearTimeout(espera)
    espera = window.setTimeout(() => {
      const antes = largo
      medir()
      if (quieta) {
        colocarQuieta()
        return
      }
      if (!vuelo) return
      // Same leg and timing over the new route: distances scale with it.
      const k = largo / antes
      inicio = { ...inicio, d: inicio.d * k, desde: inicio.desde * k, destino: inicio.destino * k }
      Object.assign(e, inicio)
      ;(vuelo.effect as KeyframeEffect).setKeyframes(marcos(t0))
    }, 150)
  }

  // Hidden tab or window: nothing runs (the compositor stops drawing too).
  const alCambiarVisibilidad = () => {
    for (const a of [vuelo, aleteo]) {
      if (!a) continue
      if (document.hidden && a.playState === 'running') a.pause()
      else if (!document.hidden && a.playState === 'paused') a.play()
    }
  }

  addEventListener('resize', alCambiarTamano)
  medir()
  if (quieta) {
    const g = AJUSTES.giroPosado
    sitio = Math.floor(Math.random() * posaderos.length)
    giroQuieta = -AJUSTES.rumbo + (Math.random() < 0.5 ? -1 : 1) * (g[0] + Math.random() * (g[1] - g[0]))
    colocarQuieta()
  } else {
    document.addEventListener('visibilitychange', alCambiarVisibilidad)
    e.d = posaderos[Math.floor(Math.random() * posaderos.length)]
    e.destino = e.d
    posar(0)
    e.posadoHasta = 1.5
    seguir(0)
  }

  return () => {
    if (vuelo) {
      vuelo.onfinish = null
      vuelo.cancel()
    }
    aleteo?.cancel()
    clearTimeout(espera)
    removeEventListener('resize', alCambiarTamano)
    document.removeEventListener('visibilitychange', alCambiarVisibilidad)
    mosca.style.transform = ''
  }
}

export default function Mosca () {
  const ref = React.useRef<HTMLDivElement>(null)
  const refAlas = React.useRef<HTMLDivElement>(null)
  React.useEffect(() => {
    const mosca = ref.current
    const alas = refAlas.current
    if (!mosca || !alas) return
    const reducir = window.matchMedia('(prefers-reduced-motion: reduce)')
    let parar = volarSobre(mosca, alas, reducir.matches)
    const alCambiar = () => {
      parar()
      parar = volarSobre(mosca, alas, reducir.matches)
    }
    reducir.addEventListener('change', alCambiar)
    return () => {
      reducir.removeEventListener('change', alCambiar)
      parar()
    }
  }, [])
  return (
    <Contenedor
      ref={ref}
      aria-hidden='true'
    >
      {capas.debajo}
      <div className='alas' ref={refAlas}>{capas.alas}</div>
      {capas.encima}
    </Contenedor>
  )
}

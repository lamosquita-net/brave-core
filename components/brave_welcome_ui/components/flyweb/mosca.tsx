// Copyright (c) 2026 lamosquita. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

// FlyWeb: the first-run fly, peeking over the top edge of the welcome card
// with "a light buzz" that does not move it from its place. CSS only, and
// only transform and opacity: the body trembles half a pixel (0.11 s) and
// sways ±1.2° (2.9 s), the wings flicker (0.07 s) and the 7 arcs come and go
// staggered (1.1 s); rhythms that do not coincide so it does not look like a
// loop. With "reduce motion", only the slow sway.

import * as React from 'react'
import styled from 'styled-components'

import moscaSvg from './moscaSvg'
import svgEnReact from './svgEnReact'

// Built once: React elements, not an HTML string (Trusted Types, see svgEnReact.tsx).
const dibujo = svgEnReact(moscaSvg)

const Contenedor = styled('div')`
  width: 116px;

  svg {
    display: block;
    width: 100%;
    overflow: visible;
  }
  .cuerpo {
    transform-box: fill-box;
    transform-origin: 50% 50%;
    animation: flyweb-tiembla .11s linear infinite alternate,
               flyweb-balancea 2.9s ease-in-out infinite;
  }
  .ala {
    animation: flyweb-aleteo .07s steps(2, jump-none) infinite alternate;
  }
  .arco {
    animation: flyweb-zumba 1.1s ease-in-out infinite;
  }
  .arco:nth-of-type(2n) { animation-delay: -.35s; }
  .arco:nth-of-type(3n) { animation-delay: -.7s; }

  @keyframes flyweb-tiembla {
    from { translate: -.5px .3px; }
    to { translate: .5px -.4px; }
  }
  @keyframes flyweb-balancea {
    0%, 100% { rotate: -1.2deg; }
    50% { rotate: 1.2deg; }
  }
  @keyframes flyweb-aleteo {
    from { opacity: .5; }
    to { opacity: .2; }
  }
  @keyframes flyweb-zumba {
    0%, 100% { opacity: .15; }
    50% { opacity: 1; }
  }

  @media (prefers-reduced-motion: reduce) {
    .cuerpo { animation: flyweb-balancea 6s ease-in-out infinite; }
    .ala, .arco { animation: none; }
  }
`

export default function MoscaZumbido () {
  return (
    <Contenedor
      aria-hidden='true'
    >
      {dibujo}
    </Contenedor>
  )
}

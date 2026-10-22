// Copyright (c) 2022 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// you can obtain one at https://mozilla.org/MPL/2.0/.

import * as React from 'react'
import * as S from './style'
import classnames from '$web-common/classnames'

// FlyWeb: our first-run photograph (components/flyweb_ntp/resources) instead
// of Brave's sky, hills, stars and pyramid. It is still, so there are no
// background scenes: the steps already call scenes?.play() optionally.
import { fondoBoot } from '../flyweb/estilo'

interface BackgroundProps {
  children?: JSX.Element
  static: boolean
  onLoad?: () => void
}

function Background (props: BackgroundProps) {
  const [hasLoaded, setHasLoaded] = React.useState(false)

  const handleImgLoad = () => {
    setHasLoaded(true)
    props.onLoad?.()
  }

  return (
    <S.Box>
      <div className="content-box">
        {props.children}
      </div>
      <img
        // We animate the background image via CSS only.
        className={classnames({
          'background-img': true,
          'is-visible': hasLoaded
        })}
        src={fondoBoot}
        onLoad={handleImgLoad}
      />
    </S.Box>
  )
}

export default Background

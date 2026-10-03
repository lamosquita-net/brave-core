// Copyright (c) 2022 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// you can obtain one at https://mozilla.org/MPL/2.0/.

import * as React from 'react'
import * as S from './style'

import classnames from '$web-common/classnames'
import { getLocale } from '$web-common/locale'
import Button from '$web-components/button'

import { WelcomeBrowserProxyImpl, DefaultBrowserBrowserProxyImpl } from '../../api/welcome_browser_proxy'
import WebAnimationPlayer from '../../api/web_animation_player'

import DataContext from '../../state/context'
import { ViewType } from '../../state/component_types'
import { shouldPlayAnimations } from '../../state/hooks'

import MoscaZumbido from '../flyweb/mosca'

function Welcome () {
  const { setViewType, scenes, browserProfiles } = React.useContext(DataContext)
  const ref = React.useRef<HTMLDivElement>(null)

  const goToImportThemeOrBrowser = () => {
    if (!browserProfiles || browserProfiles.length === 0) {
      setViewType(ViewType.ImportSelectTheme)
      return
    }
    setViewType(ViewType.ImportSelectBrowser)
  }

  const handleSetAsDefaultBrowser = () => {
    WelcomeBrowserProxyImpl.getInstance().recordP3A({ currentScreen: ViewType.DefaultBrowser, isFinished: false, isSkipped: false })
    DefaultBrowserBrowserProxyImpl.getInstance().setAsDefaultBrowser()
    goToImportThemeOrBrowser()
    scenes?.s1.play()
  }

  const handleSkip = () => {
    WelcomeBrowserProxyImpl.getInstance().recordP3A({ currentScreen: ViewType.DefaultBrowser, isFinished: false, isSkipped: true })
    goToImportThemeOrBrowser()
    scenes?.s1.play()
  }

  React.useEffect(() => {
    if (!ref.current) return
    if (!shouldPlayAnimations) return

    const backdropEl = ref.current.querySelector('.view-backdrop')
    const contentEl = ref.current.querySelector('.view-content')

    const s1 = new WebAnimationPlayer()

    // FlyWeb: the fly stays where it is (it buzzes on its own, CSS only).
    s1.to(backdropEl, { scale: 1, opacity: 1 }, { duration: 250, delay: 200, easing: 'ease-out' })
      .to(contentEl, { transform: 'translateY(0px)', opacity: 1 }, { duration: 250, delay: 200, easing: 'ease-out' })

    s1.play()

    return () => {
      s1.finish()
      s1.cancel()
    }
  }, [])

  return (
    <S.Box ref={shouldPlayAnimations ? ref : null}>
      <div className="view-logo-box">
        <MoscaZumbido />
      </div>
      <div className={classnames({ 'view-content': true, 'initial': shouldPlayAnimations })}>
        <div className="view-header-box">
          <div className="view-details">
            <h1 className="view-title">{getLocale('braveWelcomeTitle')}</h1>
            <p className="view-desc">{getLocale('braveWelcomeDesc')}</p>
          </div>
        </div>
        <S.ActionBox>
          <Button
            isPrimary={true}
            onClick={handleSetAsDefaultBrowser}
            scale="jumbo"
          >
            {getLocale('braveWelcomeSetDefaultButtonLabel')}
          </Button>
          <Button
            isTertiary={true}
            onClick={handleSkip}
            scale="jumbo"
          >
            {getLocale('braveWelcomeSkipButtonLabel')}
          </Button>
        </S.ActionBox>
      </div>
      <div className={classnames({ 'view-backdrop': true, 'initial': shouldPlayAnimations })} />
    </S.Box>
  )
}

export default Welcome

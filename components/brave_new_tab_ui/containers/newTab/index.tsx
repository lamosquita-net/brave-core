// Copyright (c) 2020 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

import * as React from 'react'

// Components
import getNTPBrowserAPI from '../../api/background'
import { addNewTopSite, editTopSite } from '../../api/topSites'
import {
  Clock, EditTopSite, OverrideReadabilityColor, SearchPromotion
} from '../../components/default'
import FooterInfo from '../../components/default/footer/footer'
import * as Page from '../../components/default/page'
import TopSitesGrid from './gridSites'
import SiteRemovalNotification from './notification'
import Stats from './stats'
import Mosca from '../../components/default/flyweb/mosca'
import { EstiloFlyWeb } from '../../components/default/flyweb/estilos'
import { esFondoFlyWeb } from '../../components/default/flyweb/recursos'

// Helpers
import { getLocale } from '$web-common/locale'
import { loadTimeData } from '$web-common/loadTimeData'
import isReadableOnBackground from '../../helpers/colorUtil'

// Types
import { NewTabActions } from '../../constants/new_tab_types'

// NTP features
import { MAX_GRID_SIZE } from '../../constants/new_tab_ui'
import Settings, { TabType as SettingsTabType } from './settings'

import GridWidget from './gridWidget'

import { setIconBasePath } from '@brave/leo/react/icon'
setIconBasePath('chrome://resources/brave-icons')

import * as style from './style'

const SearchPlaceholder = React.lazy(() => import('../../components/search/SearchPlaceholder'))

interface Props {
  newTabData: NewTab.State
  gridSitesData: NewTab.GridSitesState
  actions: NewTabActions
  saveShowBackgroundImage: (value: boolean) => void
  chooseNewCustomBackgroundImage: () => void
  setCustomImageBackground: (selectedBackground: string) => void
  removeCustomImageBackground: (background: string) => void
  setBraveBackground: (selectedBackground: string) => void
  setColorBackground: (color: string, useRandomColor: boolean) => void
}

interface State {
  showSettingsMenu: boolean
  showEditTopSite: boolean
  targetTopSiteForEditing?: NewTab.Site
  backgroundHasLoaded: boolean
  activeSettingsTab: SettingsTabType | null
  showSearchPromotion: boolean
  forceToHideWidget: boolean
}

function GetBackgroundImageSrc (props: Props) {
  if (!props.newTabData.showBackgroundImage) {
    return undefined
  }

  if (props.newTabData.backgroundWallpaper?.type === 'image' ||
      props.newTabData.backgroundWallpaper?.type === 'brave') {
    return props.newTabData.backgroundWallpaper.wallpaperImageUrl
  }

  return undefined
}

function GetShouldShowSearchPromotion (props: Props, showSearchPromotion: boolean) {
  return props.newTabData.searchPromotionEnabled && showSearchPromotion
}

function GetShouldForceToHideWidget (props: Props, showSearchPromotion: boolean) {
  if (!GetShouldShowSearchPromotion(props, showSearchPromotion)) {
    return false
  }

  // Avoid promotion popup and other widgets overlap with narrow window.
  return window.innerWidth < 1000
}

class NewTabPage extends React.Component<Props, State> {
  state: State = {
    showSettingsMenu: false,
    showEditTopSite: false,
    backgroundHasLoaded: false,
    activeSettingsTab: null,
    showSearchPromotion: false,
    forceToHideWidget: false
  }

  imgCache: HTMLImageElement
  imageSource?: string = undefined

  componentDidMount () {
    // if a notification is open at component mounting time, close it
    this.props.actions.showTilesRemovedNotice(false)
    this.imageSource = GetBackgroundImageSrc(this.props)
    this.trackCachedImage()
    this.checkShouldOpenSettings()
    const searchPromotionEnabled = this.props.newTabData.searchPromotionEnabled
    this.setState({
      showSearchPromotion: searchPromotionEnabled,
      forceToHideWidget: GetShouldForceToHideWidget(this.props, searchPromotionEnabled)
    })
    window.addEventListener('resize', this.handleResize)
    window.navigation.addEventListener('currententrychange', this.checkShouldOpenSettings)
  }

  componentWillUnmount () {
    window.removeEventListener('resize', this.handleResize)
    window.navigation.removeEventListener('currententrychange', this.checkShouldOpenSettings)
  }

  componentDidUpdate (prevProps: Props) {
    const oldImageSource = GetBackgroundImageSrc(prevProps)
    const newImageSource = GetBackgroundImageSrc(this.props)
    this.imageSource = newImageSource
    if (newImageSource && oldImageSource !== newImageSource) {
      this.trackCachedImage()
    }
    if (oldImageSource &&
      !newImageSource) {
      // reset loaded state
      console.debug('reset image loaded state due to removing image source')
      this.setState({ backgroundHasLoaded: false })
    }
  }

  shouldOverrideReadabilityColor (newTabData: NewTab.State) {
    return newTabData.backgroundWallpaper?.type === 'color' && !isReadableOnBackground(newTabData.backgroundWallpaper)
  }

  handleResize = () => {
    this.setState({
      forceToHideWidget: GetShouldForceToHideWidget(this.props, this.state.showSearchPromotion)
    })
  }

  trackCachedImage () {
    console.debug('trackCachedImage')
    if (this.state.backgroundHasLoaded) {
      console.debug('Resetting to new image')
      this.setState({ backgroundHasLoaded: false })
    }
    if (this.imageSource) {
      const imgCache = new Image()
      // Store Image in class so it doesn't go out of scope
      this.imgCache = imgCache
      imgCache.src = this.imageSource
      console.debug('image start loading...')
      imgCache.addEventListener('load', () => {
        console.debug('image loaded')
        this.setState({
          backgroundHasLoaded: true
        })
      })
      imgCache.addEventListener('error', (e) => {
        console.debug('image error', e)
      })
    }
  }

  checkShouldOpenSettings = () => {
    const params = window.location.search
    const urlParams = new URLSearchParams(params)
    const openSettings = urlParams.get('openSettings') || this.props.newTabData.forceSettingsTab

    if (openSettings) {
      let activeSettingsTab: SettingsTabType | null = null
      const activeSettingsTabRaw = typeof openSettings === 'string'
        ? openSettings
        : this.props.newTabData.forceSettingsTab || null
      if (activeSettingsTabRaw) {
        const allSettingsTabTypes = [...Object.keys(SettingsTabType)]
        if (allSettingsTabTypes.includes(activeSettingsTabRaw)) {
          activeSettingsTab = SettingsTabType[activeSettingsTabRaw]
        }
      }
      this.setState({ showSettingsMenu: true, activeSettingsTab })
      // Remove settings param so menu doesn't persist on reload
      window.history.pushState(null, '', '/')
    }
  }

  toggleShowBackgroundImage = () => {
    this.props.saveShowBackgroundImage(
      !this.props.newTabData.showBackgroundImage
    )
  }

  toggleShowTopSites = () => {
    const { showTopSites, customLinksEnabled } = this.props.newTabData
    this.props.actions.setMostVisitedSettings(!showTopSites, customLinksEnabled)
  }

  toggleCustomLinksEnabled = () => {
    const { showTopSites, customLinksEnabled } = this.props.newTabData
    this.props.actions.setMostVisitedSettings(showTopSites, !customLinksEnabled)
  }

  setMostVisitedSettings = (showTopSites: boolean, customLinksEnabled: boolean) => {
    this.props.actions.setMostVisitedSettings(showTopSites, customLinksEnabled)
  }

  closeSettings = () => {
    this.setState({
      showSettingsMenu: false,
      activeSettingsTab: null
    })
  }

  showEditTopSite = (targetTopSiteForEditing?: NewTab.Site) => {
    this.setState({
      showEditTopSite: true,
      targetTopSiteForEditing
    })
  }

  closeEditTopSite = () => {
    this.setState({
      showEditTopSite: false
    })
  }

  closeSearchPromotion = () => {
    this.setState({
      showSearchPromotion: false,
      forceToHideWidget: false
    })
  }

  saveNewTopSite = (title: string, url: string, newUrl: string) => {
    if (url) {
      editTopSite(title, url, newUrl === url ? '' : newUrl)
    } else {
      addNewTopSite(title, newUrl)
    }
    this.closeEditTopSite()
  }

  openSettings = (activeTab?: SettingsTabType) => {
    this.setState({
      showSettingsMenu: !this.state.showSettingsMenu,
      activeSettingsTab: activeTab || null
    })
  }

  renderSearchPromotion () {
    if (!GetShouldShowSearchPromotion(this.props, this.state.showSearchPromotion)) {
      return null
    }

    const onClose = () => { this.closeSearchPromotion() }
    const onDismiss = () => { getNTPBrowserAPI().pageHandler.dismissBraveSearchPromotion() }
    const onTryBraveSearch = (input: string, openNewTab: boolean) => { getNTPBrowserAPI().pageHandler.tryBraveSearchPromotion(input, openNewTab) }

    return (
      <SearchPromotion textDirection={this.props.newTabData.textDirection} onTryBraveSearch={onTryBraveSearch} onClose={onClose} onDismiss={onDismiss} />
    )
  }

  render () {
    const { newTabData, gridSitesData, actions } = this.props
    const { showSettingsMenu, showEditTopSite, targetTopSiteForEditing, forceToHideWidget } = this.state

    if (!newTabData) {
      return null
    }

    const hasImage = this.imageSource !== undefined

    const hasWallpaperInfo = newTabData.backgroundWallpaper?.type === 'brave'
    // FlyWeb: on one of our backgrounds, no dark veil and our text colours.
    const fondo = newTabData.backgroundWallpaper
    const coloresFlyWeb = hasImage && esFondoFlyWeb(fondo) ? fondo.flyweb : undefined
    const colorForBackground = newTabData.backgroundWallpaper?.type === 'color' ? newTabData.backgroundWallpaper.wallpaperColor : undefined

    const showAddNewSiteMenuItem = newTabData.customLinksNum < MAX_GRID_SIZE

    let { showTopSites, showStats, showClock } = newTabData
    // In favorites mode, add site tile is visible by default if there is no
    // item. In frecency, top sites widget is hidden with empty tiles.
    if (showTopSites && !newTabData.customLinksEnabled) {
      showTopSites = this.props.gridSitesData.gridSites.length !== 0
    }

    if (forceToHideWidget) {
      showTopSites = false
      showStats = false
      showClock = false
    }

    return (
      <Page.App
        dataIsReady={newTabData.initialDataLoaded}
        hasImage={hasImage}
        imageSrc={this.imageSource}
        imageHasLoaded={this.state.backgroundHasLoaded}
        colorForBackground={colorForBackground}
        sinVelo={!!coloresFlyWeb}>
        <OverrideReadabilityColor override={ this.shouldOverrideReadabilityColor(this.props.newTabData) } />
        <EstiloFlyWeb colores={coloresFlyWeb} fotoPropia={hasImage && !coloresFlyWeb} />
        <Mosca />
        <Page.Page
            hasImage={hasImage}
            imageSrc={this.imageSource}
            imageHasLoaded={this.state.backgroundHasLoaded}
            showClock={showClock}
            showStats={showStats}
            colorForBackground={colorForBackground}
            sinVelo={!!coloresFlyWeb}
            showTopSites={showTopSites}
        >
          {this.renderSearchPromotion()}
          <GridWidget
            pref='showStats'
            container={Page.GridItemStats}
            paddingType={'right'}
            widgetTitle={getLocale('statsTitle')}
            textDirection={newTabData.textDirection}
            menuPosition={'right'}>
            <Stats stats={newTabData.stats}/>
          </GridWidget>
          <GridWidget
            pref='showClock'
            container={Page.GridItemClock}
            paddingType='right'
            widgetTitle={getLocale('clockTitle')}
            textDirection={newTabData.textDirection}
            menuPosition='left'>
            <Clock />
          </GridWidget>
          {
            showTopSites &&
              <Page.GridItemTopSites>
                <TopSitesGrid
                  actions={actions}
                  paddingType={'none'}
                  customLinksEnabled={newTabData.customLinksEnabled}
                  onShowEditTopSite={this.showEditTopSite}
                  widgetTitle={getLocale('topSitesTitle')}
                  gridSites={gridSitesData.gridSites}
                  menuPosition={'right'}
                  hideWidget={this.toggleShowTopSites}
                  onAddSite={showAddNewSiteMenuItem ? this.showEditTopSite : undefined}
                  onToggleCustomLinksEnabled={this.toggleCustomLinksEnabled}
                  textDirection={newTabData.textDirection}
                />
              </Page.GridItemTopSites>
            }
            {
              gridSitesData.shouldShowSiteRemovedNotification
                ? (
                  <Page.GridItemNotification>
                    <SiteRemovalNotification actions={actions} showRestoreAll={!newTabData.customLinksEnabled} />
                  </Page.GridItemNotification>
                ) : null
            }
            <Page.Footer>
              <Page.FooterContent>
                <FooterInfo
                  textDirection={newTabData.textDirection}
                  backgroundImageInfo={newTabData.backgroundWallpaper}
                  showPhotoInfo={hasWallpaperInfo && newTabData.showBackgroundImage}
                  onClickSettings={this.openSettings}
                />
              </Page.FooterContent>
            </Page.Footer>
              <Page.GridItemPageFooter>
                {loadTimeData.getBoolean('featureFlagSearchWidget')
                  && <React.Suspense fallback={null}>
                    <SearchPlaceholder />
                  </React.Suspense>}
              </Page.GridItemPageFooter>
          </Page.Page>
        <Settings
          textDirection={newTabData.textDirection}
          showSettingsMenu={showSettingsMenu}
          featureCustomBackgroundEnabled={newTabData.featureCustomBackgroundEnabled}
          onClose={this.closeSettings}
          setActiveTab={this.state.activeSettingsTab || undefined}
          toggleShowBackgroundImage={this.toggleShowBackgroundImage}
          toggleShowTopSites={this.toggleShowTopSites}
          setMostVisitedSettings={this.setMostVisitedSettings}
          chooseNewCustomImageBackground={this.props.chooseNewCustomBackgroundImage}
          setCustomImageBackground={this.props.setCustomImageBackground}
          removeCustomImageBackground={this.props.removeCustomImageBackground}
          setBraveBackground={this.props.setBraveBackground}
          setColorBackground={this.props.setColorBackground}
          showBackgroundImage={newTabData.showBackgroundImage}
          showTopSites={newTabData.showTopSites}
          customLinksEnabled={newTabData.customLinksEnabled}
          newTabData={this.props.newTabData}
        />
        {
          showEditTopSite
            ? <EditTopSite
              targetTopSiteForEditing={targetTopSiteForEditing}
              textDirection={newTabData.textDirection}
              onClose={this.closeEditTopSite}
              onSave={this.saveNewTopSite}
            /> : null
        }
      </Page.App>
    )
  }
}

export default NewTabPage

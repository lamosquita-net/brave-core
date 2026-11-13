/* Copyright (c) 2026 The FlyWeb Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

// FlyWeb 3.2: the account part of «Cuenta y sincronización»
// (FlyWeb/docs/sync-cuenta.md §5.1, §6, §8, §9, §11). It paints the nine
// states of §9 from flyweb_sync::AccountService::GetStatus(), each with its
// text and its buttons, and none without a way out: every call has a time
// limit here too, and its error comes with «Reintentar» or a way back.
//
// Passwords: plain <input type="password"> outside any <form> (the password
// manager must not offer to save the account password, P23), cleared as soon
// as they are sent; only the minimum length blocks the button.

import 'chrome://resources/cr_elements/cr_button/cr_button.js';
import 'chrome://resources/cr_elements/cr_checkbox/cr_checkbox.js';
import 'chrome://resources/cr_elements/cr_dialog/cr_dialog.js';
import 'chrome://resources/cr_elements/cr_shared_style.css.js';
import '../settings_shared.css.js';
import './brave_sync_code_dialog.js';

import {I18nMixin, I18nMixinInterface} from 'chrome://resources/cr_elements/i18n_mixin.js'
import {WebUiListenerMixin, WebUiListenerMixinInterface} from 'chrome://resources/cr_elements/web_ui_listener_mixin.js';
import {PolymerElement} from 'chrome://resources/polymer/v3_0/polymer/polymer_bundled.min.js';

import {BaseMixin} from '../base_mixin.js'

import {BraveSyncBrowserProxy} from './brave_sync_browser_proxy.js';
import type {FlyWebAccountDevice, FlyWebAccountFlowKind, FlyWebAccountMessage, FlyWebAccountResult, FlyWebAccountStatus, FlyWebAccountWarning} from './brave_sync_browser_proxy.js';
import {getTemplate} from './flyweb_sync_account.html.js'

// The service gives up at 30 s per request; a call with a chain proof makes
// two requests and may stretch the password twice first. Past this, the page
// stops waiting and offers a way out (§9, P10).
const kCallTimeoutMs = 90 * 1000

// §11: the strength indicator waits for the typing to stop.
const kStrengthDelayMs = 300

// What the page shows, from the status and the local step.
type Screen = 'none' | 'loading' | 'recovery' | 'flow' | 'no-account' |
    'create' | 'login' | 'words' | 'forgot' | 'start-over' |
    'mailbox-delete' | 'synced' | 'change-email' | 'change-key' | 'keys' |
    'rejoin' | 'deleted' | 'key-changed'

// The steps the user opens with a button; the rest come from the status.
type LocalView = '' | 'create' | 'login' | 'words' | 'forgot' |
    'start-over' | 'mailbox-delete' | 'change-email' | 'change-key' | 'rejoin'

type ConfirmAction = 'remove' | 'leave' | 'delete'

interface ConfirmDialog {
  action: ConfirmAction
  title: string
  body: string
  body2: string
  button: string
  deviceId: string
}

// Flows whose last step sets a new password.
const kNewPasswordFlows: FlyWebAccountFlowKind[] =
    ['crear', 'contrasena', 'empezar-de-cero']
// Flows whose last step names this Mac.
const kDeviceNameFlows: FlyWebAccountFlowKind[] = ['crear', 'entrar']

function countCodePoints(s: string): number {
  return [...s.normalize('NFC')].length
}

// Device list and «último contacto»: a full date.
function formatDate(unixSeconds: number): string {
  if (!unixSeconds) {
    return ''
  }
  return new Date(unixSeconds * 1000).toLocaleDateString(
      undefined, {day: 'numeric', month: 'long', year: 'numeric'})
}

// Dates a few days ahead (deletion, start over): day and month.
function formatShortDate(unixSeconds: number): string {
  if (!unixSeconds) {
    return ''
  }
  return new Date(unixSeconds * 1000).toLocaleDateString(
      undefined, {day: 'numeric', month: 'long'})
}

function formatDateTime(unixSeconds: number): string {
  if (!unixSeconds) {
    return ''
  }
  return new Date(unixSeconds * 1000).toLocaleString(undefined, {
    day: 'numeric',
    month: 'long',
    hour: '2-digit',
    minute: '2-digit',
  })
}

function formatTime(unixSeconds: number): string {
  return new Date(unixSeconds * 1000).toLocaleTimeString(
      undefined, {hour: '2-digit', minute: '2-digit'})
}

function failure(error: string): FlyWebAccountResult {
  return {ok: false, error, until: 0, data: {}}
}

const SettingsFlyWebSyncAccountElementBase =
  I18nMixin(WebUiListenerMixin(BaseMixin(PolymerElement))) as {
    new(): PolymerElement & WebUiListenerMixinInterface & I18nMixinInterface
  }

export class SettingsFlyWebSyncAccountElement
    extends SettingsFlyWebSyncAccountElementBase {
  static get is() {
    return 'settings-flyweb-sync-account'
  }

  static get template() {
    return getTemplate()
  }

  static get properties() {
    return {
      /**
       * The account status, for the subpage (null: no account service in
       * this profile, the page then shows the old setup).
       */
      accountStatus: {
        type: Object,
        value: null,
        notify: true,
      },
      view_: {
        type: String,
        value: '',
      },
      screen_: {
        type: String,
        computed: 'computeScreen_(accountStatus, view_, recoveryWords_)',
      },
      email_: {type: String, value: ''},
      password_: {type: String, value: ''},
      newEmail_: {type: String, value: ''},
      code_: {type: String, value: ''},
      deviceName_: {type: String, value: ''},
      words_: {type: String, value: ''},
      strength_: {type: Number, value: -1},
      pending_: {type: Boolean, value: false},
      pendingPassword_: {type: Boolean, value: false},
      error_: {type: String, value: ''},
      canRetry_: {type: Boolean, value: false},
      info_: {type: String, value: ''},
      recoveryWords_: {type: String, value: ''},
      recoveryMustConfirm_: {type: Boolean, value: false},
      recoveryIsNew_: {type: Boolean, value: false},
      recoveryInfo_: {type: String, value: ''},
      recoverySaved_: {type: Boolean, value: false},
      notMeDone_: {type: Boolean, value: false},
      renamingId_: {type: String, value: ''},
      renameValue_: {type: String, value: ''},
      confirm_: {type: Object, value: null},
      oldVersionCode_: {type: String, value: ''},
      oldVersionCodeDialogType_: {type: String, value: null},
    }
  }

  static get observers() {
    return [
      'onPasswordChanged_(password_)',
    ]
  }

  declare accountStatus: FlyWebAccountStatus | null
  private declare view_: LocalView
  private declare screen_: Screen
  private declare email_: string
  private declare password_: string
  private declare newEmail_: string
  private declare code_: string
  private declare deviceName_: string
  private declare words_: string
  private declare strength_: number
  private declare pending_: boolean
  private declare pendingPassword_: boolean
  private declare error_: string
  private declare canRetry_: boolean
  private declare info_: string
  private declare recoveryWords_: string
  private declare recoveryMustConfirm_: boolean
  private declare recoveryIsNew_: boolean
  private declare recoveryInfo_: string
  private declare recoverySaved_: boolean
  private declare notMeDone_: boolean
  private declare renamingId_: string
  private declare renameValue_: string
  private declare confirm_: ConfirmDialog | null
  private declare oldVersionCode_: string
  private declare oldVersionCodeDialogType_: string | null

  private browserProxy_: BraveSyncBrowserProxy =
      BraveSyncBrowserProxy.getInstance()
  private retry_: (() => void) | null = null
  // After the recovery key is confirmed, end the setup as the old page did
  // (the subpage marks it successful and goes back to the section).
  private finishAfterWords_ = false
  private strengthTimer_ = 0
  private strengthSeq_ = 0

  override connectedCallback() {
    super.connectedCallback()
    this.addWebUiListener(
        'flyweb-account-status-changed',
        (status: FlyWebAccountStatus) => this.onStatus_(status))
    this.loadStatus_()
  }

  // The subpage was opened: §11 polls the account events «al abrir la
  // página», and the device list is loaded again.
  refresh() {
    this.loadStatus_().then(() => {
      if (!this.accountStatus) {
        return
      }
      this.browserProxy_.flywebAccountCall('FlyWebAccountRefresh')
      if (this.accountStatus.hasAccount && this.accountStatus.hasChain) {
        this.browserProxy_.flywebAccountCall('FlyWebAccountListDevices')
      }
    })
  }

  private async loadStatus_() {
    let status: FlyWebAccountStatus | null = null
    try {
      status = await this.browserProxy_.getFlyWebAccountStatus()
    } catch (e) {
      status = null
    }
    this.onStatus_(status)
  }

  private onStatus_(status: FlyWebAccountStatus | null) {
    if (!status || typeof status !== 'object' || !status.state) {
      this.accountStatus = null
      return
    }
    const old = this.accountStatus
    this.accountStatus = status
    if (!this.deviceName_ && status.model) {
      this.deviceName_ = status.model
    }
    // A step opened for a state that is gone (another Mac deleted the
    // account, the keychain changed...) is closed.
    if (old && old.state !== status.state && !this.pending_) {
      this.view_ = ''
    }
  }

  // --- what is shown ---

  protected computeScreen_(
      status: FlyWebAccountStatus | null, view: LocalView,
      recoveryWords: string): Screen {
    if (!status) {
      return 'none'
    }
    if (recoveryWords) {
      return 'recovery'
    }
    if (status.flow) {
      return 'flow'
    }
    if (view) {
      return view
    }
    switch (status.state) {
      case 'sin-cuenta':
        return 'no-account'
      case 'sincronizado':
      case 'borrado-programado':
        return 'synced'
      case 'claves-ilegibles':
        return 'keys'
      case 'cuenta-borrada':
        return 'deleted'
      case 'clave-cambiada':
        return 'key-changed'
      default:
        return 'loading'
    }
  }

  protected is_(screen: Screen, expected: string): boolean {
    return screen === expected
  }

  protected isState_(status: FlyWebAccountStatus | null, state: string):
      boolean {
    return !!status && status.state === state
  }

  protected hasChain_(status: FlyWebAccountStatus | null): boolean {
    return !!status && status.hasChain
  }

  protected showWarnings_(
      status: FlyWebAccountStatus | null, screen: Screen): boolean {
    return !!status && !!status.warnings && status.warnings.length > 0 &&
        screen !== 'recovery'
  }

  protected showOffline_(status: FlyWebAccountStatus | null): boolean {
    return !!status && status.offline
  }

  protected offlineText_(status: FlyWebAccountStatus | null): string {
    if (!status || !status.lastContact) {
      return this.i18n('flywebAccountErrorRed')
    }
    return this.i18n(
        'flywebAccountOffline', formatDateTime(status.lastContact))
  }

  protected showLastError_(
      status: FlyWebAccountStatus | null, error: string,
      pending: boolean): boolean {
    return !!status && !!status.lastError && !status.offline && !error &&
        !pending
  }

  protected lastErrorText_(status: FlyWebAccountStatus | null): string {
    if (!status || !status.lastError) {
      return ''
    }
    return this.i18n(
        'flywebAccountLastError', this.errorText_(failure(status.lastError)))
  }

  protected busy_(status: FlyWebAccountStatus | null, pending: boolean):
      boolean {
    return pending || (!!status && status.busy)
  }

  protected pendingText_(pendingPassword: boolean): string {
    return this.i18n(pendingPassword ? 'flywebAccountCheckingPassword' :
                                       'flywebAccountWorking')
  }

  protected accountEmailText_(status: FlyWebAccountStatus | null): string {
    if (!status || !status.email) {
      return this.i18n('flywebAccountEmailNotKnown')
    }
    return this.i18n('flywebAccountAccountEmail', status.email)
  }

  protected deletionText_(status: FlyWebAccountStatus | null): string {
    return this.i18n(
        'flywebAccountDeletionScheduled',
        formatShortDate(status ? status.deletionDate : 0))
  }

  protected devices_(status: FlyWebAccountStatus | null):
      FlyWebAccountDevice[] {
    return status && status.devices ? status.devices : []
  }

  protected hasDevices_(status: FlyWebAccountStatus | null): boolean {
    return this.devices_(status).length > 0
  }

  protected deviceAddedText_(device: FlyWebAccountDevice): string {
    return this.i18n('flywebAccountDeviceAdded', formatDate(device.alta))
  }

  protected deviceLastUsedText_(device: FlyWebAccountDevice): string {
    return this.i18n(
        'flywebAccountDeviceLastUsed', formatDate(device.ultimo_uso))
  }

  protected isRenaming_(device: FlyWebAccountDevice, renamingId: string):
      boolean {
    return !!renamingId && device.id === renamingId
  }

  protected warningText_(w: FlyWebAccountWarning): string {
    const name = w.mac || w.model
    switch (w.type) {
      case 'mac-anadido':
        return this.i18n('flywebAccountWarningMacAdded', name)
      case 'contrasena-acertada':
        return this.i18n('flywebAccountWarningPasswordGuessed')
      case 'contrasena-fallos':
        return this.i18n(
            'flywebAccountWarningPasswordFailures', String(w.count))
      case 'contrasena-cambiada':
        return this.i18n('flywebAccountWarningPasswordChanged')
      case 'email-cambiado':
        return this.i18n('flywebAccountWarningEmailChanged')
      case 'cadena-cambiada':
        return this.i18n('flywebAccountWarningChainChanged')
      case 'cadena-ligada':
        return this.i18n('flywebAccountWarningChainLinked')
      case 'aparato-quitado':
        return this.i18n('flywebAccountWarningDeviceRemoved', name)
      case 'no-he-sido-yo':
        return this.i18n('flywebAccountWarningNotMe')
      case 'borrado-pedido':
        return this.i18n(
            'flywebAccountWarningDeletionRequested', formatShortDate(w.when))
      default:
        return this.i18n('flywebAccountWarningDeletionCancelled')
    }
  }

  protected warningDate_(w: FlyWebAccountWarning): string {
    return formatDateTime(w.date)
  }

  // --- the email code step (§9 «falta el código del email») ---

  protected flowTitle_(status: FlyWebAccountStatus | null): string {
    switch (status && status.flow ? status.flow.kind : '') {
      case 'crear':
        return this.i18n('flywebAccountCreate')
      case 'entrar':
        return this.i18n('flywebAccountLogin')
      case 'contrasena':
        return this.i18n('flywebAccountChangePassword')
      case 'email':
        return this.i18n('flywebAccountChangeEmail')
      case 'borrar-por-email':
        return this.i18n('flywebAccountMailboxDelete')
      case 'empezar-de-cero':
        return this.i18n('flywebAccountStartOver')
    }
    return ''
  }

  protected flowDesc_(status: FlyWebAccountStatus | null): string {
    switch (status && status.flow ? status.flow.kind : '') {
      case 'crear':
        return this.i18n('flywebAccountFlowCreateDesc')
      case 'entrar':
        return this.i18n('flywebAccountFlowLoginDesc')
      case 'contrasena':
        return this.i18n('flywebAccountFlowPasswordDesc')
      case 'email':
        return this.i18n('flywebAccountFlowEmailDesc')
      case 'borrar-por-email':
        return this.i18n('flywebAccountFlowMailboxDeleteDesc')
      case 'empezar-de-cero':
        return this.i18n('flywebAccountFlowStartOverDesc')
    }
    return ''
  }

  protected codeSentText_(status: FlyWebAccountStatus | null): string {
    return this.i18n(
        'flywebAccountCodeSent', status && status.flow ? status.flow.email : '')
  }

  protected flowNeedsPassword_(status: FlyWebAccountStatus | null): boolean {
    return !!status && !!status.flow &&
        kNewPasswordFlows.includes(status.flow.kind)
  }

  protected flowNeedsName_(status: FlyWebAccountStatus | null): boolean {
    return !!status && !!status.flow &&
        kDeviceNameFlows.includes(status.flow.kind)
  }

  protected passwordHint_(status: FlyWebAccountStatus | null): string {
    return this.i18n(
        'flywebAccountPasswordHint', String(this.minLength_(status)))
  }

  protected showStrength_(strength: number): boolean {
    return strength >= 0
  }

  protected strengthText_(strength: number): string {
    const score = Math.max(0, Math.min(4, Math.floor(strength)))
    return this.i18n('flywebAccountStrength' + score)
  }

  protected strengthClass_(strength: number): string {
    return 'strength strength-' + Math.max(0, Math.min(4, strength))
  }

  protected canConfirmFlow_(
      status: FlyWebAccountStatus | null, code: string, password: string,
      pending: boolean): boolean {
    if (!status || !status.flow || pending || status.busy || !code.trim()) {
      return false
    }
    if (this.flowNeedsPassword_(status)) {
      return this.longEnough_(password, status)
    }
    return true
  }

  // --- forms ---

  protected canSendEmail_(
      email: string, pending: boolean, status: FlyWebAccountStatus | null):
      boolean {
    return !!email.trim() && !this.busy_(status, pending)
  }

  protected canLogin_(
      email: string, password: string, pending: boolean,
      status: FlyWebAccountStatus | null): boolean {
    return !!email.trim() && !!password && !this.busy_(status, pending)
  }

  protected canChangeEmail_(
      newEmail: string, password: string, pending: boolean,
      status: FlyWebAccountStatus | null): boolean {
    return !!newEmail.trim() && !!password && !this.busy_(status, pending)
  }

  protected canChangeKey_(
      password: string, pending: boolean,
      status: FlyWebAccountStatus | null): boolean {
    return !!password && !this.busy_(status, pending)
  }

  protected canJoin_(
      words: string, pending: boolean, status: FlyWebAccountStatus | null):
      boolean {
    return this.wordCount_(words) >= 24 && !this.busy_(status, pending)
  }

  protected wordCount_(words: string): number {
    const trimmed = words.trim()
    return trimmed ? trimmed.split(/\s+/).length : 0
  }

  protected wordCountText_(words: string): string {
    return this.i18n('braveSyncWordCount', String(this.wordCount_(words)))
  }

  private minLength_(status: FlyWebAccountStatus | null): number {
    return status && status.minPasswordLength ? status.minPasswordLength : 10
  }

  private longEnough_(password: string, status: FlyWebAccountStatus | null):
      boolean {
    return countCodePoints(password) >= this.minLength_(status)
  }

  // Strength of a new password (§4: an indicator; only the length blocks).
  protected onPasswordChanged_(password: string) {
    window.clearTimeout(this.strengthTimer_)
    this.strengthSeq_++
    this.strength_ = -1
    if (!password || !this.flowNeedsPassword_(this.accountStatus)) {
      return
    }
    const seq = this.strengthSeq_
    this.strengthTimer_ = window.setTimeout(async () => {
      let score = -1
      try {
        score = await this.browserProxy_.getFlyWebPasswordStrength(password)
      } catch (e) {
        score = -1
      }
      if (seq === this.strengthSeq_ && typeof score === 'number') {
        this.strength_ = score
      }
    }, kStrengthDelayMs)
  }

  // --- errors (all translatable; §9: the reason and a way out) ---

  private errorText_(result: FlyWebAccountResult): string {
    switch (result.error) {
      case 'red':
        return this.i18n('flywebAccountErrorRed')
      case 'servidor':
        return this.i18n('flywebAccountErrorServidor')
      case 'credenciales':
        return this.i18n('flywebAccountErrorCredenciales')
      case 'espera':
        return this.i18n(
            'flywebAccountErrorEspera',
            formatTime(result.until || Date.now() / 1000))
      case 'cupo':
        return this.i18n('flywebAccountErrorCupo')
      case 'ocupado':
        return this.i18n('flywebAccountErrorOcupado')
      case 'envoltura':
        return this.i18n('flywebAccountErrorEnvoltura')
      case 'contrasena-corta':
        return this.i18n(
            'flywebAccountErrorContrasenaCorta',
            String(this.minLength_(this.accountStatus)))
      case 'email':
        // The address typed, or (changing the password on a Mac that
        // joined with the words) the account email this Mac does not know.
        return this.accountStatus && this.accountStatus.hasAccount &&
                !this.accountStatus.email ?
            this.i18n('flywebAccountErrorEmailUnknown') :
            this.i18n('flywebAccountErrorEmail')
      case 'palabras':
        return this.i18n('flywebAccountErrorPalabras')
      case 'sin-cuenta':
        return this.i18n('flywebAccountErrorSinCuenta')
      case 'cadena-ligada':
        return this.i18n('flywebAccountErrorCadenaLigada')
      case 'cadena-borrada':
        return this.i18n('flywebAccountErrorCadenaBorrada')
      case 'email-ocupado':
        return this.i18n('flywebAccountErrorEmailOcupado')
      case 'programado':
        return this.i18n('flywebAccountErrorProgramado')
      case 'aparato':
        return this.i18n('flywebAccountErrorAparato')
      case 'prueba':
        return this.i18n('flywebAccountErrorPrueba')
      case 'token':
        return this.i18n('flywebAccountErrorToken')
      case 'llavero':
        return this.i18n('flywebAccountErrorLlavero')
      case 'ocupado-local':
        return this.i18n('flywebAccountErrorOcupadoLocal')
      case 'sin-flujo':
        return this.i18n('flywebAccountErrorSinFlujo')
      case 'sin-cadena':
        return this.i18n('flywebAccountErrorSinCadena')
      case 'peticion':
        return this.i18n('flywebAccountErrorPeticion')
      default:
        return this.i18n('flywebAccountErrorInterno')
    }
  }

  // One call at a time, with a time limit; its error goes to the page.
  // `retry` is what «Reintentar» does: null for calls that carry a password
  // or the words, which are not kept (the form is the way to try again).
  private async call_(
      message: FlyWebAccountMessage, args: string[], checksPassword: boolean,
      retry: (() => void) | null): Promise<FlyWebAccountResult> {
    this.error_ = ''
    this.info_ = ''
    this.canRetry_ = false
    this.retry_ = null
    this.pending_ = true
    this.pendingPassword_ = checksPassword
    let timer = 0
    const timeout = new Promise<FlyWebAccountResult>(resolve => {
      timer = window.setTimeout(() => resolve(failure('red')), kCallTimeoutMs)
    })
    let result: FlyWebAccountResult
    try {
      result = await Promise.race(
          [this.browserProxy_.flywebAccountCall(message, ...args), timeout])
    } catch (e) {
      result = failure('interno')
    }
    window.clearTimeout(timer)
    this.pending_ = false
    this.pendingPassword_ = false
    if (!result || typeof result !== 'object') {
      result = failure('interno')
    }
    if (!result.data) {
      result.data = {}
    }
    if (!result.ok) {
      this.error_ = this.errorText_(result)
      this.retry_ = retry
      this.canRetry_ = !!retry
    }
    return result
  }

  protected onRetry_() {
    const retry = this.retry_
    this.retry_ = null
    this.canRetry_ = false
    if (retry) {
      retry()
    }
  }

  private clearForm_() {
    this.password_ = ''
    this.newEmail_ = ''
    this.code_ = ''
    this.words_ = ''
    this.error_ = ''
    this.canRetry_ = false
    this.retry_ = null
  }

  private openView_(view: LocalView) {
    this.clearForm_()
    this.info_ = ''
    this.view_ = view
  }

  protected onBack_() {
    this.openView_('')
  }

  // Ends a join as the old setup did (see brave_sync_subpage.ts).
  private finishSetup_() {
    this.dispatchEvent(new CustomEvent('setup-success'))
  }

  private showRecoveryWords_(
      words: string, mustConfirm: boolean, isNew: boolean, info: string,
      finishAfter: boolean) {
    this.recoverySaved_ = false
    this.recoveryMustConfirm_ = mustConfirm
    this.recoveryIsNew_ = isNew
    this.recoveryInfo_ = info
    this.finishAfterWords_ = finishAfter
    this.recoveryWords_ = words
  }

  // --- without an account (§5.1, §6.4, §6.6, §8) ---

  protected onCreate_() {
    this.openView_('create')
    this.email_ = this.accountStatus ? this.accountStatus.email : ''
  }

  protected onLogin_() {
    this.openView_('login')
    this.email_ = this.accountStatus ? this.accountStatus.email : ''
  }

  protected onHaveRecoveryKey_() {
    this.openView_('words')
  }

  protected onForgot_() {
    this.openView_('forgot')
  }

  protected onStartOver_() {
    this.openView_('start-over')
    this.email_ = this.accountStatus ? this.accountStatus.email : ''
  }

  protected onMailboxDelete_() {
    this.openView_('mailbox-delete')
    this.email_ = this.accountStatus ? this.accountStatus.email : ''
  }

  protected onRejoin_() {
    this.openView_('rejoin')
  }

  protected async onCreateStart_() {
    await this.call_(
        'FlyWebAccountCreateStart', [this.email_.trim()], false,
        () => this.onCreateStart_())
  }

  protected async onLoginStart_() {
    const password = this.password_
    this.password_ = ''
    await this.call_(
        'FlyWebAccountLoginStart', [this.email_.trim(), password], true, null)
  }

  protected async onJoinWithWords_() {
    const words = this.words_.trim().split(/\s+/).join(' ')
    this.words_ = ''
    const r = await this.call_('FlyWebAccountJoinWithWords', [words], false,
                               null)
    if (r.ok) {
      this.view_ = ''
      this.info_ = this.i18n('flywebAccountDone')
      this.finishSetup_()
    }
  }

  protected async onStartOverStart_() {
    await this.call_(
        'FlyWebAccountStartOverStart', [this.email_.trim()], false,
        () => this.onStartOverStart_())
  }

  protected async onMailboxDeleteStart_() {
    await this.call_(
        'FlyWebAccountMailboxDeleteStart', [this.email_.trim()], false,
        () => this.onMailboxDeleteStart_())
  }

  // --- the code from the email ---

  private normalizedCode_(): string {
    return this.code_.replace(/\s+/g, '').toUpperCase()
  }

  protected async onFlowConfirm_() {
    const status = this.accountStatus
    if (!status || !status.flow) {
      return
    }
    const code = this.normalizedCode_()
    const name = (this.deviceName_ || status.model || '').trim()
    const password = this.password_
    this.password_ = ''
    switch (status.flow.kind) {
      case 'crear': {
        const r = await this.call_(
            'FlyWebAccountCreateFinish', [code, password, name], true, null)
        // A new chain (§8): the words come even if joining it failed
        // afterwards; the account exists already.
        if (r.data.recoveryWords) {
          this.code_ = ''
          this.view_ = ''
          this.showRecoveryWords_(
              r.data.recoveryWords, true, false, '', true)
        } else if (r.ok) {
          this.code_ = ''
          this.view_ = ''
          this.info_ = this.i18n('flywebAccountDone')
        }
        return
      }
      case 'entrar': {
        const r = await this.call_(
            'FlyWebAccountLoginFinish', [code, name], false, null)
        if (r.ok) {
          this.code_ = ''
          this.view_ = ''
          this.info_ = this.i18n('flywebAccountDone')
          this.finishSetup_()
        }
        return
      }
      case 'contrasena': {
        const r = await this.call_(
            'FlyWebAccountPasswordFinish', [code, password], true, null)
        if (r.ok) {
          this.code_ = ''
          this.view_ = ''
          this.info_ = this.i18n('flywebAccountDone')
        }
        return
      }
      case 'email': {
        const r = await this.call_(
            'FlyWebAccountEmailFinish', [code], false, null)
        if (r.ok) {
          this.code_ = ''
          this.view_ = ''
          this.info_ = this.i18n('flywebAccountDone')
        }
        return
      }
      case 'borrar-por-email': {
        const r = await this.call_(
            'FlyWebAccountMailboxDeleteFinish', [code], false, null)
        if (r.ok) {
          this.code_ = ''
          this.view_ = ''
          this.info_ = this.i18n(
              'flywebAccountMailboxDeleteDone',
              formatShortDate(r.data.fecha || 0))
        }
        return
      }
      case 'empezar-de-cero': {
        const r = await this.call_(
            'FlyWebAccountStartOverFinish', [code, password], true, null)
        if (r.data.recoveryWords) {
          this.code_ = ''
          this.view_ = ''
          this.showRecoveryWords_(
              r.data.recoveryWords, true, false,
              this.i18n(
                  'flywebAccountStartOverDone',
                  formatShortDate(r.data.fecha || 0)),
              true)
        }
        return
      }
    }
  }

  protected async onResend_() {
    const r = await this.call_(
        'FlyWebAccountResendCode', [], false, () => this.onResend_())
    if (r.ok) {
      this.info_ = this.i18n('flywebAccountDone')
    }
  }

  protected async onFlowCancel_() {
    this.password_ = ''
    this.code_ = ''
    // Cancelling only forgets local state: it does not fail.
    await this.call_('FlyWebAccountCancelFlow', [], false, null)
    this.error_ = ''
  }

  // --- the recovery key (§6.6, §8) ---

  protected async onViewRecovery_() {
    this.error_ = ''
    let words = ''
    try {
      words = await this.browserProxy_.getPureSyncCode()
    } catch (e) {
      words = ''
    }
    if (!words) {
      this.error_ = this.i18n('flywebAccountErrorInterno')
      this.retry_ = () => this.onViewRecovery_()
      this.canRetry_ = true
      return
    }
    this.showRecoveryWords_(words, false, false, '', false)
  }

  protected canCloseRecovery_(mustConfirm: boolean, saved: boolean): boolean {
    return !mustConfirm || saved
  }

  protected onRecoveryDone_() {
    if (this.recoveryMustConfirm_ && !this.recoverySaved_) {
      return
    }
    const finish = this.finishAfterWords_
    this.finishAfterWords_ = false
    this.recoveryWords_ = ''
    this.recoverySaved_ = false
    this.recoveryInfo_ = ''
    if (finish) {
      this.finishSetup_()
    }
  }

  // The 25 words with a date that FlyWeb 2.7 and 3.0 join with, in the old
  // dialog: out of the main view (decision 9), not gone.
  protected async onOldVersionCode_() {
    let code = ''
    try {
      code = await this.browserProxy_.getSyncCode()
    } catch (e) {
      code = ''
    }
    if (!code) {
      this.error_ = this.i18n('flywebAccountErrorInterno')
      return
    }
    this.oldVersionCode_ = code
    this.oldVersionCodeDialogType_ = 'words'
  }

  protected onOldVersionCodeDone_() {
    this.oldVersionCodeDialogType_ = null
    this.oldVersionCode_ = ''
  }

  // --- synced (§6) ---

  protected async onChangePassword_() {
    this.notMeDone_ = false
    this.openView_('')
    await this.call_(
        'FlyWebAccountPasswordStart', [], false,
        () => this.onChangePassword_())
  }

  protected onChangeEmail_() {
    this.notMeDone_ = false
    this.openView_('change-email')
  }

  protected onChangeKey_() {
    this.notMeDone_ = false
    this.openView_('change-key')
  }

  protected async onChangeEmailStart_() {
    const password = this.password_
    this.password_ = ''
    await this.call_(
        'FlyWebAccountEmailStart', [password, this.newEmail_.trim()], true,
        null)
  }

  protected async onChangeKeyConfirm_() {
    const password = this.password_
    this.password_ = ''
    const r =
        await this.call_('FlyWebAccountChainChange', [password], true, null)
    if (r.data.recoveryWords) {
      this.view_ = ''
      this.showRecoveryWords_(r.data.recoveryWords, true, true, '', true)
    }
  }

  protected async onReloadDevices_() {
    await this.call_(
        'FlyWebAccountListDevices', [], false, () => this.onReloadDevices_())
  }

  // The device buttons sit in a dom-if inside the dom-repeat: they carry
  // the device in data attributes instead of relying on e.model.
  private deviceOf_(e: Event): FlyWebAccountDevice | undefined {
    const id = (e.currentTarget as HTMLElement).dataset['id'] || ''
    return this.devices_(this.accountStatus).find(d => d.id === id)
  }

  protected onRenameDevice_(e: Event) {
    const device = this.deviceOf_(e)
    if (!device) {
      return
    }
    this.renamingId_ = device.id
    this.renameValue_ = device.nombre
  }

  protected onRenameCancel_() {
    this.renamingId_ = ''
    this.renameValue_ = ''
  }

  protected canSaveName_(name: string, pending: boolean): boolean {
    return !!name.trim() && !pending
  }

  protected async onRenameSave_() {
    const id = this.renamingId_
    const name = this.renameValue_.trim()
    const r = await this.call_(
        'FlyWebAccountRenameDevice', [id, name], false,
        () => this.onRenameSave_())
    if (r.ok) {
      this.onRenameCancel_()
      this.info_ = this.i18n('flywebAccountDone')
    }
  }

  protected onRemoveDevice_(e: Event) {
    const device = this.deviceOf_(e)
    if (!device) {
      return
    }
    this.confirm_ = {
      action: 'remove',
      title: this.i18n('flywebAccountRemoveFromList'),
      body: this.i18n('flywebAccountRemoveConfirm'),
      body2: '',
      button: this.i18n('flywebAccountRemoveFromList'),
      deviceId: device.id,
    }
  }

  protected onLeave_() {
    this.confirm_ = {
      action: 'leave',
      title: this.i18n('flywebAccountLeave'),
      body: this.i18n('flywebAccountLeaveConfirm'),
      body2: '',
      button: this.i18n('flywebAccountLeave'),
      deviceId: '',
    }
  }

  protected onDeleteEverywhere_() {
    // N = the devices of the account (§9, P8); at least this one.
    const count = Math.max(1, this.devices_(this.accountStatus).length)
    this.confirm_ = {
      action: 'delete',
      title: this.i18n('flywebAccountDeleteEverywhere'),
      body: this.i18n('flywebAccountDeleteConfirm', String(count)),
      body2: this.i18n('flywebAccountDeleteConfirmServer'),
      button: this.i18n('flywebAccountDeleteEverywhere'),
      deviceId: '',
    }
  }

  protected onConfirmCancel_() {
    this.confirm_ = null
  }

  protected async onConfirmAccept_() {
    const confirm = this.confirm_
    this.confirm_ = null
    if (!confirm) {
      return
    }
    switch (confirm.action) {
      case 'remove': {
        const id = confirm.deviceId
        const remove = async () => {
          const r = await this.call_(
              'FlyWebAccountRemoveDevice', [id], false, () => remove())
          if (r.ok) {
            this.info_ = this.i18n('flywebAccountDone')
          }
        }
        await remove()
        return
      }
      case 'leave':
        await this.leave_()
        return
      case 'delete':
        await this.deleteEverywhere_()
        return
    }
  }

  private async leave_() {
    const r = await this.call_(
        'FlyWebAccountLeave', [], false, () => this.leave_())
    if (r.ok) {
      this.openView_('')
      this.info_ = this.i18n('flywebAccountDone')
    }
  }

  private async deleteEverywhere_() {
    const r = await this.call_(
        'FlyWebAccountDeleteEverywhere', [], false,
        () => this.deleteEverywhere_())
    if (r.ok) {
      this.openView_('')
      this.info_ = this.i18n('flywebAccountDone')
    }
  }

  protected async onCancelDeletion_() {
    const r = await this.call_(
        'FlyWebAccountCancelDeletion', [], false,
        () => this.onCancelDeletion_())
    if (r.ok) {
      this.info_ = this.i18n('flywebAccountDone')
    }
  }

  // --- «¿Has sido tú?» (§6.2) ---

  protected async onWasMe_() {
    await this.call_('FlyWebAccountDismissWarnings', [], false, null)
  }

  protected async onNotMe_() {
    const r = await this.call_(
        'FlyWebAccountNotMe', [], false, () => this.onNotMe_())
    if (r.ok) {
      this.notMeDone_ = true
    }
  }

  protected onNotMeClose_() {
    this.notMeDone_ = false
  }

  // --- notices (§6.3, §9) ---

  protected onLoginAgain_() {
    this.onLogin_()
  }

  protected async onUnderstood_() {
    await this.call_('FlyWebAccountDismissNotice', [], false, null)
  }

  // «Sin red» and «cargando»: ask again.
  protected async onRefresh_() {
    await this.loadStatus_()
    if (!this.accountStatus) {
      return
    }
    await this.call_(
        'FlyWebAccountRefresh', [], false, () => this.onRefresh_())
    if (this.accountStatus && this.accountStatus.hasAccount &&
        this.accountStatus.hasChain) {
      this.browserProxy_.flywebAccountCall('FlyWebAccountListDevices')
    }
  }
}

declare global {
  interface HTMLElementTagNameMap {
    'settings-flyweb-sync-account': SettingsFlyWebSyncAccountElement;
  }
}

customElements.define(
  SettingsFlyWebSyncAccountElement.is, SettingsFlyWebSyncAccountElement)

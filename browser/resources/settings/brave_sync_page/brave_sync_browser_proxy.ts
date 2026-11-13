// Copyright (c) 2020 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// you can obtain one at https://mozilla.org/MPL/2.0/.

import {SyncStatus} from '/shared/settings/people_page/sync_browser_proxy.js';
import {sendWithPromise} from 'chrome://resources/js/cr.js';

export type BraveDeviceInfo = {
  name: string
  guid: string
  id: string
  os: string
  type: string
  chromeVersion: string
  lastUpdatedTimestamp: number
  sendTabToSelfReceivingEnabled: boolean
  supportsSelfDelete: boolean
  isCurrentDevice: boolean
  hasSharingInfo: boolean
}

export interface BraveSyncStatus extends SyncStatus {
  hasSyncWordsDecryptionError: boolean
  isOsEncryptionAvailable: boolean
}

// FlyWeb 3.2: «Cuenta y sincronización». The contract with
// BraveSyncHandler (FlyWebAccount* messages) and
// flyweb_sync::AccountService::GetStatus().
export type FlyWebAccountState = 'cargando' | 'sin-cuenta' | 'sincronizado' |
    'borrado-programado' | 'claves-ilegibles' | 'cuenta-borrada' |
    'clave-cambiada'

export type FlyWebAccountFlowKind = 'crear' | 'entrar' | 'contrasena' |
    'email' | 'borrar-por-email' | 'empezar-de-cero'

export interface FlyWebAccountFlow {
  kind: FlyWebAccountFlowKind
  email: string
}

export interface FlyWebAccountWarning {
  n: number
  type: string
  date: number
  mac: string
  model: string
  count: number
  since: number
  when: number
}

export interface FlyWebAccountDevice {
  id: string
  modelo: string
  nombre: string
  alta: number
  ultimo_uso: number
  este: boolean
}

export interface FlyWebAccountStatus {
  state: FlyWebAccountState
  email: string
  hasChain: boolean
  hasAccount: boolean
  deletionDate: number
  offline: boolean
  lastError: string
  lastContact: number
  busy: boolean
  model: string
  minPasswordLength: number
  flow?: FlyWebAccountFlow
  warnings: FlyWebAccountWarning[]
  devices: FlyWebAccountDevice[]
}

export interface FlyWebAccountResultData {
  recoveryWords?: string
  fecha?: number
  aparatos?: FlyWebAccountDevice[]
}

// Every FlyWebAccount* call resolves (never rejects) with this.
export interface FlyWebAccountResult {
  ok: boolean
  error: string
  until: number
  data: FlyWebAccountResultData
}

export type FlyWebAccountMessage = 'FlyWebAccountRefresh' |
    'FlyWebAccountCreateStart' | 'FlyWebAccountCreateFinish' |
    'FlyWebAccountLoginStart' | 'FlyWebAccountLoginFinish' |
    'FlyWebAccountJoinWithWords' | 'FlyWebAccountResendCode' |
    'FlyWebAccountCancelFlow' | 'FlyWebAccountPasswordStart' |
    'FlyWebAccountPasswordFinish' | 'FlyWebAccountEmailStart' |
    'FlyWebAccountEmailFinish' | 'FlyWebAccountChainChange' |
    'FlyWebAccountListDevices' | 'FlyWebAccountRemoveDevice' |
    'FlyWebAccountRenameDevice' | 'FlyWebAccountNotMe' |
    'FlyWebAccountDismissWarnings' | 'FlyWebAccountDismissNotice' |
    'FlyWebAccountDeleteEverywhere' | 'FlyWebAccountCancelDeletion' |
    'FlyWebAccountLeave' | 'FlyWebAccountMailboxDeleteStart' |
    'FlyWebAccountMailboxDeleteFinish' | 'FlyWebAccountStartOverStart' |
    'FlyWebAccountStartOverFinish'

export class BraveSyncBrowserProxy {
  getSyncCode(): Promise<string> {
    return sendWithPromise('SyncSetupGetSyncCode');
  }
  getPureSyncCode(): Promise<string> {
    return sendWithPromise('SyncSetupGetPureSyncCode');
  }
  getQRCode(syncCode: string): Promise<string> {
    return sendWithPromise('SyncGetQRCode', syncCode);
  }
  getDeviceList(): Promise<BraveDeviceInfo[]> {
    return sendWithPromise('SyncGetDeviceList');
  }
  setSyncCode(syncCode: string): Promise<boolean> {
    return sendWithPromise('SyncSetupSetSyncCode', syncCode);
  }
  resetSyncChain(): Promise<boolean> {
    return sendWithPromise('SyncSetupReset');
  }
  deleteDevice(deviceId: string): Promise<boolean> {
    return sendWithPromise('SyncDeleteDevice', deviceId);
  }
  getSyncStatus(): Promise<BraveSyncStatus> {
    return sendWithPromise('SyncSetupGetSyncStatus');
  }
  permanentlyDeleteSyncAccount(): Promise<boolean>  {
    return sendWithPromise('SyncPermanentlyDeleteAccount');
  }
  // FlyWeb 3.2: null when the profile has no account service.
  getFlyWebAccountStatus(): Promise<FlyWebAccountStatus|null> {
    return sendWithPromise('FlyWebAccountGetStatus');
  }
  // 0–4 (zxcvbn). Only an indicator: nothing but the length is enforced.
  getFlyWebPasswordStrength(password: string): Promise<number> {
    return sendWithPromise('FlyWebAccountPasswordStrength', password);
  }
  // All the arguments are strings, in the order of the contract.
  flywebAccountCall(message: FlyWebAccountMessage, ...args: string[]):
      Promise<FlyWebAccountResult> {
    return sendWithPromise(message, ...args);
  }
  static getInstance() {
    return instance || (instance = new BraveSyncBrowserProxy())
  }
}

let instance: BraveSyncBrowserProxy|null = null

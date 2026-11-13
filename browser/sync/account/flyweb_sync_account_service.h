/* Copyright (c) 2026 The FlyWeb Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_BROWSER_SYNC_ACCOUNT_FLYWEB_SYNC_ACCOUNT_SERVICE_H_
#define BRAVE_BROWSER_SYNC_ACCOUNT_FLYWEB_SYNC_ACCOUNT_SERVICE_H_

// FlyWeb 3.2: the client of the sync account (FlyWeb/docs/sync-cuenta.md §11,
// hito H3). One per regular profile. It talks to /cuenta/v1 on the sync
// server (same origin as --sync-url), keeps the account prefs encrypted with
// OSCrypt like the seed, polls the account events and drives the states of
// the «Cuenta y sincronización» page (§9).
//
// What it never keeps: the password, KE, KL, KU (§11). They are computed off
// the UI thread when the user types the password. In the two flows with an
// email code in the middle (entrar, cambiar el email) KL and KU stay in
// memory, in SecretKey, until the second step, the cancel or 10 minutes (the
// life of the code); never in prefs, on disk or in the logs.
//
// It does not touch the sync engine, Nigori or BraveSyncAuthManager: it joins
// and leaves chains through BraveSyncServiceImpl's public calls, like the
// settings page does today.

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "base/functional/callback.h"
#include "base/memory/raw_ptr.h"
#include "base/memory/scoped_refptr.h"
#include "base/memory/weak_ptr.h"
#include "base/observer_list.h"
#include "base/observer_list_types.h"
#include "base/scoped_observation.h"
#include "base/time/time.h"
#include "base/timer/timer.h"
#include "base/values.h"
#include "brave/components/brave_sync/crypto/account_crypto.h"
#include "components/keyed_service/core/keyed_service.h"
#include "components/prefs/pref_change_registrar.h"
#include "components/sync/service/sync_service.h"
#include "components/sync/service/sync_service_observer.h"
#include "url/gurl.h"

class PrefRegistrySimple;
class Profile;

namespace network {
class SharedURLLoaderFactory;
class SimpleURLLoader;
}  // namespace network

namespace os_crypt_async {
class Encryptor;
}  // namespace os_crypt_async

namespace syncer {
class BraveSyncServiceImpl;
}  // namespace syncer

namespace flyweb_sync {

// Result of an operation, for the page. `error` is one of the server's codes
// (README of cuenta/: peticion, prueba, token, credenciales, sin-cuenta,
// aparato, cadena-ligada, cadena-borrada, email-ocupado, programado, espera,
// cupo, ocupado, interno) or a client one: red (no answer in 30 s or no
// network), servidor (unexpected answer), envoltura (the wrap did not open:
// nothing activated, P15), llavero (OSCrypt), email, contrasena-corta,
// contrasena-distinta, ocupado-local (another operation running), sin-flujo,
// sin-cadena, palabras.
struct AccountResult {
  AccountResult();
  AccountResult(AccountResult&&);
  AccountResult& operator=(AccountResult&&);
  ~AccountResult();

  static AccountResult Ok();
  static AccountResult Error(std::string error, int64_t until = 0);

  bool ok = false;
  std::string error;
  int64_t until = 0;  // with «espera»: Unix time when it can be tried again
  base::DictValue data;
};
using AccountCallback = base::OnceCallback<void(AccountResult)>;

// One account event (§6.2, «eventos» of the server).
struct AccountEvent {
  int64_t n = 0;
  std::string type;  // mac-anadido, contrasena-acertada, ...
  int64_t date = 0;
  std::string mac;
  std::string model;
  int64_t count = 0;  // contrasena-fallos: n
  int64_t since = 0;  // contrasena-fallos: desde
  int64_t when = 0;   // borrado-pedido: fecha
  std::string device;  // the device that did it, if the server says it
  std::string email;   // cadena-ligada: the email of the account (K1)
  base::DictValue ToValue() const;
};

class AccountService : public KeyedService,
                       public syncer::SyncServiceObserver {
 public:
  // The nine states of §9 are these plus the flags of GetStatus(): «falta el
  // código del email» is `flow`, «¿has sido tú?» is `warnings`, «sin red» is
  // `offline`.
  enum class State {
    kLoading,
    kNoAccount,
    kSynced,
    kDeletionScheduled,
    kKeysUnreadable,
    kAccountDeleted,
    kChainChanged,
  };

  class Observer : public base::CheckedObserver {
   public:
    // Anything on the page may have changed.
    virtual void OnAccountStatusChanged() {}
    // New warnings to show in a bar (§6.2).
    virtual void OnAccountWarnings(const std::vector<AccountEvent>& events) {}
  };

  explicit AccountService(Profile* profile);
  AccountService(const AccountService&) = delete;
  AccountService& operator=(const AccountService&) = delete;
  ~AccountService() override;

  void AddObserver(Observer* observer);
  void RemoveObserver(Observer* observer);

  State state() const { return state_; }
  // Everything the page shows; see the .cc for the keys.
  base::DictValue GetStatus() const;
  const std::vector<AccountEvent>& warnings() const { return warnings_; }
  // True while asking the server why this chain was disabled (§6.3): the
  // «account deleted» bar waits for the answer.
  bool resolving_chain_disabled() const { return resolving_chain_disabled_; }

  // Poll now (the page was opened, or Reintentar).
  void Refresh(AccountCallback callback);

  // --- Without an account (§5, §6.6, §8) ---
  // Create: email → code; then code + password + name. Links the chain this
  // Mac syncs, or a new one (data.recoveryWords then holds the 24 words).
  void CreateAccountStart(const std::string& email, AccountCallback callback);
  void CreateAccountFinish(const std::string& code,
                           std::string password,
                           const std::string& device_name,
                           AccountCallback callback);
  // Enter: email + password → code; then code + name.
  void LoginStart(const std::string& email,
                  std::string password,
                  AccountCallback callback);
  void LoginFinish(const std::string& code,
                   const std::string& device_name,
                   AccountCallback callback);
  // «Tengo mi clave de recuperación»: 24 words, or 25 with a valid date.
  void JoinWithRecoveryKey(const std::string& words, AccountCallback callback);
  // Reenviar: repeats the first step of the flow in progress.
  void ResendCode(AccountCallback callback);
  // Cancelar: forgets the flow (and the keys it kept).
  void CancelFlow();

  // --- Synced (§4.6, §6) ---
  void PasswordChangeStart(AccountCallback callback);
  void PasswordChangeFinish(const std::string& code,
                            std::string new_password,
                            AccountCallback callback);
  void EmailChangeStart(std::string password,
                        const std::string& new_email,
                        AccountCallback callback);
  void EmailChangeFinish(const std::string& code, AccountCallback callback);
  // New seed (data.recoveryWords), every other Mac has to log in again.
  void ChainChange(std::string password, AccountCallback callback);
  void ListDevices(AccountCallback callback);
  void RemoveDevice(const std::string& device_id, AccountCallback callback);
  void RenameDevice(const std::string& device_id,
                    const std::string& name,
                    AccountCallback callback);
  // «No he sido yo» (§6.2) and «He sido yo» (just hides the warnings).
  void NotMe(AccountCallback callback);
  void DismissWarnings();
  void DeleteEverywhere(AccountCallback callback);
  void CancelDeletion(AccountCallback callback);
  void LeaveOnThisMac(AccountCallback callback);
  // The notice states (deleted / key changed elsewhere): «Entendido».
  void DismissNotice();

  // --- Only the mailbox (§6.4, decision 1 of §10.1) ---
  void MailboxDeleteStart(const std::string& email, AccountCallback callback);
  void MailboxDeleteFinish(const std::string& code, AccountCallback callback);
  void StartOverStart(const std::string& email, AccountCallback callback);
  // New chain here (data.recoveryWords); the account follows it at day 14.
  void StartOverFinish(const std::string& code,
                       std::string password,
                       AccountCallback callback);

  // The model sent to the server: hw.model, filtered to [A-Za-z0-9,._ -].
  static std::string DeviceModel();
  static void RegisterProfilePrefs(PrefRegistrySimple* registry);

  // syncer::SyncServiceObserver:
  void OnStateChanged(syncer::SyncService* sync) override;
  void OnSyncShutdown(syncer::SyncService* sync) override;

  // KeyedService:
  void Shutdown() override;

  void SetURLLoaderFactoryForTesting(
      scoped_refptr<network::SharedURLLoaderFactory> factory);

 private:
  struct Flow;
  struct Request;

  // --- setup and prefs ---
  void OnEncryptorReady(scoped_refptr<os_crypt_async::Encryptor> encryptor);
  void LoadPrefs();
  void OnSeedPrefChanged();
  bool ReadSeed(std::array<uint8_t, brave_sync::crypto::account::kSeedSize>*
                    seed) const;
  void StoreAccount(const std::string& account_id,
                    const std::string& email,
                    const std::string& device_id,
                    const std::string& token);
  void StoreToken(const std::string& device_id, const std::string& token);
  void StoreString(const char* path, const std::string& value);
  std::optional<std::string> LoadString(const char* path) const;
  void ForgetAccount(bool keep_email);
  void SetNotice(const std::string& notice);
  void UpdateState();
  void NotifyStatus();

  syncer::BraveSyncServiceImpl* GetSyncService() const;
  bool HasChain() const { return !chain_keys_.empty(); }

  // --- HTTP ---
  enum class Auth { kNone, kToken, kProof, kTokenOrProof };
  struct CallSpec {
    CallSpec();
    CallSpec(CallSpec&&);
    CallSpec& operator=(CallSpec&&);
    ~CallSpec();
    std::string route;  // "eventos", "alta/confirmar", ...
    std::string op;     // proof op, for kProof
    Auth auth = Auth::kNone;
    std::string account_id_for_proof;  // default: the stored one
    bool empty_account_in_proof = false;
    bool attach_token = false;  // Bearer on a call without other auth
    base::DictValue body;
    // cadena/cambiar and empezar-de-cero/confirmar sign with other keys
    // (owned by the caller's callback). Without them, a copy of this Mac's
    // chain keys is taken when the call starts.
    const brave_sync::crypto::account::ChainKeys* proof_keys = nullptr;
    const brave_sync::crypto::account::ChainKeys* new_chain_keys = nullptr;
    std::unique_ptr<brave_sync::crypto::account::ChainKeys> owned_keys;
  };
  void Call(CallSpec spec, AccountCallback callback);
  void CallWithNonce(CallSpec spec,
                     AccountCallback callback,
                     AccountResult nonce_result);
  void Send(CallSpec spec,
            std::string body,
            std::vector<std::pair<std::string, std::string>> headers,
            AccountCallback callback);
  void OnResponse(std::unique_ptr<network::SimpleURLLoader> loader,
                  AccountCallback callback,
                  std::optional<std::string> body);
  GURL RouteURL(const std::string& route) const;
  std::string RoutePath(const std::string& route) const;

  // --- events ---
  void Poll(AccountCallback callback);
  void OnEvents(AccountCallback callback,
                bool used_proof,
                AccountResult result);
  void RenewToken(AccountCallback callback);
  void RegisterThisMac(AccountCallback callback);
  void SchedulePoll(bool failed);
  void OnChainDisabled(brave_sync::crypto::account::ChainKeys old_keys);

  // --- flows ---
  bool BeginOperation(AccountCallback& callback);
  AccountCallback EndOperation(AccountCallback callback);
  void DeriveKeys(std::string password,
                  std::string email,
                  base::OnceCallback<void(
                      std::optional<brave_sync::crypto::account::AccountKeys>)>
                      callback);
  void JoinChain(const brave_sync::crypto::account::SecretKey& seed,
                 AccountCallback callback);
  void LeaveChainThen(base::OnceClosure then);
  void RefreshEmail(AccountCallback then);
  void SaveWarnings();
  const std::string& DeviceSecret();
  void AddDeviceSecret(base::DictValue& body);
  void LoadWarnings();
  void PasswordChangeFinishImpl(const std::string& code,
                                std::string new_password,
                                AccountCallback callback);
  void EmailChangeStartImpl(std::string password,
                            const std::string& new_email,
                            AccountCallback callback);
  void ChainChangeImpl(std::string password, AccountCallback callback);
  std::string CurrentEmail() const;

  raw_ptr<Profile> profile_;
  scoped_refptr<network::SharedURLLoaderFactory> url_loader_factory_;
  GURL base_url_;
  scoped_refptr<os_crypt_async::Encryptor> encryptor_;
  PrefChangeRegistrar pref_change_registrar_;
  base::ScopedObservation<syncer::SyncService, syncer::SyncServiceObserver>
      sync_observation_{this};
  base::ObserverList<Observer> observers_;

  State state_ = State::kLoading;
  bool keys_unreadable_ = false;
  bool busy_ = false;
  bool own_chain_change_ = false;  // this Mac is leaving/switching chains
  bool resolving_chain_disabled_ = false;
  bool offline_ = false;
  std::string last_error_;
  base::Time last_contact_;

  // Account (decrypted from prefs).
  std::string account_id_;
  std::string email_;
  std::string device_id_;
  std::string token_;
  std::string device_secret_;  // K3, base64url of 32 random bytes
  int64_t last_event_ = 0;
  std::string server_state_;  // activa, borrado-programado, ...
  int64_t deletion_date_ = 0;
  std::string notice_;  // cuenta-borrada, cadena-cambiada
  base::ListValue devices_;

  // The chain this Mac syncs, kept while it does (and a little after, to ask
  // the server why it was disabled, §6.3).
  brave_sync::crypto::account::ChainKeys chain_keys_;
  std::vector<AccountEvent> warnings_;
  std::unique_ptr<Flow> flow_;

  base::OneShotTimer poll_timer_;
  base::OneShotTimer flow_timer_;
  base::TimeDelta poll_backoff_;
  bool polling_ = false;
  bool registering_ = false;
  // After joining an account, the first poll only reads up to the newest
  // event: the history of the account is not a warning for this Mac.
  bool baseline_pending_ = false;
  // When this Mac joined its chain by itself (null: before this run).
  base::Time joined_chain_at_;

  base::WeakPtrFactory<AccountService> weak_ptr_factory_{this};
};

}  // namespace flyweb_sync

#endif  // BRAVE_BROWSER_SYNC_ACCOUNT_FLYWEB_SYNC_ACCOUNT_SERVICE_H_

/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_COMPONENTS_BRAVE_COMPONENT_UPDATER_BROWSER_LOCAL_DATA_FILES_SERVICE_H_
#define BRAVE_COMPONENTS_BRAVE_COMPONENT_UPDATER_BROWSER_LOCAL_DATA_FILES_SERVICE_H_

#include <memory>
#include <string>

#include "base/files/file_path.h"
#include "base/observer_list.h"
#include "brave/components/brave_component_updater/browser/brave_component.h"

namespace brave_component_updater {

class LocalDataFilesObserver;

// FlyWeb: our own Local Data component, built by softmac
// FlyWeb/servidor/componentes (empaquetar.mjs) and signed on bak with the
// "datos-locales" key (claves-publicas.json).
const char kLocalDataFilesComponentName[] = "FlyWeb Local Data Updater";
const char kLocalDataFilesComponentId[] = "fmaniolnkjoldhophmcgimdfppdpdobj";
const char kLocalDataFilesComponentBase64PublicKey[] =
    "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAv4vMzrCf+oeSk6mPND5K"
    "CHpPY8rYwyfvbwuXVvnXTUwOUbxsb2JW41/NP1Z54g98tZwnENUu1dRXIa7ftBwX"
    "TccmBz6fi7LQtlPFfNu/i6sRVRM5qxk1BYa4URoeevf+yc/xjZKEQvnc9+XhfvdZ"
    "rRHZRnAztNf3TOmlOr4GZUkmRyiW1JeCvS3T9U+XR8Yneru3p1M6w2OAGPLrR9xo"
    "gUi9OPUaIRMHBp9vqJPMeofL8oufLKHAH0RjlJ01dGFjbkrAcLOwjHMSiQIi+CH4"
    "BHYl20Le3LJZ5duz2/OHBPWBNSC7873FUlaaTN4xiL+XTw/6Uui1aTkLVIUdYsuN"
    "vwIDAQAB";

// The component in charge of delegating access to different DAT files
// such as tracking protection.
class LocalDataFilesService : public BraveComponent {
 public:
  explicit LocalDataFilesService(BraveComponent::Delegate* delegate);
  LocalDataFilesService(const LocalDataFilesService&) = delete;
  LocalDataFilesService& operator=(const LocalDataFilesService&) = delete;
  ~LocalDataFilesService() override;
  bool Start();
  bool IsInitialized() const { return initialized_; }
  void AddObserver(LocalDataFilesObserver* observer);
  void RemoveObserver(LocalDataFilesObserver* observer);

  static void SetComponentIdAndBase64PublicKeyForTest(
      const std::string& component_id,
      const std::string& component_base64_public_key);

 protected:
  void OnComponentReady(const std::string& component_id,
      const base::FilePath& install_dir,
      const std::string& manifest) override;

 private:
  static std::string g_local_data_files_component_id_;
  static std::string g_local_data_files_component_base64_public_key_;

  bool initialized_;
  base::ObserverList<LocalDataFilesObserver>::Unchecked observers_;
};

// Creates the LocalDataFilesService
std::unique_ptr<LocalDataFilesService>
LocalDataFilesServiceFactory(BraveComponent::Delegate* delegate);

}  // namespace brave_component_updater

#endif  // BRAVE_COMPONENTS_BRAVE_COMPONENT_UPDATER_BROWSER_LOCAL_DATA_FILES_SERVICE_H_

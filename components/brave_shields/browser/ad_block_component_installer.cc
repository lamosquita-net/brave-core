/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/components/brave_shields/browser/ad_block_component_installer.h"

#include <memory>
#include <string>
#include <vector>

#include "base/base64.h"
#include "base/functional/bind.h"
#include "base/functional/callback.h"
#include "brave/components/brave_component_updater/browser/brave_on_demand_updater.h"
#include "components/component_updater/component_installer.h"
#include "components/component_updater/component_updater_service.h"
#include "crypto/sha2.h"

using brave_component_updater::BraveOnDemandUpdater;

namespace brave_shields {

namespace {

constexpr size_t kHashSize = 32;
const char kAdBlockResourceComponentName[] = "FlyWeb Shields: resources";
// FlyWeb: own component, signed on bak (softmac FlyWeb/servidor/componentes);
// Brave's update server needs Brave's private service key.
const char kAdBlockResourceComponentId[] = "cociljnememddffhfnpcgamaljohlion";
const char kAdBlockResourceComponentBase64PublicKey[] =
    "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAoavVS9RkFr7CvEvtBVPV"
    "UyWpZz6gd9an6lE38qCIfASITT81ZnSo5buYm4u1yJadsLJFtr4t2Th4uGdYD5uX"
    "PpawXi/bo6iNVxlVzKstWzAWNpAadjwAJ3prtmWWVG/2CM75o1Cu2G8WHsWBOkZa"
    "CZ9g/TH2ruwGTC99z2NJB0cYzetlqMkg1RYb6U257pcGvnCzB9FGUvOICPkUiV6p"
    "7FGwmy3K9bFdHvXAYFhyEGg9jGaZ7hbd9TZlL+6wv9rFtcfTUOBlgI+qUjXqpYnN"
    "eiQyW1tMSFQ2Ao4/y6PjdptrW/2pWJG6kNWsOivPVyYOpOJLgUcLooLCSoX9rkxU"
    "yQIDAQAB";

const char kAdBlockFilterListCatalogComponentName[] =
    "FlyWeb Shields: list catalog";
// FlyWeb: own component, signed on bak (softmac FlyWeb/servidor/componentes);
// Brave's update server needs Brave's private service key.
const char kAdBlockFilterListCatalogComponentId[] =
    "aafcdjdpmachpbehepncfijpeojclgfc";
const char kAdBlockFilterListCatalogComponentBase64PublicKey[] =
    "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEA0yDcmadLHX3cwtnyqR7b"
    "XYyOI9LhUiT+0hp1HbmW/87aHaCGMY9Ng13kTfiI1lgkAwO30eythOCUV8ocmqq7"
    "HU7qmQZNvq5vMG5Vzg6sp6IADS7FqKHat02aKrw6ZtkhH5aysLkuTP1DFOOYs1Ex"
    "XA0wuAewHcpsfxMWqOcN+nLvD6PWiNRfb9pUq+8u0gb2GUdqH8gPE+3J128tPvo6"
    "YMUnYtA0ydEWYG8VZkZJKdfSSTC2dl7eFhWkHiHDB5uJJH6pW0q50ihbeXVtA+eo"
    "gaab1Q6bjqdtHbkXnLm2Bssgqy/K3Mfb0GTzuOmt6DAW1rpwc1UJYkLYZAdL9t94"
    "lwIDAQAB";

const char kAdBlockIosDefaultDatComponentName[] = "Brave Ad Block Updater";
const char kAdBlockIosDefaultDatComponentId[] =
    "cffkpbalmllkdoenhmdmpbkajipdjfam";
const char kAdBlockIosDefaultDatComponentBase64PublicKey[] =
    "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAs0qzJmHSgIiw7IGFCxij"
    "1NnB5hJ5ZQ1LKW9htL4EBOaMJvmqaDs/wfq0nw/goBHWsqqkMBynRTu2Hxxirvdb"
    "cugn1Goys5QKPgAvKwDHJp9jlnADWm5xQvPQ4GE1mK1/I3ka9cEOCzPW6GI+wGLi"
    "VPx9VZrxHHsSBIJRaEB5Tyi5bj0CZ+kcfMnRTsXIBw3C6xJgCVKISQUkd8mawVvG"
    "vqOhBOogCdb9qza5eJ1Cgx8RWKucFfaWWxKLOelCiBMT1Hm1znAoVBHG/blhJJOD"
    "5HcH/heRrB4MvrE1J76WF3fvZ03aHVcnlLtQeiNNOZ7VbBDXdie8Nomf/QswbBGa"
    "VwIDAQAB";

class AdBlockComponentInstallerPolicy
    : public component_updater::ComponentInstallerPolicy {
 public:
  explicit AdBlockComponentInstallerPolicy(
      const std::string& component_public_key,
      const std::string& component_id,
      const std::string& component_name,
      OnComponentReadyCallback callback);
  ~AdBlockComponentInstallerPolicy() override;

  AdBlockComponentInstallerPolicy(const AdBlockComponentInstallerPolicy&) =
      delete;
  AdBlockComponentInstallerPolicy& operator=(
      const AdBlockComponentInstallerPolicy&) = delete;

  // component_updater::ComponentInstallerPolicy
  bool SupportsGroupPolicyEnabledComponentUpdates() const override;
  bool RequiresNetworkEncryption() const override;
  update_client::CrxInstaller::Result OnCustomInstall(
      const base::Value::Dict& manifest,
      const base::FilePath& install_dir) override;
  void OnCustomUninstall() override;
  bool VerifyInstallation(const base::Value::Dict& manifest,
                          const base::FilePath& install_dir) const override;
  void ComponentReady(const base::Version& version,
                      const base::FilePath& path,
                      base::Value::Dict manifest) override;
  base::FilePath GetRelativeInstallDir() const override;
  void GetHash(std::vector<uint8_t>* hash) const override;
  std::string GetName() const override;
  update_client::InstallerAttributes GetInstallerAttributes() const override;

 private:
  const std::string component_id_;
  const std::string component_name_;
  OnComponentReadyCallback ready_callback_;
  uint8_t component_hash_[kHashSize];
};

AdBlockComponentInstallerPolicy::AdBlockComponentInstallerPolicy(
    const std::string& component_public_key,
    const std::string& component_id,
    const std::string& component_name,
    OnComponentReadyCallback callback)
    : component_id_(component_id),
      component_name_(component_name),
      ready_callback_(callback) {
  // Generate hash from public key.
  std::string decoded_public_key;
  base::Base64Decode(component_public_key, &decoded_public_key);
  crypto::SHA256HashString(decoded_public_key, component_hash_, kHashSize);
}

AdBlockComponentInstallerPolicy::~AdBlockComponentInstallerPolicy() = default;

bool AdBlockComponentInstallerPolicy::
    SupportsGroupPolicyEnabledComponentUpdates() const {
  return true;
}

bool AdBlockComponentInstallerPolicy::RequiresNetworkEncryption() const {
  return false;
}

update_client::CrxInstaller::Result
AdBlockComponentInstallerPolicy::OnCustomInstall(
    const base::Value::Dict& manifest,
    const base::FilePath& install_dir) {
  return update_client::CrxInstaller::Result(0);
}

void AdBlockComponentInstallerPolicy::OnCustomUninstall() {}

void AdBlockComponentInstallerPolicy::ComponentReady(
    const base::Version& version,
    const base::FilePath& path,
    base::Value::Dict manifest) {
  ready_callback_.Run(path);
}

bool AdBlockComponentInstallerPolicy::VerifyInstallation(
    const base::Value::Dict& manifest,
    const base::FilePath& install_dir) const {
  return true;
}

base::FilePath AdBlockComponentInstallerPolicy::GetRelativeInstallDir() const {
  return base::FilePath::FromUTF8Unsafe(component_id_);
}

void AdBlockComponentInstallerPolicy::GetHash(
    std::vector<uint8_t>* hash) const {
  hash->assign(component_hash_, component_hash_ + kHashSize);
}

std::string AdBlockComponentInstallerPolicy::GetName() const {
  return component_name_;
}

update_client::InstallerAttributes
AdBlockComponentInstallerPolicy::GetInstallerAttributes() const {
  return update_client::InstallerAttributes();
}

void OnRegistered(const std::string& component_id) {
  BraveOnDemandUpdater::GetInstance()->OnDemandUpdate(component_id);
}

}  // namespace

void RegisterAdBlockIosDefaultDatComponent(
    component_updater::ComponentUpdateService* cus,
    OnComponentReadyCallback callback) {
  // In test, |cus| could be nullptr.
  if (!cus)
    return;

  auto installer = base::MakeRefCounted<component_updater::ComponentInstaller>(
      std::make_unique<AdBlockComponentInstallerPolicy>(
          kAdBlockIosDefaultDatComponentBase64PublicKey,
          kAdBlockIosDefaultDatComponentId, kAdBlockIosDefaultDatComponentName,
          callback));
  installer->Register(
      cus, base::BindOnce(&OnRegistered, kAdBlockIosDefaultDatComponentId));
}

void RegisterAdBlockDefaultResourceComponent(
    component_updater::ComponentUpdateService* cus,
    OnComponentReadyCallback callback) {
  // In test, |cus| could be nullptr.
  if (!cus)
    return;

  auto installer = base::MakeRefCounted<component_updater::ComponentInstaller>(
      std::make_unique<AdBlockComponentInstallerPolicy>(
          kAdBlockResourceComponentBase64PublicKey, kAdBlockResourceComponentId,
          kAdBlockResourceComponentName, callback));
  installer->Register(
      cus, base::BindOnce(&OnRegistered, kAdBlockResourceComponentId));
}

void RegisterAdBlockFilterListCatalogComponent(
    component_updater::ComponentUpdateService* cus,
    OnComponentReadyCallback callback) {
  // In test, |cus| could be nullptr.
  if (!cus)
    return;

  auto installer = base::MakeRefCounted<component_updater::ComponentInstaller>(
      std::make_unique<AdBlockComponentInstallerPolicy>(
          kAdBlockFilterListCatalogComponentBase64PublicKey,
          kAdBlockFilterListCatalogComponentId,
          kAdBlockFilterListCatalogComponentName, callback));
  installer->Register(
      cus, base::BindOnce(&OnRegistered, kAdBlockFilterListCatalogComponentId));
}

void RegisterAdBlockFiltersComponent(
    component_updater::ComponentUpdateService* cus,
    const std::string& component_public_key,
    const std::string& component_id,
    const std::string& component_name,
    OnComponentReadyCallback callback) {
  // In test, |cus| could be nullptr.
  if (!cus)
    return;

  auto installer = base::MakeRefCounted<component_updater::ComponentInstaller>(
      std::make_unique<AdBlockComponentInstallerPolicy>(
          component_public_key, component_id, component_name, callback));
  installer->Register(cus, base::BindOnce(&OnRegistered, component_id));
}

}  // namespace brave_shields

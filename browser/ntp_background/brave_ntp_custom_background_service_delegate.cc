// Copyright (c) 2021 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

#include "brave/browser/ntp_background/brave_ntp_custom_background_service_delegate.h"

#include <utility>

#include "base/files/file_path.h"
#include "brave/browser/ntp_background/constants.h"
#include "brave/browser/ntp_background/custom_background_file_manager.h"
#include "brave/browser/ntp_background/ntp_background_prefs.h"
#include "brave/components/constants/pref_names.h"
#include "brave/components/ntp_background_images/browser/url_constants.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/common/pref_names.h"
#include "chrome/common/url_constants.h"
#include "components/prefs/pref_service.h"
#include "url/gurl.h"

BraveNTPCustomBackgroundServiceDelegate::
    BraveNTPCustomBackgroundServiceDelegate(Profile* profile)
    : profile_(profile),
      file_manager_(std::make_unique<CustomBackgroundFileManager>(profile_)) {
  if (ShouldMigrateCustomImagePref()) {
    DVLOG(2) << "Migrate old custom background image";

    MigrateCustomImage();
  }
}

BraveNTPCustomBackgroundServiceDelegate::
    ~BraveNTPCustomBackgroundServiceDelegate() = default;

bool BraveNTPCustomBackgroundServiceDelegate::ShouldMigrateCustomImagePref()
    const {
  auto prefs = NTPBackgroundPrefs(profile_->GetPrefs());
  return prefs.IsCustomImageType() && prefs.GetCustomImageList().empty();
}

void BraveNTPCustomBackgroundServiceDelegate::MigrateCustomImage(
    base::OnceCallback<void(bool)> callback) {
  auto prefs = NTPBackgroundPrefs(profile_->GetPrefs());
  file_manager_->MoveImage(
      base::FilePath(profile_->GetPath().AppendASCII(
          ntp_background_images::kSanitizedImageFileNameDeprecated)),
      base::BindOnce(
          [](Profile* profile, bool result) {
            auto prefs = NTPBackgroundPrefs(profile->GetPrefs());
            if (!result) {
              LOG(ERROR) << "Failed to migrate Custom background image. "
                            "Resets to default background";
              prefs.SetType(NTPBackgroundPrefs::Type::kBrave);
              prefs.SetShouldUseRandomValue(true);
              prefs.SetSelectedValue(std::string());
              return false;
            }

            prefs.SetSelectedValue(
                ntp_background_images::kSanitizedImageFileNameDeprecated);
            prefs.AddCustomImageToList(
                ntp_background_images::kSanitizedImageFileNameDeprecated);
            return true;
          },
          profile_)
          .Then(std::move(callback)));
}

bool BraveNTPCustomBackgroundServiceDelegate::IsCustomImageBackgroundEnabled()
    const {
  if (profile_->GetPrefs()->IsManagedPreference(
          prefs::kNtpCustomBackgroundDict)) {
    return false;
  }

  return NTPBackgroundPrefs(profile_->GetPrefs()).IsCustomImageType();
}

base::FilePath
BraveNTPCustomBackgroundServiceDelegate::GetCustomBackgroundImageLocalFilePath(
    const GURL& url) const {
  return CustomBackgroundFileManager::Converter(url, file_manager_.get())
      .To<base::FilePath>();
}

GURL BraveNTPCustomBackgroundServiceDelegate::GetCustomBackgroundImageURL()
    const {
  DCHECK(IsCustomImageBackgroundEnabled());

  auto prefs = NTPBackgroundPrefs(profile_->GetPrefs());
  auto name = prefs.GetSelectedValue();
  return CustomBackgroundFileManager::Converter(name).To<GURL>();
}

bool BraveNTPCustomBackgroundServiceDelegate::IsColorBackgroundEnabled() const {
  return NTPBackgroundPrefs(profile_->GetPrefs()).IsColorType();
}

std::string BraveNTPCustomBackgroundServiceDelegate::GetColor() const {
  if (!IsColorBackgroundEnabled()) {
    return {};
  }

  const auto selected_value =
      NTPBackgroundPrefs(profile_->GetPrefs()).GetSelectedValue();
  return selected_value;
}

bool BraveNTPCustomBackgroundServiceDelegate::ShouldUseRandomValue() const {
  return NTPBackgroundPrefs(profile_->GetPrefs()).ShouldUseRandomValue();
}

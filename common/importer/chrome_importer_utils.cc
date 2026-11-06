/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "brave/common/importer/chrome_importer_utils.h"

#include <memory>
#include <optional>
#include <utility>

#include "base/files/file_path.h"
#include "base/files/file_util.h"
#include "base/json/json_reader.h"
#include "base/values.h"
#include "brave/common/importer/importer_constants.h"
#include "components/user_data_importer/common/importer_data_types.h"

#if BUILDFLAG(ENABLE_EXTENSIONS)
#include "extensions/common/extension.h"
#include "extensions/common/manifest.h"
#endif

#if BUILDFLAG(ENABLE_EXTENSIONS)
using extensions::Extension;
using extensions::Manifest;
#endif

namespace {
#if BUILDFLAG(ENABLE_EXTENSIONS)

std::optional<base::DictValue> GetChromeExtensionsListFromFile(
    const base::FilePath& preference_path) {
  if (!base::PathExists(preference_path))
    return std::nullopt;

  std::string preference_content;
  base::ReadFileToString(preference_path, &preference_content);

  std::optional<base::Value> preference = base::JSONReader::Read(
      preference_content, base::JSON_PARSE_CHROMIUM_EXTENSIONS);
  DCHECK(preference);
  DCHECK(preference->is_dict());
  if (auto* extensions = preference->GetDict().FindDictByDottedPath(
          kChromeExtensionsListPath)) {
    return std::move(*extensions);
  }
  return std::nullopt;
}

bool HasImportableExtensions(const base::FilePath& profile_path) {
  return GetImportableChromeExtensionsList(profile_path).has_value();
}

std::vector<std::string> GetImportableListFromChromeExtensionsList(
    const base::DictValue& extensions_list) {
  std::vector<std::string> extensions;
  for (const auto [key, value] : extensions_list) {
    DCHECK(value.is_dict());
    const base::DictValue& dict = value.GetDict();
    // Only import if type is extension, it's came from webstore and it's not
    // installed by default.
    if (dict.FindBool("was_installed_by_default").value_or(true))
      continue;

    // `"state": 0` means disabled state
    if (!dict.FindInt("state").value_or(false))
      continue;

    if (!dict.FindBool("from_webstore").value_or(false))
      continue;

    if (auto* manifest_dict = dict.FindDict("manifest")) {
      if (Manifest::GetTypeFromManifestValue(*manifest_dict) ==
          Manifest::TYPE_EXTENSION) {
        extensions.push_back(key);
      }
    }
  }

  return extensions;
}

std::optional<base::DictValue> GetChromeExtensionsList(
    const base::FilePath& profile_path) {
  auto list_from_secure_preference = GetChromeExtensionsListFromFile(
      profile_path.AppendASCII(kChromeSecurePreferencesFile));

  auto list_from_preferences = GetChromeExtensionsListFromFile(
      profile_path.AppendASCII(kChromePreferencesFile));
  if (!list_from_secure_preference.has_value())
    return list_from_preferences;

  if (list_from_secure_preference.has_value() &&
      list_from_preferences.has_value()) {
    list_from_secure_preference->Merge(
        std::move(list_from_preferences.value()));
    return list_from_secure_preference;
  }

  return list_from_secure_preference;
}
#endif

bool IsLastActiveProfile(const std::string& profile,
                         const base::ListValue& last_active_profiles) {
  for (const auto& it : last_active_profiles) {
    if (it.GetString() == profile) {
      return true;
    }
  }
  return false;
}

}  // namespace

base::ListValue GetChromeSourceProfiles(
    const base::FilePath& local_state_path) {
  base::ListValue profiles;
  if (base::PathExists(local_state_path)) {
    std::string local_state_content;
    base::ReadFileToString(local_state_path, &local_state_content);
    std::optional<base::Value> local_state = base::JSONReader::Read(
        local_state_content, base::JSON_PARSE_CHROMIUM_EXTENSIONS);
    if (!local_state)
      return profiles;

    const auto* local_state_dict = local_state->GetIfDict();
    if (!local_state_dict)
      return profiles;

    const auto* profile_dict = local_state_dict->FindDict("profile");
    if (profile_dict) {
      const auto* last_active_profiles =
          profile_dict->FindList("last_active_profiles");

      const auto* info_cache = profile_dict->FindDict("info_cache");
      if (info_cache) {
        for (const auto value : *info_cache) {
          const auto* profile = value.second.GetIfDict();
          if (!profile)
            continue;

          auto* name = profile->FindString("name");
          DCHECK(name);
          base::DictValue entry;
          entry.Set("id", value.first);
          entry.Set("name", *name);
          if (last_active_profiles)
            entry.Set("last_active",
                      IsLastActiveProfile(value.first, *last_active_profiles));

          auto* avatar_icon = profile->FindString("avatar_icon");
          if (avatar_icon) {
            entry.Set("avatar_icon", *avatar_icon);
          }
          auto active_time = profile->FindDouble("active_time");
          if (active_time) {
            entry.Set("active_time", *active_time);
          }
          profiles.Append(std::move(entry));
        }
      }
    }
  }
  if (profiles.empty()) {
    base::DictValue entry;
    entry.Set("id", "");
    entry.Set("name", "Default");
    profiles.Append(std::move(entry));
  }
  return profiles;
}

bool ChromeImporterCanImport(const base::FilePath& profile,
                             uint16_t* services_supported) {
  DCHECK(services_supported);
  *services_supported = user_data_importer::NONE;

  base::FilePath bookmarks =
    profile.Append(base::FilePath::StringType(FILE_PATH_LITERAL("Bookmarks")));
  base::FilePath history =
    profile.Append(base::FilePath::StringType(FILE_PATH_LITERAL("History")));
  if (base::PathExists(bookmarks))
    *services_supported |= user_data_importer::FAVORITES;
  if (base::PathExists(history))
    *services_supported |= user_data_importer::HISTORY;
  // FlyWeb (156): passwords and payment methods are no longer imported from
  // Chromium-based browsers. The utility-process ChromeImporter decrypted them
  // with os_crypt/sync, which no longer exists (Brave 1.98 dropped them too).
#if BUILDFLAG(ENABLE_EXTENSIONS)
  if (HasImportableExtensions(profile))
    *services_supported |= user_data_importer::EXTENSIONS;
#endif

  return *services_supported != user_data_importer::NONE;
}

#if BUILDFLAG(ENABLE_EXTENSIONS)
std::optional<std::vector<std::string>> GetImportableChromeExtensionsList(
    const base::FilePath& profile_path) {
  if (auto extensions = GetChromeExtensionsList(profile_path)) {
    return GetImportableListFromChromeExtensionsList(extensions.value());
  }
  return std::nullopt;
}
#endif

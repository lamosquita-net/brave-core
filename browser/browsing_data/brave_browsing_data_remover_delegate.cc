/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/browser/browsing_data/brave_browsing_data_remover_delegate.h"

#include <utility>
#include <vector>

#include "base/containers/flat_map.h"
#include "base/memory/raw_ptr.h"
#include "base/no_destructor.h"
#include "brave/components/content_settings/core/browser/brave_content_settings_pref_provider.h"
#include "brave/components/content_settings/core/browser/brave_content_settings_utils.h"
#include "build/build_config.h"
#include "chrome/browser/browsing_data/chrome_browsing_data_remover_constants.h"
#include "chrome/browser/content_settings/host_content_settings_map_factory.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/common/buildflags.h"
#include "components/content_settings/core/browser/host_content_settings_map.h"
#include "components/content_settings/core/common/content_settings.h"
#include "components/content_settings/core/common/content_settings_pattern.h"
#include "components/content_settings/core/common/content_settings_types.h"

namespace {

// Clearing "Content settings" must not reset the user's global Shields
// defaults (e.g. aggressive ad blocking, blocked cookies everywhere): the
// upstream remover nukes every rule of the type, wildcard ones included.
// Snapshot the user-level wildcard Shields rules before clearing and restore
// them afterwards. Ported from Brave 1.98.
class ContentSettingsDefaultsKeeper {
 public:
  explicit ContentSettingsDefaultsKeeper(Profile* profile) : profile_(profile) {
    auto* map = HostContentSettingsMapFactory::GetForProfile(profile_);

    const auto& shields_content_types =
        content_settings::GetShieldsContentSettingsTypes();
    for (const auto content_type : shields_content_types) {
      if (content_type == ContentSettingsType::BRAVE_COOKIES) {
        // BRAVE_COOKIES don't have own default values. They rely on the
        // default COOKIES settings which are stored in the profile prefs.
        continue;
      }
      ContentSettingsForOneType settings =
          map->GetSettingsForOneType(content_type);
      for (const auto& setting : settings) {
        // Only preserve user-level wildcard rules. Re-applying rules from
        // other providers (e.g. policy) through the user pref store would
        // leave stale user-level overrides once their source is gone.
        if (setting.source == content_settings::ProviderType::kPrefProvider &&
            setting.primary_pattern.MatchesAllHosts()) {
          defaults_[content_type].push_back(setting);
        }
      }
    }
  }

  ContentSettingsDefaultsKeeper(const ContentSettingsDefaultsKeeper&) = delete;
  ContentSettingsDefaultsKeeper& operator=(
      const ContentSettingsDefaultsKeeper&) = delete;

  ~ContentSettingsDefaultsKeeper() {
    auto* map = HostContentSettingsMapFactory::GetForProfile(profile_);
    for (auto&& [content_type, settings] : defaults_) {
      for (auto&& setting : settings) {
        RestoreSetting(map, content_type, std::move(setting));
      }
    }
  }

 private:
  static const ContentSettingsPattern& BalancedPattern() {
    static const base::NoDestructor<ContentSettingsPattern> kPattern(
        ContentSettingsPattern::FromString("https://balanced/*"));
    return *kPattern;
  }

  void RestoreSetting(HostContentSettingsMap* map,
                      ContentSettingsType content_type,
                      ContentSettingPatternSource setting) {
    if (content_type == ContentSettingsType::BRAVE_FINGERPRINTING_V2 &&
        setting.secondary_pattern == BalancedPattern()) {
      // Legacy "balanced" fingerprinting rules (possibly restored by Sync after
      // BravePrefProvider's migration) can't be written back with a
      // non-default value: no provider accepts them. Write the default
      // instead, which also tells Sync to drop the stale rule.
      map->SetWebsiteSettingCustomScope(
          setting.primary_pattern, setting.secondary_pattern,
          ContentSettingsType::BRAVE_FINGERPRINTING_V2, base::Value(), {});
    } else {
      map->SetWebsiteSettingCustomScope(setting.primary_pattern,
                                        setting.secondary_pattern, content_type,
                                        std::move(setting.setting_value));
    }
  }

  raw_ptr<Profile> profile_ = nullptr;
  base::flat_map<ContentSettingsType, std::vector<ContentSettingPatternSource>>
      defaults_;
};

}  // namespace

BraveBrowsingDataRemoverDelegate::BraveBrowsingDataRemoverDelegate(
    content::BrowserContext* browser_context)
    : ChromeBrowsingDataRemoverDelegate(browser_context),
      profile_(Profile::FromBrowserContext(browser_context)) {}

BraveBrowsingDataRemoverDelegate::~BraveBrowsingDataRemoverDelegate() = default;

void BraveBrowsingDataRemoverDelegate::RemoveEmbedderData(
    const base::Time& delete_begin,
    const base::Time& delete_end,
    uint64_t remove_mask,
    content::BrowsingDataFilterBuilder* filter_builder,
    uint64_t origin_type_mask,
    base::OnceCallback<void(/*failed_data_types=*/uint64_t)> callback) {
  ContentSettingsDefaultsKeeper default_values_keeper(profile_);

  ChromeBrowsingDataRemoverDelegate::RemoveEmbedderData(
      delete_begin, delete_end, remove_mask, filter_builder, origin_type_mask,
      std::move(callback));

  // We do this because ChromeBrowsingDataRemoverDelegate::RemoveEmbedderData()
  // doesn't clear shields settings with non all time range.
  // The reason is upstream assumes that plugins type only as empty string
  // resource ids with plugins type. but we use plugins type to store our
  // shields settings with non-empty resource ids.
  if (remove_mask & chrome_browsing_data_remover::DATA_TYPE_CONTENT_SETTINGS) {
    ClearShieldsSettings(delete_begin, delete_end);
  }
}

void BraveBrowsingDataRemoverDelegate::ClearShieldsSettings(
    base::Time begin_time,
    base::Time end_time) {
  if (begin_time.is_null() && (end_time.is_null() || end_time.is_max())) {
    // For all time range, we don't need to do anything here because
    // ChromeBrowsingDataRemoverDelegate::RemoveEmbedderData() nukes whole
    // plugins type.
    return;
  }

  auto* map = HostContentSettingsMapFactory::GetForProfile(profile_);
  auto* provider =
      static_cast<content_settings::BravePrefProvider*>(map->GetPrefProvider());
  for (const auto& content_type :
       content_settings::GetShieldsContentSettingsTypes()) {
    ContentSettingsForOneType settings =
        map->GetSettingsForOneType(content_type);
    for (const ContentSettingPatternSource& setting : settings) {
      base::Time last_modified = setting.metadata.last_modified();
      if (last_modified >= begin_time &&
          (last_modified < end_time || end_time.is_null())) {
        provider->SetWebsiteSetting(setting.primary_pattern,
                                    setting.secondary_pattern, content_type,
                                    base::Value(), {});
      }
    }
  }
}

/* Copyright (c) 2023 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "brave/browser/ui/tabs/shared_pinned_tab_service_factory.h"

#include "base/feature_list.h"
#include "base/no_destructor.h"
#include "brave/browser/ui/tabs/brave_tab_prefs.h"
#include "brave/browser/ui/tabs/features.h"
#include "brave/browser/ui/tabs/shared_pinned_tab_service.h"
#include "chrome/browser/profiles/incognito_helpers.h"
#include "chrome/browser/profiles/profile.h"
#include "components/prefs/pref_service.h"

// static
SharedPinnedTabService* SharedPinnedTabServiceFactory::GetForProfile(
    Profile* profile) {
  return static_cast<SharedPinnedTabService*>(
      GetInstance()->GetServiceForBrowserContext(profile, true));
}

// static
bool SharedPinnedTabServiceFactory::IsEnabledForProfile(Profile* profile) {
  return base::FeatureList::IsEnabled(tabs::features::kBraveSharedPinnedTabs) &&
         profile && GetForProfile(profile);
}

SharedPinnedTabServiceFactory* SharedPinnedTabServiceFactory::GetInstance() {
  static base::NoDestructor<SharedPinnedTabServiceFactory> instance;
  return instance.get();
}

SharedPinnedTabServiceFactory::SharedPinnedTabServiceFactory()
    : ProfileKeyedServiceFactory(
          "SharedPinnedTabService",
          ProfileSelections::Builder()
              .WithRegular(ProfileSelection::kOwnInstance)
              .WithGuest(ProfileSelection::kOwnInstance)
              .Build()) {}

SharedPinnedTabServiceFactory::~SharedPinnedTabServiceFactory() {}

KeyedService* SharedPinnedTabServiceFactory::BuildServiceInstanceFor(
    content::BrowserContext* context) const {
  // FlyWeb: no service (and so no sharing) when the user turned it off in
  // Settings (F7.8). The pref is read once per profile: changes apply after
  // relaunching.
  Profile* profile = Profile::FromBrowserContext(context);
  if (!profile->GetPrefs()->GetBoolean(brave_tabs::kSharedPinnedTab)) {
    return nullptr;
  }
  return new SharedPinnedTabService(profile);
}

bool SharedPinnedTabServiceFactory::ServiceIsCreatedWithBrowserContext() const {
  return true;
}

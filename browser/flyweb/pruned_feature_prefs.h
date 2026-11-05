/* Copyright (c) 2026 lamosquita. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_BROWSER_FLYWEB_PRUNED_FEATURE_PREFS_H_
#define BRAVE_BROWSER_FLYWEB_PRUNED_FEATURE_PREFS_H_

class PrefRegistrySimple;
class PrefService;

namespace user_prefs {
class PrefRegistrySyncable;
}  // namespace user_prefs

// Prefs left in FlyWeb 2.x profiles by Brave features that FlyWeb no longer
// ships (pruned in the Chromium 156 port). They are registered only so that
// MigrateObsolete*Prefs() can clear them; nothing else reads them.
namespace flyweb {

void RegisterPrunedFeatureProfilePrefs(
    user_prefs::PrefRegistrySyncable* registry);
void ClearPrunedFeatureProfilePrefs(PrefService* profile_prefs);

void RegisterPrunedFeatureLocalStatePrefs(PrefRegistrySimple* registry);
void ClearPrunedFeatureLocalStatePrefs(PrefService* local_state);

}  // namespace flyweb

#endif  // BRAVE_BROWSER_FLYWEB_PRUNED_FEATURE_PREFS_H_

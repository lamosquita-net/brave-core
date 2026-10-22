// Copyright (c) 2019 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

#ifndef BRAVE_COMPONENTS_BRAVE_SHIELDS_CORE_COMMON_BRAVE_SHIELD_CONSTANTS_H_
#define BRAVE_COMPONENTS_BRAVE_SHIELDS_CORE_COMMON_BRAVE_SHIELD_CONSTANTS_H_

#include "base/files/file_path.h"

namespace brave_shields {

inline constexpr char kAds[] = "shieldsAds";
inline constexpr char kCosmeticFiltering[] = "cosmeticFiltering";
inline constexpr char kTrackers[] = "trackers";
inline constexpr char kHTTPUpgradableResources[] = "httpUpgradableResources";
inline constexpr char kHTTPSUpgrades[] = "httpsUpgrades";
inline constexpr char kJavaScript[] = "javascript";
inline constexpr char kFingerprintingV2[] = "fingerprintingV2";
inline constexpr char kBraveShields[] = "braveShields";
inline constexpr char kReferrers[] = "referrers";
inline constexpr char kCookies[] = "shieldsCookiesV3";
inline constexpr char kFacebookEmbeds[] = "fb-embeds";
inline constexpr char kTwitterEmbeds[] = "twitter-embeds";
inline constexpr char kLinkedInEmbeds[] = "linked-in-embeds";

// Values used before the migration away from ResourceIdentifier, kept around
// for migration purposes only.
inline constexpr char kObsoleteAds[] = "ads";
inline constexpr char kObsoleteCookies[] = "cookies";
inline constexpr char kObsoleteShieldsCookies[] = "shieldsCookies";

// Some users were not properly migrated from fingerprinting V1.
inline constexpr char kObsoleteFingerprinting[] = "fingerprinting";

// Filename for cached text from a custom filter list subscription
const base::FilePath::CharType kCustomSubscriptionListText[] =
    FILE_PATH_LITERAL("list_text.txt");

inline constexpr char kCookieListUuid[] =
    "AC023D22-AE88-4060-A978-4FEEEC4221693";
inline constexpr char kMobileNotificationsListUuid[] =
    "2F3DCE16-A19A-493C-A88F-2E110FBD37D6";
inline constexpr char kExperimentalListUuid[] =
    "564C3B75-8731-404C-AD7C-5683258BA0B0";

// FlyWeb: own components, signed on bak (softmac FlyWeb/servidor/componentes);
// Brave's update server needs Brave's private service key. The resources
// component of the 2.0 has its own ID and key ("recursos-20" in
// claves-publicas.json): adblock-rust 0.8's resources.json does not work in
// 1.x's 0.7.9, so 1.x keeps the old ID (brave-sync-shields.md 3.4).
inline constexpr char kAdBlockResourceComponentName[] =
    "FlyWeb Shields: resources";
inline constexpr char kAdBlockResourceComponentId[] =
    "nilendadbigifkognoeoodppkjkgmppg";
inline constexpr char kAdBlockResourceComponentBase64PublicKey[] =
    "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAw2IP9JoOjG9V8kRRJ4gJ"
    "GkJHJcyrJYu+1RPH3/ZbPh7eGP+4O1pFMy6/z2TubZFtD/K89lX00Hj6kaIQRfcI"
    "Rns5jS9kyS6alIfHLe4Q7ZljXvCB3sOYik741aUk4DADHXhvMCmSmf5wRriN/Bp0"
    "stoeCVIWfXSYtOZDZoZeRk+gIhdjL2IR1ArGV4k3GrDMQBhw6AjBhhnDdgkAoCcJ"
    "3dhWbNyPY37gJUpz8L1/TY1v7wTnQLCklg9cRoa+MXjlD/FrzC2X0ueX7Uj4VWCR"
    "kuns4YdznxxkypBbFrC/BG+LEoixvI6Bc3wmww9sPpKOww1yU6+XmzxLKTa0MWYt"
    "fwIDAQAB";

inline constexpr char kAdBlockFilterListCatalogComponentName[] =
    "FlyWeb Shields: list catalog";
inline constexpr char kAdBlockFilterListCatalogComponentId[] =
    "aafcdjdpmachpbehepncfijpeojclgfc";
inline constexpr char kAdBlockFilterListCatalogComponentBase64PublicKey[] =
    "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEA0yDcmadLHX3cwtnyqR7b"
    "XYyOI9LhUiT+0hp1HbmW/87aHaCGMY9Ng13kTfiI1lgkAwO30eythOCUV8ocmqq7"
    "HU7qmQZNvq5vMG5Vzg6sp6IADS7FqKHat02aKrw6ZtkhH5aysLkuTP1DFOOYs1Ex"
    "XA0wuAewHcpsfxMWqOcN+nLvD6PWiNRfb9pUq+8u0gb2GUdqH8gPE+3J128tPvo6"
    "YMUnYtA0ydEWYG8VZkZJKdfSSTC2dl7eFhWkHiHDB5uJJH6pW0q50ihbeXVtA+eo"
    "gaab1Q6bjqdtHbkXnLm2Bssgqy/K3Mfb0GTzuOmt6DAW1rpwc1UJYkLYZAdL9t94"
    "lwIDAQAB";

inline constexpr char kCookieListEnabledHistogram[] =
    "Brave.Shields.CookieListEnabled";
inline constexpr char kCookieListPromptHistogram[] =
    "Brave.Shields.CookieListPrompt";

}  // namespace brave_shields

#endif  // BRAVE_COMPONENTS_BRAVE_SHIELDS_CORE_COMMON_BRAVE_SHIELD_CONSTANTS_H_

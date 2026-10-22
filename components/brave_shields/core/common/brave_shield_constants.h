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
// component keeps 1.x's ID for now: brave-sync-shields.md 3.4 asks for a new
// ID and key for the 2.0 (adblock 0.8 resources.json), pending on bak.
inline constexpr char kAdBlockResourceComponentName[] =
    "FlyWeb Shields: resources";
inline constexpr char kAdBlockResourceComponentId[] =
    "cociljnememddffhfnpcgamaljohlion";
inline constexpr char kAdBlockResourceComponentBase64PublicKey[] =
    "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAoavVS9RkFr7CvEvtBVPV"
    "UyWpZz6gd9an6lE38qCIfASITT81ZnSo5buYm4u1yJadsLJFtr4t2Th4uGdYD5uX"
    "PpawXi/bo6iNVxlVzKstWzAWNpAadjwAJ3prtmWWVG/2CM75o1Cu2G8WHsWBOkZa"
    "CZ9g/TH2ruwGTC99z2NJB0cYzetlqMkg1RYb6U257pcGvnCzB9FGUvOICPkUiV6p"
    "7FGwmy3K9bFdHvXAYFhyEGg9jGaZ7hbd9TZlL+6wv9rFtcfTUOBlgI+qUjXqpYnN"
    "eiQyW1tMSFQ2Ao4/y6PjdptrW/2pWJG6kNWsOivPVyYOpOJLgUcLooLCSoX9rkxU"
    "yQIDAQAB";

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

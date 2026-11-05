/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "services/network/restricted_cookie_manager.h"

#include "base/feature_list.h"
#include "components/content_settings/core/common/cookie_settings_base.h"
#include "net/base/features.h"
#include "net/cookies/canonical_cookie.h"
#include "net/cookies/cookie_monster.h"
#include "net/cookies/site_for_cookies.h"
#include "services/network/cookie_settings.h"
#include "url/gurl.h"
#include "url/origin.h"

// IsEphemeralCookieAccessible and AnnotateAndMoveUserBlockedCookies declared &
// defined via a chromium_src override for network/cookie_settings.{h,cc}.
#define IsCookieAccessible IsEphemeralCookieAccessible
#define AnnotateAndMoveUserBlockedCookies \
  AnnotateAndMoveUserBlockedEphemeralCookies

// IsEphemeralCookieAccessAllowed declared & defined via a chromium_src override
// for components/content_settings/core/common/cookie_settings_base.{h,cc}.
#define IsFullCookieAccessAllowed IsEphemeralCookieAccessAllowed

// Caps the expiration of cookies set from script (CookieStore and
// document.cookie) to 7 days. canonical_cookie.h is included above so that its
// FromStorage() declaration is not rewritten.
#define FromStorage(NAME, VALUE, DOMAIN, PATH, CREATION, EXPIRY, LAST_ACCESS, \
                    LAST_UPDATE, SECURE, HTTP_ONLY, SAME_SITE, PRIORITY,      \
                    PARTITION, SOURCE_SCHEME, PORT, COOKIE_SOURCE, CALL_SITE) \
  FromStorage(NAME, VALUE, DOMAIN, PATH, CREATION,                            \
              ModifyExpiration(EXPIRY, CREATION), LAST_ACCESS, LAST_UPDATE,   \
              SECURE, HTTP_ONLY, SAME_SITE, PRIORITY, PARTITION,              \
              SOURCE_SCHEME, PORT, COOKIE_SOURCE, CALL_SITE)

#include "src/services/network/restricted_cookie_manager.cc"

#undef FromStorage
#undef IsFullCookieAccessAllowed
#undef AnnotateAndMoveUserBlockedCookies
#undef IsCookieAccessible

namespace {

constexpr base::TimeDelta kMaxCookieExpiration =
    base::Days(7);  // For JS cookies: CookieStore and document.cookie

}  // namespace

namespace network {

base::Time RestrictedCookieManager::ModifyExpiration(
    const base::Time& expiry_date,
    const base::Time& creation_date) const {
  const base::Time max_expiration = creation_date + kMaxCookieExpiration;

  return std::min(expiry_date, max_expiration);
}

net::CookieOptions RestrictedCookieManager::MakeOptionsForSet(
    mojom::RestrictedCookieManagerRole role,
    const GURL& url,
    const net::SiteForCookies& site_for_cookies,
    const url::Origin& top_frame_origin,
    const CookieSettings& cookie_settings) const {
  net::CookieOptions cookie_options = ::network::MakeOptionsForSet(
      role, url, site_for_cookies, top_frame_origin, cookie_settings);
  net::FillEphemeralStorageParams(url, site_for_cookies, top_frame_origin,
                                  cookie_store_->cookie_access_delegate(),
                                  &cookie_options);
  return cookie_options;
}

net::CookieOptions RestrictedCookieManager::MakeOptionsForGet(
    mojom::RestrictedCookieManagerRole role,
    const GURL& url,
    const net::SiteForCookies& site_for_cookies,
    const url::Origin& top_frame_origin,
    const CookieSettings& cookie_settings) const {
  net::CookieOptions cookie_options = ::network::MakeOptionsForGet(
      role, url, site_for_cookies, top_frame_origin, cookie_settings);
  net::FillEphemeralStorageParams(url, site_for_cookies, top_frame_origin,
                                  cookie_store_->cookie_access_delegate(),
                                  &cookie_options);
  return cookie_options;
}

}  // namespace network

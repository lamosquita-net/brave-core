/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <optional>

#include "build/build_config.h"
#include "components/permissions/request_type.h"

#if BUILDFLAG(IS_ANDROID)
#include "components/resources/android/theme_resources.h"
#else
#include "components/vector_icons/vector_icons.h"
#include "ui/gfx/vector_icon_types.h"
#endif

#if BUILDFLAG(IS_ANDROID)
namespace {
constexpr auto kAndroidStorageAccess = IDR_ANDROID_STORAGE_ACCESS;
}  // namespace
#endif

// Add Brave cases into GetIconIdAndroid.
#define IDR_ANDROID_STORAGE_ACCESS                \
  kAndroidStorageAccess;                          \
  case RequestType::kWidevine:                    \
  case RequestType::kBraveGoogleSignInPermission: \
    return IDR_ANDROID_INFOBAR_PERMISSION_COOKIE

// Add Brave cases into GetIconIdDesktop.
#define kStorageAccessIcon                        \
  kStorageAccessIcon;                             \
  case RequestType::kWidevine:                    \
  case RequestType::kBraveGoogleSignInPermission: \
    return vector_icons::kExtensionIcon

#define BRAVE_PERMISSION_KEY_FOR_REQUEST_TYPE                  \
  case permissions::RequestType::kWidevine:                    \
    return "widevine";                                         \
  case permissions::RequestType::kBraveGoogleSignInPermission: \
    return "brave_google_sign_in";

#define ContentSettingsTypeToRequestType \
  ContentSettingsTypeToRequestType_ChromiumImpl

#define RequestTypeToContentSettingsType \
  RequestTypeToContentSettingsType_ChromiumImpl

#define IsRequestablePermissionType IsRequestablePermissionType_ChromiumImpl

#include "src/components/permissions/request_type.cc"

#undef IsRequestablePermissionType
#undef RequestTypeToContentSettingsType
#undef ContentSettingsTypeToRequestType
#undef BRAVE_PERMISSION_KEY_FOR_REQUEST_TYPE
#undef kStorageAccessIcon
#undef IDR_ANDROID_STORAGE_ACCESS

namespace permissions {

RequestType ContentSettingsTypeToRequestType(
    ContentSettingsType content_settings_type) {
  switch (content_settings_type) {
    case ContentSettingsType::BRAVE_GOOGLE_SIGN_IN:
      return RequestType::kBraveGoogleSignInPermission;
    case ContentSettingsType::DEFAULT:
      // Currently we have only one DEFAULT type that is
      // not mapped, which is Widevine, it's used for
      // UMA purpose only
      return RequestType::kWidevine;
    default:
      return ContentSettingsTypeToRequestType_ChromiumImpl(
          content_settings_type);
  }
}

std::optional<ContentSettingsType> RequestTypeToContentSettingsType(
    RequestType request_type) {
  switch (request_type) {
    case RequestType::kBraveGoogleSignInPermission:
      return ContentSettingsType::BRAVE_GOOGLE_SIGN_IN;
    default:
      return RequestTypeToContentSettingsType_ChromiumImpl(request_type);
  }
}

bool IsRequestablePermissionType(ContentSettingsType content_settings_type) {
  switch (content_settings_type) {
    case ContentSettingsType::BRAVE_GOOGLE_SIGN_IN:
      return true;
    default:
      return IsRequestablePermissionType_ChromiumImpl(content_settings_type);
  }
}

}  // namespace permissions

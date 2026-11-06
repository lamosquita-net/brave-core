/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_COMPONENTS_NTP_BACKGROUND_IMAGES_BROWSER_URL_CONSTANTS_H_
#define BRAVE_COMPONENTS_NTP_BACKGROUND_IMAGES_BROWSER_URL_CONSTANTS_H_

namespace ntp_background_images {

inline constexpr char kCustomWallpaperHost[] = "custom-wallpaper";
inline constexpr char kCustomWallpaperURL[] = "chrome://custom-wallpaper/";

inline constexpr char kIsBackgroundKey[] = "isBackground";
inline constexpr char kWallpaperImageURLKey[] = "wallpaperImageUrl";
inline constexpr char kWallpaperColorKey[] = "wallpaperColor";
inline constexpr char kWallpaperTypeKey[] = "type";
inline constexpr char kWallpaperRandomKey[] = "random";

}  // namespace ntp_background_images

#endif  // BRAVE_COMPONENTS_NTP_BACKGROUND_IMAGES_BROWSER_URL_CONSTANTS_H_

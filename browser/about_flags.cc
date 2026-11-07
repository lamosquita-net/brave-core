/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

// This file is included into //chrome/browser/about_flags.cc.

#include <initializer_list>

#include "brave/browser/brave_browser_features.h"
#include "brave/browser/ui/brave_ui_features.h"
#include "brave/browser/ui/tabs/features.h"
#include "brave/components/brave_component_updater/browser/features.h"
#include "brave/components/brave_shields/core/common/features.h"
#include "brave/components/brave_sync/features.h"
#include "brave/components/de_amp/common/features.h"
#include "brave/components/debounce/core/common/features.h"
#include "brave/components/google_sign_in_permission/features.h"
#include "brave/components/request_otr/common/buildflags/buildflags.h"
#include "brave/components/webcompat/features.h"
#include "build/build_config.h"
#include "chrome/browser/ui/ui_features.h"
#include "components/content_settings/core/common/features.h"
#include "components/history/core/browser/features.h"
#include "components/omnibox/common/omnibox_features.h"
#include "components/webui/flags/feature_entry.h"
#include "components/webui/flags/feature_entry_macros.h"
#include "components/webui/flags/flags_state.h"
#include "net/base/features.h"
#include "third_party/blink/public/common/features.h"

#if BUILDFLAG(ENABLE_REQUEST_OTR)
#include "brave/components/request_otr/common/features.h"
#endif

#if BUILDFLAG(IS_ANDROID)
#include "brave/browser/android/preferences/features.h"
#include "brave/browser/android/safe_browsing/features.h"
#else
#include "brave/components/commands/common/features.h"
#endif

#if BUILDFLAG(IS_WIN)
#include "sandbox/policy/features.h"
#endif

#define EXPAND_FEATURE_ENTRIES(...) __VA_ARGS__,

#define REQUEST_OTR_FEATURE_ENTRIES                                           \
  IF_BUILDFLAG(                                                               \
      ENABLE_REQUEST_OTR,                                                     \
      EXPAND_FEATURE_ENTRIES({                                                \
          "brave-request-otr-tab",                                            \
          "Enable Request-OTR Tab",                                           \
          "Suggest going off-the-record when visiting potentially sensitive " \
          "URLs",                                                             \
          kOsDesktop | kOsAndroid,                                            \
          FEATURE_VALUE_TYPE(request_otr::features::kBraveRequestOTRTab),     \
      }))

#define BRAVE_MODULE_FILENAME_PATCH                                            \
  IF_BUILDFLAG(                                                                \
      IS_WIN,                                                                  \
      EXPAND_FEATURE_ENTRIES({                                                 \
          "brave-module-filename-patch",                                       \
          "Enable Module Filename patch",                                      \
          "Enables patching of executable's name from brave.exe to "           \
          "chrome.exe in sandboxed processes.",                                \
          kOsWin,                                                              \
          FEATURE_VALUE_TYPE(sandbox::policy::features::kModuleFileNamePatch), \
      }))

#if !BUILDFLAG(IS_ANDROID)
#define BRAVE_COMMANDS_FEATURE_ENTRIES                                      \
  EXPAND_FEATURE_ENTRIES(                                                   \
      {                                                                     \
          "brave-commands",                                                 \
          "Brave Commands",                                                 \
          "Enable experimental page for viewing and executing commands in " \
          "Brave",                                                          \
          kOsWin | kOsMac | kOsLinux,                                       \
          FEATURE_VALUE_TYPE(commands::features::kBraveCommands),           \
      })
#else
#define BRAVE_COMMANDS_FEATURE_ENTRIES
#endif

#if BUILDFLAG(IS_LINUX)
#define BRAVE_CHANGE_ACTIVE_TAB_ON_SCROLL_EVENT_FEATURE_ENTRIES               \
  EXPAND_FEATURE_ENTRIES({                                                    \
      "brave-change-active-tab-on-scroll-event",                              \
      "Change active tab on scroll event",                                    \
      "Change the active tab when scroll events occur on tab strip.",         \
      kOsLinux,                                                               \
      FEATURE_VALUE_TYPE(tabs::features::kBraveChangeActiveTabOnScrollEvent), \
  })
#else
#define BRAVE_CHANGE_ACTIVE_TAB_ON_SCROLL_EVENT_FEATURE_ENTRIES
#endif

#if BUILDFLAG(IS_ANDROID)
#define BRAVE_BACKGROUND_VIDEO_PLAYBACK_ANDROID                                \
  EXPAND_FEATURE_ENTRIES({                                                     \
      "brave-background-video-playback",                                       \
      "Background video playback",                                             \
      "Enables play audio from video in background when tab is not active or " \
      "device screen is turned off. Try to switch to desktop mode if this "    \
      "feature is not working.",                                               \
      kOsAndroid,                                                              \
      FEATURE_VALUE_TYPE(                                                      \
          preferences::features::kBraveBackgroundVideoPlayback),               \
  })
#define BRAVE_SAFE_BROWSING_ANDROID                                           \
  EXPAND_FEATURE_ENTRIES({                                                    \
      "brave-safe-browsing",                                                  \
      "Safe Browsing",                                                        \
      "Enables Google Safe Browsing for determining whether a URL has been "  \
      "marked as a known threat.",                                            \
      kOsAndroid,                                                             \
      FEATURE_VALUE_TYPE(safe_browsing::features::kBraveAndroidSafeBrowsing), \
  })
#else
#define BRAVE_BACKGROUND_VIDEO_PLAYBACK_ANDROID
#define BRAVE_SAFE_BROWSING_ANDROID
#endif  // BUILDFLAG(IS_ANDROID)

#if !BUILDFLAG(IS_ANDROID)
#define BRAVE_TABS_FEATURE_ENTRIES                                        \
  EXPAND_FEATURE_ENTRIES(                                                 \
      {                                                                   \
          "brave-shared-pinned-tabs",                                     \
          "Shared pinned tab",                                            \
          "Pinned tabs are shared across windows",                        \
          kOsWin | kOsMac | kOsLinux,                                     \
          FEATURE_VALUE_TYPE(tabs::features::kBraveSharedPinnedTabs),     \
      })
#else
#define BRAVE_TABS_FEATURE_ENTRIES
#endif

#if BUILDFLAG(IS_MAC) || BUILDFLAG(IS_LINUX)
#define BRAVE_MIDDLE_CLICK_AUTOSCROLL_FEATURE_ENTRY                      \
  EXPAND_FEATURE_ENTRIES({                                               \
      "middle-button-autoscroll",                                        \
      "Middle button autoscroll",                                        \
      "Enables autoscrolling when the middle mouse button is clicked",   \
      kOsMac | kOsLinux,                                                 \
      FEATURE_VALUE_TYPE(blink::features::kMiddleButtonClickAutoscroll), \
  })
#else
#define BRAVE_MIDDLE_CLICK_AUTOSCROLL_FEATURE_ENTRY
#endif

#define BRAVE_OMNIBOX_FEATURES                                                \
  EXPAND_FEATURE_ENTRIES(                                                     \
      {                                                                       \
          "brave-omnibox-tab-switch-by-default",                              \
          "Brave Tab Switch by Default",                                      \
          "Prefer switching to already open tabs, rather than navigating in " \
          "a "                                                                \
          "new tab",                                                          \
          kOsWin | kOsLinux | kOsMac,                                         \
          FEATURE_VALUE_TYPE(omnibox::kOmniboxTabSwitchByDefault),            \
      },                                                                      \
      {                                                                       \
          "brave-history-more-search-results",                                \
          "Brave More History",                                               \
          "Include more history in the omnibox search results",               \
          kOsWin | kOsLinux | kOsMac | kOsAndroid,                            \
          FEATURE_VALUE_TYPE(history::kHistoryMoreSearchResults),             \
      })

// Keep the last item empty.
#define LAST_BRAVE_FEATURE_ENTRIES_ITEM

#define BRAVE_ABOUT_FLAGS_FEATURE_ENTRIES                                      \
  EXPAND_FEATURE_ENTRIES(                                                      \
      {                                                                        \
          "use-dev-updater-url",                                               \
          "Use dev updater url",                                               \
          "Use the dev url for the component updater. This is for internal "   \
          "testing only.",                                                     \
          kOsAll,                                                              \
          FEATURE_VALUE_TYPE(brave_component_updater::kUseDevUpdaterUrl),      \
      },                                                                       \
      {                                                                        \
          "brave-ntp-search-widget",                                           \
          "Brave Search Widget on the NTP",                                    \
          "Enables searching directly from the New Tab Page",                  \
          kOsDesktop,                                                          \
          FEATURE_VALUE_TYPE(features::kBraveNtpSearchWidget),                 \
      },                                                                       \
      {                                                                        \
          "brave-adblock-cname-uncloaking",                                    \
          "Enable CNAME uncloaking",                                           \
          "Take DNS CNAME records into account when making network request "   \
          "blocking decisions.",                                               \
          kOsAll,                                                              \
          FEATURE_VALUE_TYPE(                                                  \
              brave_shields::features::kBraveAdblockCnameUncloaking),          \
      },                                                                       \
      {                                                                        \
          "brave-adblock-collapse-blocked-elements",                           \
          "Collapse HTML elements with blocked source attributes",             \
          "Cause iframe and img elements to be collapsed if the URL of their " \
          "src attribute is blocked",                                          \
          kOsAll,                                                              \
          FEATURE_VALUE_TYPE(                                                  \
              brave_shields::features::kBraveAdblockCollapseBlockedElements),  \
      },                                                                       \
      {                                                                        \
          "brave-adblock-cookie-list-default",                                 \
          "Treat 'Easylist-Cookie List' as a default list source",             \
          "Enables the 'Easylist-Cookie List' regional list if its toggle in " \
          "flyweb://adblock hasn't otherwise been modified",                    \
          kOsAll,                                                              \
          FEATURE_VALUE_TYPE(                                                  \
              brave_shields::features::kBraveAdblockCookieListDefault),        \
      },                                                                       \
      {                                                                        \
          "brave-adblock-cosmetic-filtering",                                  \
          "Enable cosmetic filtering",                                         \
          "Enable support for cosmetic filtering",                             \
          kOsAll,                                                              \
          FEATURE_VALUE_TYPE(                                                  \
              brave_shields::features::kBraveAdblockCosmeticFiltering),        \
      },                                                                       \
      {                                                                        \
          "brave-adblock-csp-rules",                                           \
          "Enable support for CSP rules",                                      \
          "Applies additional CSP rules to pages for which a $csp rule has "   \
          "been loaded from a filter list",                                    \
          kOsAll,                                                              \
          FEATURE_VALUE_TYPE(brave_shields::features::kBraveAdblockCspRules),  \
      },                                                                       \
      {                                                                        \
          "brave-adblock-default-1p-blocking",                                 \
          "Shields first-party network blocking",                              \
          "Allow Brave Shields to block first-party network requests in "      \
          "Standard blocking mode",                                            \
          kOsAll,                                                              \
          FEATURE_VALUE_TYPE(                                                  \
              brave_shields::features::kBraveAdblockDefault1pBlocking),        \
      },                                                                       \
      {                                                                        \
          "brave-adblock-mobile-notifications-list-default",                   \
          "Treat 'Fanboy's Mobile Notifications List' as a default list "      \
          "source",                                                            \
                                                                               \
          "Enables the 'Fanboy's Mobile Notifications List' regional list if " \
          "its toggle in flyweb://adblock hasn't otherwise been modified",      \
          kOsAll,                                                              \
          FEATURE_VALUE_TYPE(brave_shields::features::                         \
                                 kBraveAdblockMobileNotificationsListDefault), \
      },                                                                       \
      {                                                                        \
          "brave-adblock-experimental-list-default",                           \
          "Treat 'Brave Experimental Adblock Rules' as a default list "        \
          "source",                                                            \
                                                                               \
          "Enables the 'Brave Experimental Adblock Rules' regional list if "   \
          "its toggle in flyweb://adblock hasn't otherwise been modified",      \
          kOsAll,                                                              \
          FEATURE_VALUE_TYPE(                                                  \
              brave_shields::features::kBraveAdblockExperimentalListDefault),  \
      },                                                                       \
      {                                                                        \
          "brave-adblock-scriptlet-debug-logs",                                \
          "Enable debug logging for scriptlet injections",                     \
          "Enable console debugging for scriptlets injected by cosmetic "      \
          "filtering, exposing additional information that can be useful for " \
          "filter authors.",                                                   \
          kOsDesktop,                                                          \
          FEATURE_VALUE_TYPE(                                                  \
              brave_shields::features::kBraveAdblockScriptletDebugLogs),       \
      },                                                                       \
      {                                                                        \
          "brave-dark-mode-block",                                             \
          "Enable dark mode blocking fingerprinting protection",               \
          "Always report light mode when fingerprinting protections set to "   \
          "Strict",                                                            \
          kOsAll,                                                              \
          FEATURE_VALUE_TYPE(brave_shields::features::kBraveDarkModeBlock),    \
      },                                                                       \
      {                                                                        \
          "brave-domain-block",                                                \
          "Enable domain blocking",                                            \
          "Enable support for blocking domains with an interstitial page",     \
          kOsAll,                                                              \
          FEATURE_VALUE_TYPE(brave_shields::features::kBraveDomainBlock),      \
      },                                                                       \
      {                                                                        \
          "brave-domain-block-1pes",                                           \
          "Enable domain blocking using First Party Ephemeral Storage",        \
          "When visiting a blocked domain, Brave will try to enable "          \
          "Ephemeral Storage for a first party context, meaning neither "      \
          "cookies nor localStorage data will be persisted after a website "   \
          "is closed. Ephemeral Storage will be auto-enabled only if no data " \
          "was previously stored for a website",                               \
          kOsAll,                                                              \
          FEATURE_VALUE_TYPE(brave_shields::features::kBraveDomainBlock1PES),  \
      },                                                                       \
      {                                                                        \
          "brave-debounce",                                                    \
          "Enable debouncing",                                                 \
          "Enable support for skipping top-level redirect tracking URLs",      \
          kOsAll,                                                              \
          FEATURE_VALUE_TYPE(debounce::features::kBraveDebounce),              \
      },                                                                       \
      {                                                                        \
          "brave-de-amp",                                                      \
          "Enable De-AMP",                                                     \
          "Enable De-AMPing feature",                                          \
          kOsAll,                                                              \
          FEATURE_VALUE_TYPE(de_amp::features::kBraveDeAMP),                   \
      },                                                                       \
      {                                                                        \
          "brave-google-sign-in-permission",                                   \
          "Enable Google Sign-In Permission Prompt",                           \
          "Enable permissioning access to legacy Google Sign-In",              \
          kOsAll,                                                              \
          FEATURE_VALUE_TYPE(google_sign_in_permission::features::             \
                                 kBraveGoogleSignInPermission),                \
      },                                                                       \
      {                                                                        \
          "brave-extension-network-blocking",                                  \
          "Enable extension network blocking",                                 \
          "Enable blocking for network requests initiated by extensions",      \
          kOsAll,                                                              \
          FEATURE_VALUE_TYPE(                                                  \
              brave_shields::features::kBraveExtensionNetworkBlocking),        \
      },                                                                       \
      {                                                                        \
          "brave-reduce-language",                                             \
          "Reduce language identifiability",                                   \
          "Reduce the identifiability of my language preferences",             \
          kOsAll,                                                              \
          FEATURE_VALUE_TYPE(brave_shields::features::kBraveReduceLanguage),   \
      },                                                                       \
      {                                                                        \
          "brave-cosmetic-filtering-sync-load",                                \
          "Enable sync loading of cosmetic filter rules",                      \
          "Enable sync loading of cosmetic filter rules",                      \
          kOsAll,                                                              \
          FEATURE_VALUE_TYPE(                                                  \
              brave_shields::features::kCosmeticFilteringSyncLoad),            \
      },                                                                       \
      {                                                                        \
          "brave-ephemeral-storage",                                           \
          "Enable Ephemeral Storage",                                          \
          "Use ephemeral storage for third-party frames",                      \
          kOsAll,                                                              \
          FEATURE_VALUE_TYPE(net::features::kBraveEphemeralStorage),           \
      },                                                                       \
      {                                                                        \
          "brave-ephemeral-storage-keep-alive",                                \
          "Ephemeral Storage Keep Alive",                                      \
          "Keep ephemeral storage partitions alive for a specified time "      \
          "after all tabs for that origin are closed",                         \
          kOsAll,                                                              \
          FEATURE_VALUE_TYPE(net::features::kBraveEphemeralStorageKeepAlive),  \
      },                                                                       \
      {                                                                        \
          "brave-first-party-ephemeral-storage",                               \
          "Enable First Party Ephemeral Storage",                              \
          "Enable support for First Party Ephemeral Storage using "            \
          "SESSION_ONLY cookie setting",                                       \
          kOsAll,                                                              \
          FEATURE_VALUE_TYPE(net::features::kBraveFirstPartyEphemeralStorage), \
      },                                                                       \
      {                                                                        \
          "brave-forget-first-party-storage",                                  \
          "Enable First Party Storage Cleanup support",                        \
          "Add cookie blocking mode which allows Brave to cleanup first "      \
          "party storage (Cookies, DOM Storage) on website close",             \
          kOsAll,                                                              \
          FEATURE_VALUE_TYPE(net::features::kBraveForgetFirstPartyStorage),    \
      },                                                                       \
      {                                                                        \
          "brave-sync-v2",                                                     \
          "Enable Brave Sync v2",                                              \
          "Brave Sync v2 integrates with chromium sync engine with Brave "     \
          "specific authentication flow and enforce client side encryption",   \
          kOsDesktop,                                                          \
          FEATURE_VALUE_TYPE(brave_sync::features::kBraveSync),                \
      },                                                                       \
      {                                                                        \
          "file-system-access-api",                                            \
          "File System Access API",                                            \
          "Enables the File System Access API, giving websites access to the " \
          "file system",                                                       \
          kOsDesktop,                                                          \
          FEATURE_VALUE_TYPE(blink::features::kFileSystemAccessAPI),           \
      },                                                                       \
      {                                                                        \
          "brave-web-bluetooth-api",                                           \
          "Web Bluetooth API",                                                 \
          "Enables the Web Bluetooth API, giving websites access to "          \
          "Bluetooth devices",                                                 \
          kOsAll,                                                              \
          FEATURE_VALUE_TYPE(blink::features::kBraveWebBluetoothAPI),          \
      },                                                                       \
      {                                                                        \
          "navigator-connection-attribute",                                    \
          "Enable navigator.connection attribute",                             \
          "Enables the navigator.connection API. Enabling this API will "      \
          "allow sites to learn information about your network and internet "  \
          "connection. Trackers can use this information to fingerprint your " \
          "browser, or to infer when you are traveling or at home.",           \
          kOsAll,                                                              \
          FEATURE_VALUE_TYPE(blink::features::kNavigatorConnectionAttribute),  \
      },                                                                       \
      {                                                                        \
          "restrict-websockets-pool",                                          \
          "Restrict WebSockets pool",                                          \
          "Limits simultaneous active WebSockets connections per eTLD+1",      \
          kOsAll,                                                              \
          FEATURE_VALUE_TYPE(blink::features::kRestrictWebSocketsPool),        \
      },                                                                       \
      {                                                                        \
          "allow-incognito-permission-inheritance",                            \
          "Allow permission inheritance in incognito profiles",                \
          "When enabled, most permissions set in a normal profile will be "    \
          "inherited in incognito profile if they are less permissive, for "   \
          "ex. Geolocation BLOCK will be automatically set to BLOCK in "       \
          "incognito.",                                                        \
          kOsAll,                                                              \
          FEATURE_VALUE_TYPE(                                                  \
              content_settings::kAllowIncognitoPermissionInheritance),         \
      },                                                                       \
      {                                                                        \
          "brave-block-screen-fingerprinting",                                 \
          "Block screen fingerprinting",                                       \
          "Prevents JavaScript and CSS from learning the user's screen "       \
          "dimensions or window position.",                                    \
          kOsAll,                                                              \
          FEATURE_VALUE_TYPE(                                                  \
              blink::features::kBraveBlockScreenFingerprinting),               \
      },                                                                       \
      {                                                                        \
          "brave-webgl-balanced-fingerprinting-protections",                   \
          "Add more protections for graphics in balanced fingerprinting",      \
          "Adds extra protections when sites asks for device specific "        \
          "information from the graphics APIs. It also adds noise to APIs "    \
          "that enumerates webgl extensions which can be used to fingerprint " \
          "users across sites.",                                               \
          kOsDesktop | kOsAndroid,                                             \
          FEATURE_VALUE_TYPE(                                                  \
              blink::features::kWebGLBalancedFingerprintingProtection),        \
      },                                                                       \
      {                                                                        \
          "brave-round-time-stamps",                                           \
          "Round time stamps",                                                 \
          "Prevents JavaScript from getting access to high-resolution clocks " \
          "by rounding all DOMHighResTimeStamps to the nearest millisecond.",  \
          kOsAll,                                                              \
          FEATURE_VALUE_TYPE(blink::features::kBraveRoundTimeStamps),          \
      },                                                                       \
      {                                                                        \
          "restrict-event-source-pool",                                        \
          "Restrict Event Source Pool",                                        \
          "Limits simultaneous active WebSockets connections per eTLD+1",      \
          kOsAll,                                                              \
          FEATURE_VALUE_TYPE(blink::features::kRestrictEventSourcePool),       \
      },                                                                       \
      {                                                                        \
          "brave-copy-clean-link-by-default",                                  \
          "Override default copy hotkey with copy clean link",                 \
          "Sanitize url before copying, replaces default ctrl+c hotkey for "   \
          "url ",                                                              \
          kOsWin | kOsLinux | kOsMac,                                          \
          FEATURE_VALUE_TYPE(features::kBraveCopyCleanLinkByDefault),          \
      },                                                                       \
      {                                                                        \
          "brave-global-privacy-control-enabled",                              \
          "Enable Global Privacy Control",                                     \
          "Enable the Sec-GPC request header and the "                         \
          "navigator.globalPrivacyControl JS API",                             \
          kOsAll,                                                              \
          FEATURE_VALUE_TYPE(blink::features::kBraveGlobalPrivacyControl),     \
      },                                                                       \
      {                                                                        \
          "https-by-default",                                                  \
          "Use HTTPS by Default",                                              \
          "Attempt to connect to all websites using HTTPS before falling "     \
          "back to HTTP.",                                                     \
          kOsAll,                                                              \
          FEATURE_VALUE_TYPE(net::features::kBraveHttpsByDefault),             \
      },                                                                       \
      {                                                                        \
          "fallback-dns-over-https",                                           \
          "Use a fallback DoH provider",                                       \
          "In Automatic DoH mode, use a fallback DoH provider if the current " \
          "provider doesn't offer Secure DNS.",                                \
          kOsAll,                                                              \
          FEATURE_VALUE_TYPE(net::features::kBraveFallbackDoHProvider),        \
      },                                                                       \
      {                                                                        \
          "brave-show-strict-fingerprinting-mode",                             \
          "Show Strict Fingerprinting Mode",                                   \
          "Show Strict (aggressive) option for Fingerprinting Mode in "        \
          "Brave Shields ",                                                    \
          kOsAll,                                                              \
          FEATURE_VALUE_TYPE(                                                  \
              brave_shields::features::kBraveShowStrictFingerprintingMode),    \
      },                                                                       \
      {                                                                        \
          "brave-override-download-danger-level",                              \
          "Override download danger level",                                    \
          "Disables download warnings for files which are considered "         \
          "dangerous when Safe Browsing is disabled. Use at your own risks. "  \
          "Not recommended.",                                                  \
          kOsWin | kOsLinux | kOsMac,                                          \
          FEATURE_VALUE_TYPE(features::kBraveOverrideDownloadDangerLevel),     \
      },                                                                       \
      {                                                                        \
          "brave-webcompat-exceptions-service",                                \
          "Allow feature exceptions for webcompat",                            \
          "Disables Brave features for specific websites when they break"      \
          "website functionality.",                                            \
          kOsAll,                                                              \
          FEATURE_VALUE_TYPE(                                                  \
              webcompat::features::kBraveWebcompatExceptionsService),          \
      })                                                                       \
  REQUEST_OTR_FEATURE_ENTRIES                                                  \
  BRAVE_MODULE_FILENAME_PATCH                                                  \
  BRAVE_COMMANDS_FEATURE_ENTRIES                                               \
  BRAVE_BACKGROUND_VIDEO_PLAYBACK_ANDROID                                      \
  BRAVE_SAFE_BROWSING_ANDROID                                                  \
  BRAVE_CHANGE_ACTIVE_TAB_ON_SCROLL_EVENT_FEATURE_ENTRIES                      \
  BRAVE_TABS_FEATURE_ENTRIES                                                   \
  BRAVE_OMNIBOX_FEATURES                                                       \
  BRAVE_MIDDLE_CLICK_AUTOSCROLL_FEATURE_ENTRY                                  \
  LAST_BRAVE_FEATURE_ENTRIES_ITEM  // Keep it as the last item.
namespace flags_ui {
namespace {

// Unused function to reference Brave feature entries for clang checks.
[[maybe_unused]] void UseBraveAboutFlags() {
  // These vars are declared in anonymous namespace in
  // //chrome/browser/about_flags.cc. We declare them here manually to
  // instantiate BRAVE_ABOUT_FLAGS_FEATURE_ENTRIES without errors.
  constexpr int kOsAll = 0;
  constexpr int kOsDesktop = 0;

  static_assert(
      std::initializer_list<FeatureEntry>{BRAVE_ABOUT_FLAGS_FEATURE_ENTRIES}
          .size());
}

}  // namespace
}  // namespace flags_ui

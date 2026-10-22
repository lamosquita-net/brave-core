/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_CHROMIUM_SRC_CHROME_COMMON_URL_CONSTANTS_H_
#define BRAVE_CHROMIUM_SRC_CHROME_COMMON_URL_CONSTANTS_H_

#include <stddef.h>

#include "build/branding_buildflags.h"
#include "build/build_config.h"
#include "chrome/common/webui_url_constants.h"
#include "net/net_buildflags.h"
#include "ppapi/buildflags/buildflags.h"

namespace chrome {

// "Learn more" URL linked in the dialog to cast using a code.
inline constexpr char kAccessCodeCastLearnMoreURL[] =
    "https://flyweb.lamosquita.net/ayuda/";

// "Learn more" URL for accessibility image labels, linked from the permissions
// dialog shown when a user enables the feature.
inline constexpr char kAccessibilityLabelsLearnMoreURL[] =
    "https://flyweb.lamosquita.net/ayuda/";

// "Learn more" URL for Ad Privacy.
inline constexpr char kAdPrivacyLearnMoreURL[] =
    "https://flyweb.lamosquita.net/ayuda/#privacidad";

// "Learn more" URL for when profile settings are automatically reset.
inline constexpr char kAutomaticSettingsResetLearnMoreURL[] =
    "https://flyweb.lamosquita.net/ayuda/#restablecer";

// "Learn more" URL for Advanced Protection download warnings.
inline constexpr char kAdvancedProtectionDownloadLearnMoreURL[] =
    "https://flyweb.lamosquita.net/ayuda/#descargas";

// "Chrome Settings" URL for the appearance page.
inline constexpr char kBrowserSettingsSearchEngineURL[] =
    "chrome://settings/search";

// "Learn more" URL for Battery Saver Mode.
inline constexpr char kBatterySaverModeLearnMoreUrl[] =
    "https://flyweb.lamosquita.net/ayuda/#rendimiento";

// The URL for providing help when the Bluetooth adapter is off.
inline constexpr char kBluetoothAdapterOffHelpURL[] =
    "https://flyweb.lamosquita.net/ayuda/";

// "Learn more" URL shown in the dialog to enable cloud services for Cast.
inline constexpr char kCastCloudServicesHelpURL[] =
    "https://flyweb.lamosquita.net/ayuda/";

// The URL for the help center article to show when no Cast destination has been
// found.
inline constexpr char kCastNoDestinationFoundURL[] =
    "https://flyweb.lamosquita.net/ayuda/";

// The URL for the WebHID API help center article.
inline constexpr char kChooserHidOverviewUrl[] =
    "https://flyweb.lamosquita.net/ayuda/#permisos";

// The URL for the Web Serial API help center article.
inline constexpr char kChooserSerialOverviewUrl[] =
    "https://flyweb.lamosquita.net/ayuda/#permisos";

// The URL for the WebUsb help center article.
inline constexpr char kChooserUsbOverviewURL[] =
    "https://flyweb.lamosquita.net/ayuda/#permisos";

// Link to the forum for Chrome Beta.
inline constexpr char kChromeBetaForumURL[] =
    "https://flyweb.lamosquita.net/ayuda/";

// The URL for the help center article to fix Chrome update problems.
inline constexpr char16_t kChromeFixUpdateProblems[] =
    u"https://flyweb.lamosquita.net/ayuda/#actualizaciones";

// General help links for Chrome, opened using various actions.
inline constexpr char kChromeHelpViaKeyboardURL[] =
    "https://flyweb.lamosquita.net/ayuda/";

inline constexpr char kChromeHelpViaMenuURL[] =
    "https://flyweb.lamosquita.net/ayuda/";

inline constexpr char kChromeHelpViaWebUIURL[] =
    "https://flyweb.lamosquita.net/ayuda/";

inline constexpr char kFirstPartySetsLearnMoreURL[] =
    "https://flyweb.lamosquita.net/ayuda/#permisos";

// The isolated-app: scheme is used for Isolated Web Apps. A public explainer
// can be found here: https://github.com/reillyeon/isolated-web-apps
inline constexpr char kIsolatedAppScheme[] = "isolated-app";
inline constexpr char16_t kIsolatedAppSchemeUtf16[] = u"isolated-app";

// The chrome-native: scheme is used show pages rendered with platform specific
// widgets instead of using HTML.
inline constexpr char kChromeNativeScheme[] = "chrome-native";

// The URL of safe section in Chrome page.
inline constexpr char16_t kChromeSafePageURL[] =
    u"https://flyweb.lamosquita.net/ayuda/#seguridad";

// Pages under chrome-search.
inline constexpr char kChromeSearchLocalNtpHost[] = "local-ntp";

// Host and URL for most visited iframes used on the Instant Extended NTP.
inline constexpr char kChromeSearchMostVisitedHost[] = "most-visited";
inline constexpr char kChromeSearchMostVisitedUrl[] =
    "chrome-search://most-visited/";

// URL for NTP custom background image selected from the user's machine and
// filename for the version of the file in the Profile directory
inline constexpr char kChromeUIUntrustedNewTabPageBackgroundUrl[] =
    "chrome-untrusted://new-tab-page/background.jpg";
inline constexpr char kChromeUIUntrustedNewTabPageBackgroundFilename[] =
    "background.jpg";

// Page under chrome-search.
inline constexpr char kChromeSearchRemoteNtpHost[] = "remote-ntp";

// The chrome-search: scheme is served by the same backend as chrome:.  However,
// only specific URLDataSources are enabled to serve requests via the
// chrome-search: scheme.  See |InstantIOContext::ShouldServiceRequest| and its
// callers for details.  Note that WebUIBindings should never be granted to
// chrome-search: pages.  chrome-search: pages are displayable but not readable
// by external search providers (that are rendered by Instant renderer
// processes), and neither displayable nor readable by normal (non-Instant) web
// pages.  To summarize, a non-Instant process, when trying to access
// 'chrome-search://something', will bump up against the following:
//
//  1. Renderer: The display-isolated check in WebKit will deny the request,
//  2. Browser: Assuming they got by #1, the scheme checks in
//     URLDataSource::ShouldServiceRequest will deny the request,
//  3. Browser: for specific sub-classes of URLDataSource, like ThemeSource
//     there are additional Instant-PID checks that make sure the request is
//     coming from a blessed Instant process, and deny the request.
inline constexpr char kChromeSearchScheme[] = "chrome-search";

// This is the base URL of content that can be embedded in chrome://new-tab-page
// using an <iframe>. The embedded untrusted content can make web requests and
// can include content that is from an external source.
inline constexpr char kChromeUIUntrustedNewTabPageUrl[] =
    "chrome-untrusted://new-tab-page/";

// The URL for the Chromium project used in the About dialog.
inline constexpr char16_t kChromiumProjectURL[] = u"https://www.chromium.org/";

inline constexpr char16_t kContentSettingsExceptionsLearnMoreURL[] =
    u"https://flyweb.lamosquita.net/ayuda/#permisos";

// "Learn more" URL for cookies.
inline constexpr char kCookiesSettingsHelpCenterURL[] =
    "https://flyweb.lamosquita.net/ayuda/#permisos";

// "Learn more" URL for "Aw snap" page when showing "Reload" button.
inline constexpr char kCrashReasonURL[] =
    "https://flyweb.lamosquita.net/ayuda/#fallos";

// "Learn more" URL for "Aw snap" page when showing "Send feedback" button.
inline constexpr char kCrashReasonFeedbackDisplayedURL[] =
    "https://flyweb.lamosquita.net/ayuda/#fallos";

// "Learn more" URL for the inactive tabs appearance setting.
inline constexpr char16_t kDiscardRingTreatmentLearnMoreUrl[] =
    u"https://flyweb.lamosquita.net/ayuda/#rendimiento";

// "Learn more" URL for the "Do not track" setting in the privacy section.
inline constexpr char16_t kDoNotTrackLearnMoreURL[] =
    u"https://flyweb.lamosquita.net/ayuda/#privacidad";

// The URL for the "Learn more" page for interrupted downloads.
inline constexpr char kDownloadInterruptedLearnMoreURL[] =
    "https://flyweb.lamosquita.net/ayuda/#descargas";

// The URL for the "Learn more" page for download scanning.
inline constexpr char kDownloadScanningLearnMoreURL[] =
    "https://flyweb.lamosquita.net/ayuda/#descargas";

// The URL for the "Learn more" page for blocked downloads.
// Note: This is the same as the above URL. This is done to decouple the URLs,
// in case the support page is split apart into separate pages in the future.
inline constexpr char kDownloadBlockedLearnMoreURL[] =
    "https://flyweb.lamosquita.net/ayuda/#descargas";

// "Learn more" URL for the Settings API, NTP bubble and other settings bubbles
// showing which extension is controlling them.
inline constexpr char kExtensionControlledSettingLearnMoreURL[] =
    "https://flyweb.lamosquita.net/ayuda/#extensiones";

// URL used to indicate that an extension resource load request was invalid.
inline constexpr char kExtensionInvalidRequestURL[] =
    "chrome-extension://invalid/";

// Link for creating family group with Google Families.
inline constexpr char16_t kFamilyGroupCreateURL[] =
    u"https://flyweb.lamosquita.net/ayuda/";

// Link for viewing family group with Google Families.
inline constexpr char16_t kFamilyGroupViewURL[] =
    u"https://flyweb.lamosquita.net/ayuda/";

// Url to a blogpost about Flash deprecation.
inline constexpr char kFlashDeprecationLearnMoreURL[] =
    "https://blog.chromium.org/2017/07/so-long-and-thanks-for-all-flash.html";

// URL of the 'Activity controls' section of the privacy settings page.
inline constexpr char kGoogleAccountActivityControlsURL[] =
    "https://flyweb.lamosquita.net/ayuda/#privacidad";

// URL of the 'Activity controls' section of the privacy settings page, with
// privacy guide parameters and a link for users to manage data.
inline constexpr char kGoogleAccountActivityControlsURLInPrivacyGuide[] =
    "https://flyweb.lamosquita.net/ayuda/#privacidad";

// URL of the 'Linked services' section of the privacy settings page.
inline constexpr char kGoogleAccountLinkedServicesURL[] =
    "https://flyweb.lamosquita.net/ayuda/";

// URL of the Google Account.
inline constexpr char kGoogleAccountURL[] =
    "https://flyweb.lamosquita.net/ayuda/";

// URL of the Google Account chooser.
inline constexpr char kGoogleAccountChooserURL[] =
    "https://flyweb.lamosquita.net/ayuda/";

// URL of the Google Account page showing the known user devices.
inline constexpr char kGoogleAccountDeviceActivityURL[] =
    "https://flyweb.lamosquita.net/ayuda/";

// URL of the Google Password Manager.
inline constexpr char kGooglePasswordManagerURL[] =
    "https://flyweb.lamosquita.net/ayuda/#contrasenas";

// The URL for the "Learn more" link for the High Efficiency Mode.
inline constexpr char kMemorySaverModeLearnMoreUrl[] =
    "https://flyweb.lamosquita.net/ayuda/#rendimiento";

// The URL in the help text for the High Efficiency Mode tab discarding
// exceptions add dialog.
inline constexpr char16_t kMemorySaverModeTabDiscardingHelpUrl[] =
    u"https://flyweb.lamosquita.net/ayuda/#rendimiento";

// The URL to the help center article of Incognito mode.
inline constexpr char16_t kIncognitoHelpCenterURL[] =
    u"https://flyweb.lamosquita.net/ayuda/#privacidad";

// The URL for the Help Center page about IP Protection.
inline constexpr char kIpProtectionHelpCenterURL[] =
    "https://support.google.com/chrome?p=ip_protection";

// The URL for the "Learn more" page for the usage/crash reporting option in the
// first run dialog.
inline constexpr char kLearnMoreReportingURL[] =
    "https://flyweb.lamosquita.net/ayuda/#privacidad";

// The URL for the Help Center page about managing third-party cookies.
inline constexpr char kManage3pcHelpCenterURL[] =
    "https://flyweb.lamosquita.net/ayuda/#privacidad";

// The URL for the tab group sync help center page.
inline constexpr char kTabGroupsLearnMoreURL[] =
    "https://flyweb.lamosquita.net/ayuda/";

// The URL for the Learn More page about policies and enterprise enrollment.
inline constexpr char16_t kManagedUiLearnMoreUrl[] =
    u"https://flyweb.lamosquita.net/ayuda/";

// The URL for the "Learn more" page for insecure download blocking.
inline constexpr char kInsecureDownloadBlockingLearnMoreUrl[] =
    "https://flyweb.lamosquita.net/ayuda/#descargas";

// "myactivity.google.com" URL for the history checkbox in ClearBrowsingData.
inline constexpr char16_t kMyActivityUrlInClearBrowsingData[] =
    u"https://flyweb.lamosquita.net/ayuda/#privacidad";

// Help URL for the Omnibox setting.
inline constexpr char16_t kOmniboxLearnMoreURL[] =
    u"https://flyweb.lamosquita.net/ayuda/#buscador";

// "What do these mean?" URL for the Page Info bubble.
inline constexpr char kPageInfoHelpCenterURL[] =
    "https://flyweb.lamosquita.net/ayuda/#seguridad";

// Help URL for the bulk password check.
inline constexpr char kPasswordCheckLearnMoreURL[] =
    "https://flyweb.lamosquita.net/ayuda/#contrasenas";

// Help URL for password generation.
inline constexpr char kPasswordGenerationLearnMoreURL[] =
    "https://flyweb.lamosquita.net/ayuda/#contrasenas";

inline constexpr char16_t kPasswordManagerLearnMoreURL[] =
    u"https://flyweb.lamosquita.net/ayuda/#contrasenas";

// Help URL for passwords import.
inline constexpr char kPasswordManagerImportLearnMoreURL[] =
    "https://flyweb.lamosquita.net/ayuda/#contrasenas";

// Help URL for password sharing.
inline constexpr char kPasswordSharingLearnMoreURL[] =
    "https://flyweb.lamosquita.net/ayuda/#contrasenas";

// Help URL for troubleshooting password sharing.
inline constexpr char kPasswordSharingTroubleshootURL[] =
    "https://flyweb.lamosquita.net/ayuda/#contrasenas";

// Help URL for the Payment methods page of the Google Pay site.
inline constexpr char16_t kPaymentMethodsURL[] =
    u"https://flyweb.lamosquita.net/ayuda/#autorrelleno";

// Help URL for the newer GPay Web site instead of the legacy Payments Center.
inline constexpr char16_t kPaymentMethodsURLForGPayWeb[] =
    u"https://flyweb.lamosquita.net/ayuda/#autorrelleno";

// The URL for the "Fill out forms automatically" support page.
inline constexpr char kAddressesAndPaymentMethodsLearnMoreURL[] =
    "https://flyweb.lamosquita.net/ayuda/#autorrelleno";

// "Learn more" URL for the performance intervention notification setting.
inline constexpr char16_t kPerformanceInterventionLearnMoreUrl[] =
    u"https://flyweb.lamosquita.net/ayuda/#rendimiento";

// "Learn more" URL for the preloading section in Performance settings.
inline constexpr char kPreloadingLearnMoreUrl[] =
    "https://flyweb.lamosquita.net/ayuda/#privacidad";

// "Learn more" URL for the Privacy section under Options.
inline constexpr char kPrivacyLearnMoreURL[] =
    "https://flyweb.lamosquita.net/ayuda/#privacidad";

// The URL for the Learn More link of the non-CWS bubble.
inline constexpr char kRemoveNonCWSExtensionURL[] =
    "https://flyweb.lamosquita.net/ayuda/#extensiones";

// "Learn more" URL for resetting profile preferences.
inline constexpr char kResetProfileSettingsLearnMoreURL[] =
    "https://flyweb.lamosquita.net/ayuda/#restablecer";

// "Learn more" URL for Safebrowsing
inline constexpr char kSafeBrowsingHelpCenterURL[] =
    "https://flyweb.lamosquita.net/ayuda/#seguridad";

// Updated "Info icon" URL for Safebrowsing
inline constexpr char kSafeBrowsingHelpCenterUpdatedURL[] =
    "https://flyweb.lamosquita.net/ayuda/#seguridad";

// "Learn more" URL for Enhanced Protection
inline constexpr char16_t kSafeBrowsingInChromeHelpCenterURL[] =
    u"https://flyweb.lamosquita.net/ayuda/#seguridad";

// The URL of Safe Browsing p-tour.
inline constexpr char16_t kSafeBrowsingUseInChromeURL[] =
    u"https://flyweb.lamosquita.net/ayuda/#seguridad";

// "Learn more" URL for Safety Check page.
inline constexpr char16_t kSafetyHubHelpCenterURL[] =
    u"https://flyweb.lamosquita.net/ayuda/#seguridad";

// "Learn more" URL for safety tip bubble.
inline constexpr char kSafetyTipHelpCenterURL[] =
    "https://flyweb.lamosquita.net/ayuda/#seguridad";

// Google search history URL that leads users of the CBD dialog to their search
// history in their Google account.
inline constexpr char16_t kSearchHistoryUrlInClearBrowsingData[] =
    u"https://flyweb.lamosquita.net/ayuda/#privacidad";

// The URL for the "See more security tips" with advices how to create a strong
// password.
inline constexpr char kSeeMoreSecurityTipsURL[] =
    "https://flyweb.lamosquita.net/ayuda/#seguridad";

// Help URL for the settings page's search feature.
inline constexpr char16_t kSettingsSearchHelpURL[] =
    u"https://flyweb.lamosquita.net/ayuda/#buscador";

// The URL for the Learn More page about Sync and Google services.
inline constexpr char kSyncAndGoogleServicesLearnMoreURL[] =
    "https://flyweb.lamosquita.net/ayuda/sincronizar";

// The URL for the "Learn more" page on sync encryption.
inline constexpr char16_t kSyncEncryptionHelpURL[] =
    u"https://flyweb.lamosquita.net/ayuda/sincronizar";

// The URL for the "Learn more" link when there is a sync error.
inline constexpr char kSyncErrorsHelpURL[] =
    "https://flyweb.lamosquita.net/ayuda/sincronizar";

inline constexpr char kSyncGoogleDashboardURL[] =
    "https://flyweb.lamosquita.net/ayuda/sincronizar";

// The URL for the "Learn more" page for sync setup on the personal stuff page.
inline constexpr char16_t kSyncLearnMoreURL[] =
    u"https://flyweb.lamosquita.net/ayuda/sincronizar";

// The URL for the "Learn more" page for Help me Write.
inline constexpr char kComposeLearnMorePageURL[] =
    "https://flyweb.lamosquita.net/ayuda/";

// The URL for the "Learn more" links for pages related to History search.
// TODO(crbug.com/328300718): Update help article URL.
inline constexpr char kHistorySearchLearnMorePageURL[] =
    "https://flyweb.lamosquita.net/ayuda/";

// The URL for the Settings page to enable history search.
inline constexpr char16_t kHistorySearchSettingURL[] =
    u"chrome://settings/historySearch";

// The URL for the "Learn more" link in the enterprise disclaimer for managed
// profile in the Signin Intercept bubble.
inline constexpr char kSigninInterceptManagedDisclaimerLearnMoreURL[] =
    "https://flyweb.lamosquita.net/ayuda/";

#if !BUILDFLAG(IS_ANDROID)
// The URL for the trusted vault sync passphrase opt in.
inline constexpr char kSyncTrustedVaultOptInURL[] =
    "https://flyweb.lamosquita.net/ayuda/sincronizar";
#endif

// The URL for the "Learn more" link for the trusted vault sync passphrase.
inline constexpr char kSyncTrustedVaultLearnMoreURL[] =
    "https://flyweb.lamosquita.net/ayuda/sincronizar";

// The URL for the Help Center page about Tracking Protection settings.
inline constexpr char16_t kTrackingProtectionHelpCenterURL[] =
    u"https://flyweb.lamosquita.net/ayuda/#privacidad";

// The URL for the Help Center page about User Bypass.
inline constexpr char16_t kUserBypassHelpCenterURL[] =
    u"https://flyweb.lamosquita.net/ayuda/#privacidad";

inline constexpr char kUpgradeHelpCenterBaseURL[] =
    "https://flyweb.lamosquita.net/ayuda/#actualizaciones";

// Help center URL for who the account administrator is.
inline constexpr char16_t kWhoIsMyAdministratorHelpURL[] =
    u"https://flyweb.lamosquita.net/ayuda/";

// The URL for the "Learn more" link about CWS Enhanced Safe Browsing.
inline constexpr char16_t kCwsEnhancedSafeBrowsingLearnMoreURL[] =
    u"https://flyweb.lamosquita.net/ayuda/#seguridad";

#if BUILDFLAG(IS_ANDROID)
// "Learn more" URL for the enhanced playback notification dialog.
inline constexpr char kEnhancedPlaybackNotificationLearnMoreURL[] =
    // Keep in sync with chrome/android/java/strings/android_chrome_strings.grd
    "https://community.brave.com";
#endif

#if BUILDFLAG(IS_MAC)
// "Learn more" URL for the enterprise sign-in confirmation dialog.
inline constexpr char kChromeEnterpriseSignInLearnMoreURL[] =
    "https://flyweb.lamosquita.net/ayuda/";

// The URL for the "learn more" link on the macOS version obsolescence infobar.
inline constexpr char kMacOsObsoleteURL[] =
    "https://flyweb.lamosquita.net/ayuda/#actualizaciones";
#endif

#if BUILDFLAG(IS_WIN)
// The URL for the Windows XP/Vista deprecation help center article.
inline constexpr char kWindowsXPVistaDeprecationURL[] =
    "https://flyweb.lamosquita.net/ayuda/";

// The URL for the Windows 7/8.1 deprecation help center article.
inline constexpr char kWindows78DeprecationURL[] =
    "https://flyweb.lamosquita.net/ayuda/";
#endif  // BUILDFLAG(IS_WIN)

// "Learn more" URL for the one click signin infobar.
inline constexpr char kChromeSyncLearnMoreURL[] =
    "https://flyweb.lamosquita.net/ayuda/sincronizar";

#if BUILDFLAG(ENABLE_PLUGINS)
// The URL for the "Learn more" page for the outdated plugin infobar.
inline constexpr char kOutdatedPluginLearnMoreURL[] =
    "https://flyweb.lamosquita.net/ayuda/";
#endif

#if BUILDFLAG(IS_WIN) || BUILDFLAG(IS_MAC) || BUILDFLAG(IS_LINUX)
// "Learn more" URL for the chrome apps deprecation dialog.
inline constexpr char kChromeAppsDeprecationLearnMoreURL[] =
    "https://support.google.com/chrome/?p=chrome_app_deprecation";
#endif

#if BUILDFLAG(CHROME_ROOT_STORE_SUPPORTED)
// TODO(b/1339340): add help center link when help center link is created.
inline constexpr char kChromeRootStoreSettingsHelpCenterURL[] =
    "https://chromium.googlesource.com/chromium/src/+/main/net/data/ssl/"
    "chrome_root_store/root_store.md";
#endif

}  // namespace chrome

#endif  // BRAVE_CHROMIUM_SRC_CHROME_COMMON_URL_CONSTANTS_H_

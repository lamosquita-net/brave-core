/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

// Contains constants for known URLs and portions thereof.
// Except for WebUI UI/Host/SubPage constants. Those go in
// chrome/common/webui_url_constants.h.
//
// - The constants are divided into sections: Cross platform, platform-specific,
//   and feature-specific.
// - When adding platform/feature specific constants, if there already exists an
//   appropriate #if block, use that.
// - Keep the constants sorted by name within its section.

#ifndef BRAVE_CHROMIUM_SRC_CHROME_COMMON_URL_CONSTANTS_H_
#define BRAVE_CHROMIUM_SRC_CHROME_COMMON_URL_CONSTANTS_H_

#include "build/branding_buildflags.h"
#include "build/build_config.h"
#include "chrome/common/buildflags.h"
#include "chrome/common/webui_url_constants.h"
#include "content/public/common/url_constants.h"
#include "net/net_buildflags.h"

#if BUILDFLAG(IS_CHROMEOS)
#include "ash/constants/chrome_url_constants.h"
#endif  // BUILDFLAG(IS_CHROMEOS)

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
inline constexpr char16_t kBatterySaverModeLearnMoreUrl[] =
    u"https://flyweb.lamosquita.net/ayuda/#rendimiento";

// The URL for providing help when the Bluetooth adapter is off.
inline constexpr char kBluetoothAdapterOffHelpURL[] =
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

// General help links for Chrome, opened using various actions.
inline constexpr char kChromeHelpViaKeyboardURL[] =
    "https://flyweb.lamosquita.net/ayuda/";

inline constexpr char kChromeHelpViaMenuURL[] =
    "https://flyweb.lamosquita.net/ayuda/";

inline constexpr char kChromeHelpViaWebUIURL[] =
    "https://flyweb.lamosquita.net/ayuda/";
#if BUILDFLAG(IS_CHROMEOS)
inline constexpr char kChromeOsHelpViaWebUIURL[] =
    "https://flyweb.lamosquita.net/ayuda/";
#endif  // BUILDFLAG(IS_CHROMEOS)

// The chrome-native: scheme is used show pages rendered with platform specific
// widgets instead of using HTML.
inline constexpr char kChromeNativeScheme[] = "chrome-native";

// The chrome-experimental-site-token-provider: scheme is used to retrieve
// site-specific tokens.
inline constexpr char kChromeExperimentalSiteTokenProviderScheme[] =
    "chrome-experimental-site-token-provider";
inline constexpr char kChromeExperimentalSiteTokenHost[] = "token";

// The URL of safe section in Chrome page (https://www.google.com/chrome).
inline constexpr char16_t kChromeSafePageURL[] =
    u"https://flyweb.lamosquita.net/ayuda/#seguridad";

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
#if BUILDFLAG(IS_CHROMEOS)
static_assert(std::u16string_view(kChromiumProjectURL) ==
              ash::chrome_external_urls::kChromiumProjectURL);
#endif

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

// "Learn more" URL for Universal Opt-Out.
inline constexpr char16_t kUniversalOptOutLearnMoreURL[] =
    u"https://flyweb.lamosquita.net/ayuda/";

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

// Link for creating family group with Google Families.
inline constexpr char16_t kFamilyGroupCreateURL[] =
    u"https://flyweb.lamosquita.net/ayuda/";

// Link for viewing family group with Google Families.
inline constexpr char16_t kFamilyGroupViewURL[] =
    u"https://flyweb.lamosquita.net/ayuda/";

// "Learn more" URL for related website sets.
inline constexpr char kRelatedWebsiteSetsLearnMoreURL[] =
    "https://flyweb.lamosquita.net/ayuda/";

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

// URL of the Google Account home address page.
inline constexpr char kGoogleAccountHomeAddressURL[] =
    "https://flyweb.lamosquita.net/ayuda/";

// URL of the Google Account work address page.
inline constexpr char kGoogleAccountWorkAddressURL[] =
    "https://flyweb.lamosquita.net/ayuda/";

// URL of the change Google Account name page.
inline constexpr char kGoogleAccountNameEmailAddressEditURL[] =
    "https://flyweb.lamosquita.net/ayuda/";

// URL of the two factor authentication setup required intersitial.
inline constexpr char kGoogleTwoFactorIntersitialURL[] =
    "https://flyweb.lamosquita.net/ayuda/";

// URL of the Google Password Manager.
inline constexpr char kGooglePasswordManagerURL[] =
    "https://flyweb.lamosquita.net/ayuda/#contrasenas";

// URL of the Google Photos.
inline constexpr char kGooglePhotosURL[] = "https://photos.google.com";

// The URL for the "Learn more" link for the Memory Saver Mode.
inline constexpr char16_t kMemorySaverModeLearnMoreUrl[] =
    u"https://flyweb.lamosquita.net/ayuda/#rendimiento";

// The URL in the help text for the Memory Saver Mode tab discarding
// exceptions add dialog.
inline constexpr char16_t kMemorySaverModeTabDiscardingHelpUrl[] =
    u"https://flyweb.lamosquita.net/ayuda/#rendimiento";

// The URL to the help center article of Incognito mode.
inline constexpr char16_t kIncognitoHelpCenterURL[] =
    u"https://flyweb.lamosquita.net/ayuda/#privacidad";

// "Learn more" URL for the web app installation flow, linked from installation
// dialog and options view.
inline constexpr char kInstallDialogFlowLearnMoreURL[] =
    "https://flyweb.lamosquita.net/ayuda/";

// The URL for the "Learn more" page for the usage/crash reporting option in the
// first run dialog.
inline constexpr char kLearnMoreReportingURL[] =
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

// "myactivity.google.com" URLs with their respective UTM sources.
// - In the Clear Browsing Data footer.
// - In the Clear Browsing Data "notice about other forms of history".
// - On the history page.
inline constexpr char16_t kMyActivityUrlInClearBrowsingData[] =
    u"https://flyweb.lamosquita.net/ayuda/#privacidad";
inline constexpr char16_t kMyActivityUrlInClearBrowsingDataNotice[] =
    u"https://flyweb.lamosquita.net/ayuda/";
inline constexpr char16_t kMyActivityUrlInHistory[] =
    u"https://flyweb.lamosquita.net/ayuda/";

// The URL for the Gemini Personal Context page.
inline constexpr char16_t kGeminiPersonalContextUrl[] =
    u"https://flyweb.lamosquita.net/ayuda/";

// The URL for "Your Gemini Apps Activity" page.
inline constexpr char16_t kMyActivityGeminiAppsUrl[] =
    u"https://flyweb.lamosquita.net/ayuda/";

// The URL for Glic activity button.
inline constexpr char kGlicActivityUrl[] =
    "https://flyweb.lamosquita.net/ayuda/";

inline constexpr char16_t kMyActivityAiModeUrl[] =
    u"https://flyweb.lamosquita.net/ayuda/";

inline constexpr char16_t kMyActivitySearchServicesAppsUrl[] =
    u"https://flyweb.lamosquita.net/ayuda/";

#if BUILDFLAG(GOOGLE_CHROME_BRANDING)
// "Learn more" URL for On-Device AI.
inline constexpr char16_t kOnDeviceAiLearnMoreUrl[] =
    u"https://flyweb.lamosquita.net/ayuda/";
#endif

// Help URL for the Omnibox setting.
inline constexpr char16_t kOmniboxLearnMoreURL[] =
    u"https://flyweb.lamosquita.net/ayuda/#buscador";

// "What do these mean?" URL for the Page Info bubble.
inline constexpr char kPageInfoHelpCenterURL[] =
    "https://flyweb.lamosquita.net/ayuda/#seguridad";

// Help center article URL for automated password change.
inline constexpr char16_t kPasswordChangeLearnMoreURL[] =
    u"https://flyweb.lamosquita.net/ayuda/";

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

// The URL for the "Fill out forms automatically" support page.
inline constexpr char kAddressesAndPaymentMethodsLearnMoreURL[] =
    "https://flyweb.lamosquita.net/ayuda/#autorrelleno";

// The URL for the "Pay over time" support page.
inline constexpr char16_t kPayOverTimeLearnMoreUrl[] =
    u"https://flyweb.lamosquita.net/ayuda/";

// The URL for the Wallet website.
inline constexpr char16_t kWalletUrl[] =
    u"https://flyweb.lamosquita.net/ayuda/";

// Help URL for Autofill AI.
inline constexpr char16_t kAutofillAiLearnMoreURL[] =
    u"https://flyweb.lamosquita.net/ayuda/";

// "Learn more" URL for the autofill show card benefits setting.
inline constexpr char16_t kCardBenefitsLearnMoreURL[] =
    u"https://flyweb.lamosquita.net/ayuda/";

// "Learn more" URL for the Gmail OTP filling setting.
inline constexpr char16_t kGmailOtpFillingLearnMoreURL[] =
    u"https://flyweb.lamosquita.net/ayuda/";

// URL for Gmail general settings.
inline constexpr char16_t kGmailSettingsURL[] =
    u"https://flyweb.lamosquita.net/ayuda/";

// URL for Gmail smart features settings.
inline constexpr char16_t kGmailSmartFeaturesURL[] =
    u"https://flyweb.lamosquita.net/ayuda/";

// "Learn more" URL for the performance intervention notification setting.
inline constexpr char16_t kPerformanceInterventionLearnMoreUrl[] =
    u"https://flyweb.lamosquita.net/ayuda/#rendimiento";

// "Learn more" URL for the preloading section in Performance settings.
inline constexpr char16_t kPreloadingLearnMoreUrl[] =
    u"https://flyweb.lamosquita.net/ayuda/#privacidad";

// "Learn more" URL for the Privacy section under Options.
inline constexpr char kPrivacyLearnMoreURL[] =
    "https://flyweb.lamosquita.net/ayuda/#privacidad";

// "Chrome Settings" URL for Ad Topics page
inline constexpr char kPrivacySandboxAdTopicsURL[] = "";

// "Chrome Settings" URL for Managing Topics page
inline constexpr char kPrivacySandboxManageTopicsURL[] = "";

// The URL for the Learn More link of the non-CWS bubble.
inline constexpr char kRemoveNonCWSExtensionURL[] =
    "https://flyweb.lamosquita.net/ayuda/#extensiones";

// "Learn more" URL for resetting profile preferences.
inline constexpr char kResetProfileSettingsLearnMoreURL[] =
    "https://flyweb.lamosquita.net/ayuda/#restablecer";

// "Learn more" URL for Safebrowsing
inline constexpr char kSafeBrowsingHelpCenterURL[] =
    "https://flyweb.lamosquita.net/ayuda/#seguridad";

// "Learn more" URL for unsafe site warnings.
inline constexpr char kUnsafeSiteWarningHelpCenterURL[] =
    "https://flyweb.lamosquita.net/ayuda/";

// Updated "Info icon" URL for Safebrowsing
inline constexpr char kSafeBrowsingHelpCenterUpdatedURL[] =
    "https://flyweb.lamosquita.net/ayuda/#seguridad";

// "Learn more" URL for Enhanced Protection
inline constexpr char16_t kSafeBrowsingInChromeHelpCenterURL[] =
    u"https://flyweb.lamosquita.net/ayuda/#seguridad";

// The URL for Safe Browsing link in Safety Check page.
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
#if BUILDFLAG(IS_CHROMEOS)
static_assert(std::string_view(kSyncAndGoogleServicesLearnMoreURL) ==
              ash::chrome_external_urls::kSyncAndGoogleServicesLearnMoreURL);
#endif

// The URL for the "Learn more" page on sync encryption.
inline constexpr char16_t kSyncEncryptionHelpURL[] =
    u"https://flyweb.lamosquita.net/ayuda/sincronizar";

// The URL for the "Learn more" link when there is a sync error.
inline constexpr char kSyncErrorsHelpURL[] =
    "https://flyweb.lamosquita.net/ayuda/sincronizar";

// Legacy URL to the sync google dashboard.
inline constexpr char kLegacySyncGoogleDashboardURL[] =
    "https://flyweb.lamosquita.net/ayuda/";
#if BUILDFLAG(IS_CHROMEOS)
static_assert(std::string_view(kLegacySyncGoogleDashboardURL) ==
              ash::chrome_external_urls::kLegacySyncGoogleDashboardURL);
#endif

// New URL to the sync google dashboard.
inline constexpr char kNewSyncGoogleDashboardURL[] =
    "https://flyweb.lamosquita.net/ayuda/";
#if BUILDFLAG(IS_CHROMEOS)
static_assert(std::string_view(kNewSyncGoogleDashboardURL) ==
              ash::chrome_external_urls::kNewSyncGoogleDashboardURL);
#endif

// The URL for the "Learn more" page for sync setup on the personal stuff page.
inline constexpr char16_t kSyncLearnMoreURL[] =
    u"https://flyweb.lamosquita.net/ayuda/sincronizar";

// The URL for the "Learn more" page for signing in to chrome with expanded
// section on "Sign in and turn on sync" in the Computer/Desktop tab.
inline constexpr char kSigninOnDesktopLearnMoreURL[] =
    "https://flyweb.lamosquita.net/ayuda/";

// The URL for the "Learn more" page for adding a new profile to Chrome.
inline constexpr char kAddNewProfileOnDesktopLearnMoreURL[] =
    "https://flyweb.lamosquita.net/ayuda/";

// The URL for the "Learn more" page for AI settings for managed users.
inline constexpr char16_t kAiSettingsLearnMorePageManagedUrl[] =
    u"https://flyweb.lamosquita.net/ayuda/";

// The URL for the "Learn more" page for Help me Write.
inline constexpr char kComposeLearnMorePageURL[] =
    "https://flyweb.lamosquita.net/ayuda/";

// The URL for the "Learn more" page for Help me Write for managed users.
inline constexpr char kComposeLearnMorePageManagedURL[] =
    "https://flyweb.lamosquita.net/ayuda/";

// The URL for the "Learn more" links for pages related to History search.
inline constexpr char kHistorySearchLearnMorePageURL[] =
    "https://flyweb.lamosquita.net/ayuda/";

// The URL for the "Learn more" links for pages related to History search for
// managed users.
inline constexpr char kHistorySearchLearnMorePageManagedURL[] =
    "https://flyweb.lamosquita.net/ayuda/";

// The URL for the Settings page to enable history search.
inline constexpr char16_t kHistorySearchSettingURL[] =
    u"chrome://settings/historySearch";

// The URL for the "Learn more" page for Wallpaper Search.
inline constexpr char kWallpaperSearchLearnMorePageURL[] =
    "https://flyweb.lamosquita.net/ayuda/";

// The URL for the passed in Google Wallet.
inline constexpr char kWalletPassesPageURL[] =
    "https://flyweb.lamosquita.net/ayuda/";

// The help center article URL on using Wallet private passes across Google.
inline constexpr char kWalletPrivatePassHelpCenterURL[] =
    "https://flyweb.lamosquita.net/ayuda/";

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

// The URL path to Google's Privacy Policy page.
inline constexpr char kPrivacyPolicyURL[] =
    "https://flyweb.lamosquita.net/ayuda/";

// The URL path to Google's Privacy Policy page for users in China.
inline constexpr char kPrivacyPolicyURLChina[] =
    "https://flyweb.lamosquita.net/ayuda/";

// The URL path to Google's Embedded Privacy Policy page.
inline constexpr char kPrivacyPolicyOnlineURLPath[] =
    "https://flyweb.lamosquita.net/ayuda/";

// The URL path to Google's Embedded Privacy Policy page for users in China.
inline constexpr char kPrivacyPolicyEmbeddedURLPathChina[] =
    "https://flyweb.lamosquita.net/ayuda/";

#if BUILDFLAG(IS_ANDROID) || BUILDFLAG(IS_CHROMEOS)
// "Learn more" URL for the enhanced playback notification dialog.
inline constexpr char kEnhancedPlaybackNotificationLearnMoreURL[] =
    "https://flyweb.lamosquita.net/ayuda/";
#endif

#if BUILDFLAG(IS_CHROMEOS)
// Chrome OS default pre-defined custom handlers
inline constexpr char kChromeOSDefaultMailtoHandler[] =
    "https://mail.google.com/mail/?extsrc=mailto&amp;url=%s";
inline constexpr char kChromeOSDefaultWebcalHandler[] =
    "https://www.google.com/calendar/render?cid=%s";

// The URL for the "Account recovery" page.
inline constexpr char kAccountRecoveryURL[] =
    "https://accounts.google.com/signin/recovery";

// The URL for the Learn More page about enterprise enrolled devices.
inline constexpr char kLearnMoreEnterpriseURL[] =
    "https://flyweb.lamosquita.net/ayuda/";

#endif  // BUILDFLAG(IS_CHROMEOS)

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

// The URL for the "Learn more" page for process isolation.
inline constexpr char kProcessIsolationLearnMoreUrl[] =
    "https://flyweb.lamosquita.net/ayuda/";
#endif

#if BUILDFLAG(IS_WIN) || BUILDFLAG(IS_MAC) || BUILDFLAG(IS_LINUX)
// "Learn more" URL for the chrome apps deprecation dialog.
inline constexpr char kChromeAppsDeprecationLearnMoreURL[] =
    "https://support.google.com/chrome/?p=chrome_app_deprecation";
#endif

#if BUILDFLAG(CHROME_ROOT_STORE_SUPPORTED)
inline constexpr char kChromeRootStoreSettingsHelpCenterURL[] =
    "https://chromium.googlesource.com/chromium/src/+/main/net/data/ssl/chrome_root_store/root_store.md";
#endif

#if BUILDFLAG(ENABLE_PDF_SAVE_TO_DRIVE)
// "Learn more" URL for how to save PDF to Google Drive from the PDF viewer.
inline constexpr char kPdfViewerSaveToDriveHelpCenterURL[] =
    "https://flyweb.lamosquita.net/ayuda/";
#endif

// Please do not append entries here. See the comments at the top of the file.

}  // namespace chrome

#endif  // BRAVE_CHROMIUM_SRC_CHROME_COMMON_URL_CONSTANTS_H_

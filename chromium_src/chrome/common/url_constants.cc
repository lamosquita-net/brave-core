/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "chrome/common/url_constants.h"

#include "build/branding_buildflags.h"
#include "build/build_config.h"
#include "chrome/common/webui_url_constants.h"

namespace chrome {

const char kAccessCodeCastLearnMoreURL[] =
    "https://flyweb.lamosquita.net/ayuda/";

const char kAccessibilityLabelsLearnMoreURL[] =
    "https://flyweb.lamosquita.net/ayuda/";

const char kAdPrivacyLearnMoreURL[] =
    "https://flyweb.lamosquita.net/ayuda/#privacidad";

const char kAutomaticSettingsResetLearnMoreURL[] =
    "https://flyweb.lamosquita.net/ayuda/#restablecer";

const char kAdvancedProtectionDownloadLearnMoreURL[] =
    "https://flyweb.lamosquita.net/ayuda/#descargas";

const char kBatterySaverModeLearnMoreUrl[] =
    "https://flyweb.lamosquita.net/ayuda/#rendimiento";

const char kBluetoothAdapterOffHelpURL[] =
    "https://flyweb.lamosquita.net/ayuda/";

const char kCastCloudServicesHelpURL[] = "https://flyweb.lamosquita.net/ayuda/";

const char kCastNoDestinationFoundURL[] =
    "https://flyweb.lamosquita.net/ayuda/";

const char kChooserHidOverviewUrl[] =
    "https://flyweb.lamosquita.net/ayuda/#permisos";

const char kChooserSerialOverviewUrl[] =
    "https://flyweb.lamosquita.net/ayuda/#permisos";

const char kChooserUsbOverviewURL[] =
    "https://flyweb.lamosquita.net/ayuda/#permisos";

const char kChromeBetaForumURL[] = "https://flyweb.lamosquita.net/ayuda/";

const char kChromeFixUpdateProblems[] =
    "https://flyweb.lamosquita.net/ayuda/#actualizaciones";

const char kChromeHelpViaKeyboardURL[] = "https://flyweb.lamosquita.net/ayuda/";

const char kChromeHelpViaMenuURL[] = "https://flyweb.lamosquita.net/ayuda/";

const char kChromeHelpViaWebUIURL[] = "https://flyweb.lamosquita.net/ayuda/";

const char kFirstPartySetsLearnMoreURL[] =
    "https://flyweb.lamosquita.net/ayuda/#permisos";

const char kIsolatedAppScheme[] = "isolated-app";

const char kChromeNativeScheme[] = "chrome-native";

const char kChromeSearchLocalNtpHost[] = "local-ntp";

const char kChromeSearchMostVisitedHost[] = "most-visited";
const char kChromeSearchMostVisitedUrl[] = "chrome-search://most-visited/";

const char kChromeUIUntrustedNewTabPageBackgroundUrl[] =
    "chrome-untrusted://new-tab-page/background.jpg";
const char kChromeUIUntrustedNewTabPageBackgroundFilename[] = "background.jpg";

const char kChromeSearchRemoteNtpHost[] = "remote-ntp";

const char kChromeSearchScheme[] = "chrome-search";

const char kChromeUIUntrustedNewTabPageUrl[] =
    "chrome-untrusted://new-tab-page/";

const char kChromiumProjectURL[] = "https://www.chromium.org/";

const char kContentSettingsExceptionsLearnMoreURL[] =
    "https://flyweb.lamosquita.net/ayuda/#permisos";

const char kCookiesSettingsHelpCenterURL[] =
    "https://flyweb.lamosquita.net/ayuda/#permisos";

const char kCrashReasonURL[] = "https://flyweb.lamosquita.net/ayuda/#fallos";

const char kCrashReasonFeedbackDisplayedURL[] =
    "https://flyweb.lamosquita.net/ayuda/#fallos";

const char kDoNotTrackLearnMoreURL[] =
    "https://flyweb.lamosquita.net/ayuda/#privacidad";

const char kDownloadInterruptedLearnMoreURL[] =
    "https://flyweb.lamosquita.net/ayuda/#descargas";

const char kDownloadScanningLearnMoreURL[] =
    "https://flyweb.lamosquita.net/ayuda/#descargas";

const char kExtensionControlledSettingLearnMoreURL[] =
    "https://flyweb.lamosquita.net/ayuda/#extensiones";

const char kExtensionInvalidRequestURL[] = "chrome-extension://invalid/";

const char kFlashDeprecationLearnMoreURL[] =
    "https://blog.chromium.org/2017/07/so-long-and-thanks-for-all-flash.html";

const char kGoogleAccountActivityControlsURL[] =
    "https://flyweb.lamosquita.net/ayuda/#privacidad";

const char kGoogleAccountActivityControlsURLInPrivacyGuide[] =
    "https://flyweb.lamosquita.net/ayuda/#privacidad";

const char kGoogleAccountURL[] = "https://flyweb.lamosquita.net/ayuda/";

const char kGoogleAccountChooserURL[] = "https://flyweb.lamosquita.net/ayuda/";

const char kGoogleAccountDeviceActivityURL[] =
    "https://flyweb.lamosquita.net/ayuda/";

const char kGooglePasswordManagerURL[] =
    "https://flyweb.lamosquita.net/ayuda/#contrasenas";

const char kLearnMoreReportingURL[] =
    "https://flyweb.lamosquita.net/ayuda/#privacidad";

const char kHighEfficiencyModeLearnMoreUrl[] =
    "https://flyweb.lamosquita.net/ayuda/#rendimiento";

const char kHighEfficiencyModeTabDiscardingHelpUrl[] =
    "https://flyweb.lamosquita.net/ayuda/#rendimiento";

const char kManagedUiLearnMoreUrl[] = "https://flyweb.lamosquita.net/ayuda/";

const char kInsecureDownloadBlockingLearnMoreUrl[] =
    "https://flyweb.lamosquita.net/ayuda/#descargas";

const char kMyActivityUrlInClearBrowsingData[] =
    "https://flyweb.lamosquita.net/ayuda/#privacidad";

const char kOmniboxLearnMoreURL[] =
    "https://flyweb.lamosquita.net/ayuda/#buscador";

const char kPageInfoHelpCenterURL[] =
    "https://flyweb.lamosquita.net/ayuda/#seguridad";

const char kPasswordCheckLearnMoreURL[] =
    "https://flyweb.lamosquita.net/ayuda/#contrasenas";

const char kPasswordGenerationLearnMoreURL[] =
    "https://flyweb.lamosquita.net/ayuda/#contrasenas";

const char kPasswordManagerLearnMoreURL[] =
    "https://flyweb.lamosquita.net/ayuda/#contrasenas";

const char kPaymentMethodsURL[] =
    "https://flyweb.lamosquita.net/ayuda/#autorrelleno";

const char kPrivacyLearnMoreURL[] =
    "https://flyweb.lamosquita.net/ayuda/#privacidad";

const char kRemoveNonCWSExtensionURL[] =
    "https://flyweb.lamosquita.net/ayuda/#extensiones";

const char kResetProfileSettingsLearnMoreURL[] =
    "https://flyweb.lamosquita.net/ayuda/#restablecer";

const char kSafeBrowsingHelpCenterURL[] =
    "https://flyweb.lamosquita.net/ayuda/#seguridad";

const char kSafetyTipHelpCenterURL[] =
    "https://flyweb.lamosquita.net/ayuda/#seguridad";

const char kSearchHistoryUrlInClearBrowsingData[] =
    "https://flyweb.lamosquita.net/ayuda/#privacidad";

const char kSeeMoreSecurityTipsURL[] =
    "https://flyweb.lamosquita.net/ayuda/#seguridad";

const char kSettingsSearchHelpURL[] =
    "https://flyweb.lamosquita.net/ayuda/#buscador";

const char kSyncAndGoogleServicesLearnMoreURL[] =
    "https://flyweb.lamosquita.net/ayuda/sincronizar";

const char kSyncEncryptionHelpURL[] =
    "https://flyweb.lamosquita.net/ayuda/sincronizar";

const char kSyncErrorsHelpURL[] =
    "https://flyweb.lamosquita.net/ayuda/sincronizar";

const char kSyncGoogleDashboardURL[] =
    "https://flyweb.lamosquita.net/ayuda/sincronizar";

const char kSyncLearnMoreURL[] =
    "https://flyweb.lamosquita.net/ayuda/sincronizar";

const char kSigninInterceptManagedDisclaimerLearnMoreURL[] =
    "https://flyweb.lamosquita.net/ayuda/";

#if !BUILDFLAG(IS_ANDROID)
const char kSyncTrustedVaultOptInURL[] =
    "https://flyweb.lamosquita.net/ayuda/sincronizar";
#endif

const char kSyncTrustedVaultLearnMoreURL[] =
    "https://flyweb.lamosquita.net/ayuda/sincronizar";

const char kUpgradeHelpCenterBaseURL[] =
    "https://flyweb.lamosquita.net/ayuda/#actualizaciones";

const char kWhoIsMyAdministratorHelpURL[] =
    "https://flyweb.lamosquita.net/ayuda/";

const char kCwsEnhancedSafeBrowsingLearnMoreURL[] =
    "https://flyweb.lamosquita.net/ayuda/#seguridad";

#if BUILDFLAG(IS_ANDROID)
const char kEnhancedPlaybackNotificationLearnMoreURL[] =
// Keep in sync with chrome/android/java/strings/android_chrome_strings.grd
    "https://flyweb.lamosquita.net/ayuda/";
#endif

#if BUILDFLAG(IS_MAC)
const char kChromeEnterpriseSignInLearnMoreURL[] =
    "https://flyweb.lamosquita.net/ayuda/";

const char kMacOsObsoleteURL[] =
    "https://flyweb.lamosquita.net/ayuda/#actualizaciones";
#endif

#if BUILDFLAG(IS_WIN)
const char kWindowsXPVistaDeprecationURL[] =
    "https://flyweb.lamosquita.net/ayuda/";

const char kWindows78DeprecationURL[] = "https://flyweb.lamosquita.net/ayuda/";
#endif  // BUILDFLAG(IS_WIN)

const char kChromeSyncLearnMoreURL[] =
    "https://flyweb.lamosquita.net/ayuda/sincronizar";

#if BUILDFLAG(ENABLE_PLUGINS)
const char kOutdatedPluginLearnMoreURL[] =
    "https://flyweb.lamosquita.net/ayuda/";
#endif

#if BUILDFLAG(IS_WIN) || BUILDFLAG(IS_MAC) || BUILDFLAG(IS_LINUX)
const char kChromeAppsDeprecationLearnMoreURL[] =
    "https://support.google.com/chrome/?p=chrome_app_deprecation";
#endif

#if BUILDFLAG(CHROME_ROOT_STORE_SUPPORTED)
// TODO(b/1339340): add help center link when help center link is created.
const char kChromeRootStoreSettingsHelpCenterURL[] =
    "https://chromium.googlesource.com/chromium/src/+/main/net/data/ssl/"
    "chrome_root_store/root_store.md";
#endif

const char kAddressesAndPaymentMethodsLearnMoreURL[] =
    "https://flyweb.lamosquita.net/ayuda/#autorrelleno";

const char kPasswordManagerImportLearnMoreURL[] =
    "https://flyweb.lamosquita.net/ayuda/#contrasenas";

}  // namespace chrome

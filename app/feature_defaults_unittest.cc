/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

// Inventory of the upstream feature defaults that FlyWeb overrides with
// OVERRIDE_FEATURE_DEFAULT_STATES in chromium_src, plus the Blink features
// whose base_feature_status FlyWeb sets in runtime_enabled_features.json5.
// Not listed, as their headers are internal to //content:
// PrefetchClientHints and PrefetchServiceWorker (off) in
// content/browser/preloading/prefetch/prefetch_features.cc and
// FileSystemAccessDirectoryIterationBlocklistCheck (off) in
// content/browser/file_system_access/features.cc.

#include "base/feature_list.h"
#include "build/build_config.h"
#include "chrome/browser/browser_features.h"
#include "chrome/browser/devtools/features.h"
#include "chrome/browser/ui/ui_features.h"
#include "chrome/common/chrome_features.h"
#include "components/attribution_reporting/features.h"
#include "components/autofill/core/common/autofill_debug_features.h"
#include "components/autofill/core/common/autofill_features.h"
#include "components/autofill/core/common/autofill_payments_features.h"
#include "components/commerce/core/commerce_feature_list.h"
#include "components/compose/core/browser/compose_features.h"
#include "components/heap_profiling/in_process/heap_profiler_parameters.h"
#include "components/history/core/browser/features.h"
#include "components/history_clusters/core/features.h"
#include "components/history_clusters/core/on_device_clustering_features.h"
#include "components/lens/lens_features.h"
#include "components/metrics/metrics_features.h"
#include "components/metrics/structured/structured_metrics_features.h"
#include "components/network_time/network_time_tracker.h"
#include "components/ntp_tiles/features.h"
#include "components/omnibox/browser/aim_eligibility_service_features.h"
#include "components/omnibox/common/omnibox_features.h"
#include "components/optimization_guide/core/optimization_guide_features.h"
#include "components/passage_embeddings/core/passage_embeddings_features.h"
#include "components/password_manager/core/browser/features/password_features.h"
#include "components/permissions/features.h"
#include "components/personal_context/core/personal_context_features.h"
#include "components/privacy_sandbox/privacy_sandbox_features.h"
#include "components/private_ai/features.h"
#include "components/safe_browsing/core/common/features.h"
#include "components/search/ntp_features.h"
#include "components/segmentation_platform/public/features.h"
#include "components/signin/public/base/signin_switches.h"
#include "components/subresource_filter/core/common/common_features.h"
#include "components/webapps/browser/features.h"
#include "content/common/features.h"
#include "content/public/common/content_features.h"
#include "gpu/config/gpu_finch_features.h"
#include "media/base/media_switches.h"
#include "net/base/features.h"
#include "pdf/buildflags.h"
#include "services/device/public/cpp/device_features.h"
#include "services/network/public/cpp/features.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "third_party/blink/public/common/features.h"

#if !BUILDFLAG(IS_ANDROID)
#include "chrome/browser/sharing_hub/sharing_hub_features.h"
#endif

#if BUILDFLAG(ENABLE_PDF)
#include "pdf/pdf_features.h"
#endif

#if BUILDFLAG(IS_ANDROID)
#include "components/feed/feed_feature_list.h"
#endif

namespace {

void ExpectOverriddenFeatureDefault(const base::Feature& feature,
                                    bool enabled) {
  SCOPED_TRACE(feature.name);
  EXPECT_EQ(base::FeatureList::IsEnabled(feature), enabled);

  auto* feature_list = base::FeatureList::GetInstance();
  ASSERT_NE(feature_list, nullptr);
  EXPECT_TRUE(feature_list->IsFeatureOverridden(feature.name));
  EXPECT_EQ(base::FeatureList::GetStateIfOverridden(feature), enabled);
}

}  // namespace

TEST(FeatureDefaultsTest, DisabledFeatures) {
  // Please, keep alphabetized.
  const base::Feature* features[] = {
      &attribution_reporting::features::kConversionMeasurement,
      &autofill::features::debug::kAutofillServerCommunication,
      &autofill::features::kAutofillAiServerModel,
      &autofill::features::kAutofillAiWithDataSchema,
      &autofill::features::kAutofillEnableAmountExtraction,
      &autofill::features::kAutofillEnableBuyNowPayLater,
      &blink::features::kAllowURNsInIframes,
      &blink::features::kBackgroundResourceFetch,
      &blink::features::kFencedFrames,
#if BUILDFLAG(ENABLE_PDF) && BUILDFLAG(ENABLE_PDF_SAVE_TO_DRIVE)
      &chrome_pdf::features::kPdfSaveToDrive,
#endif
      &commerce::kCommerceAllowOnDemandBookmarkUpdates,
      &commerce::kCommerceMerchantViewer,
      &commerce::kRetailCoupons,
      &commerce::kShoppingList,
      &commerce::kShoppingPDPMetrics,
      &compose::features::kEnableCompose,
      &features::kChromeStructuredMetrics,
      &features::kDestroyProfileOnBrowserClose,
      &features::kDevToolsAiAssistanceAccessibilityAgent,
      &features::kDevToolsAiAssistanceContextSelectionAgent,
      &features::kDevToolsAiAssistanceFileAgent,
      &features::kDevToolsAiAssistanceNetworkAgent,
      &features::kDevToolsAiAssistancePerformanceAgent,
      &features::kDevToolsAiAssistanceStorageAgent,
      &features::kDevToolsAiCodeCompletion,
      &features::kDevToolsAiCodeCompletionStyles,
      &features::kDevToolsAiCodeGeneration,
      &features::kDevToolsAiGeneratedTimelineLabels,
      &features::kDevToolsAiNaturalLanguageInterface,
      &features::kDevToolsAiOriginTrialsApis,
      &features::kDevToolsAiV2Architecture,
      &features::kDevToolsConsoleInsights,
      &features::kDevToolsConsoleInsightsTeasers,
      &features::kDevToolsFreestyler,
      &features::kDevToolsGdpProfiles,
      &features::kDevToolsGlobalAiButton,
      &features::kDevToolsNewPermissionDialog,
      &features::kDevToolsUseGcaApi,
      &features::kDigitalGoodsApi,
      &features::kEmailVerificationProtocol,
      &features::kFedCm,
#if BUILDFLAG(IS_WIN) || BUILDFLAG(IS_MAC) || BUILDFLAG(IS_LINUX)
      &features::kFewerUpdateConfirmations,
#endif
      &features::kHttpsFirstBalancedMode,
      &features::kNewTabPageTriggerForPrerender2,
      &features::kPrefetchProxy,
      &features::kPrivacySandboxAdsAPIsM1Override,
      &features::kPrivacySandboxAdsAPIsOverride,
      &features::kSCTAuditingHashdance,
      &features::kServiceWorkerAutoPreload,
      &features::kTabHoverCardImages,
#if !BUILDFLAG(IS_ANDROID)
      &features::kTrustSafetySentimentSurvey,
      &features::kTrustSafetySentimentSurveyV2,
#endif
      &features::kWebGPUBlobCache,
      &features::kWebGPUService,
      &features::kWebIdentityDigitalCredentials,
      &features::kWebIdentityDigitalCredentialsCreation,
      &features::kWebOTP,
#if BUILDFLAG(IS_ANDROID)
      &feed::kInterestFeedV2,
#endif
      &heap_profiling::kHeapProfilerReporting,
      &history::kOrganicRepeatableQueries,
      &history_clusters::features::kOnDeviceClustering,
      &history_clusters::features::kOnDeviceClusteringKeywordFiltering,
      &history_clusters::internal::kHistoryClustersInternalsPage,
      &history_clusters::internal::kHistoryClustersNavigationContextClustering,
      &history_clusters::internal::kJourneys,
      &history_clusters::internal::kJourneysImages,
      &history_clusters::internal::kOmniboxHistoryClusterProvider,
      &lens::features::kLensOverlay,
      &lens::features::kLensStandalone,
      &media::kLiveCaption,
      &metrics::features::kStructuredMetrics,
      &metrics::structured::kPhoneHubStructuredMetrics,
      &net::features::kTLSTrustAnchorIDs,
      &network::features::kBrowsingTopics,
      &network_time::kNetworkTimeServiceQuerying,
      &ntp_features::kCustomizeChromeSidePanelExtensionsCard,
      &ntp_features::kCustomizeChromeWallpaperSearch,
      &ntp_features::kNtpAlphaBackgroundCollections,
      &ntp_features::kNtpBackgroundImageErrorDetection,
      &ntp_features::kNtpChromeCartModule,
      &ntp_features::kNtpModulesMaxColumnCount,
      &ntp_features::kNtpSimplificationBookmarkBar,
      &ntp_tiles::kNtpMostLikelyFaviconsFromServerFeature,
      &ntp_tiles::kPopularSitesBakedInContentFeature,
      &omnibox::kAimEnabled,
      &omnibox::kMlUrlScoring,
      &omnibox::kRichAutocompletion,
      &omnibox::kStarterPackExpansion,
      &optimization_guide::features::kOptimizationGuideFetchingForSRP,
      &optimization_guide::features::kOptimizationGuideModelExecution,
      &optimization_guide::features::kOptimizationHints,
      &passage_embeddings::kPassageEmbedder,
      &permissions::features::kPermissionOnDeviceNotificationPredictions,
      &permissions::features::kPermissionPredictionsV2,
#if !BUILDFLAG(IS_ANDROID)
      &permissions::features::kPermissionsPromptSurvey,
#endif
      &permissions::features::kShowRelatedWebsiteSetsPermissionGrants,
      &personal_context::features::kPersonalContext,
      &privacy_sandbox::kEnforcePrivacySandboxAttestations,
      &private_ai::kPrivateAi,
      &safe_browsing::kClientSideDetectionClipboardCopyApi,
      &safe_browsing::kGooglePlayProtectInApkTelemetry,
      &safe_browsing::kNotificationTelemetry,
      &segmentation_platform::features::kSegmentationPlatformDeviceTier,
      &segmentation_platform::features::kSegmentationPlatformFeature,
      &segmentation_platform::features::kSegmentationPlatformTimeDelaySampling,
      &subresource_filter::kAdTagging,
      &switches::kSyncEnableBookmarksInTransportMode,
      &webapps::features::kWebAppsEnableMLModelForPromotion,
  };

  for (const auto* feature : features) {
    ExpectOverriddenFeatureDefault(*feature, false);
  }
}

TEST(FeatureDefaultsTest, EnabledFeatures) {
  // Please, keep alphabetized.
  const base::Feature* features[] = {
      &blink::features::kMixedContentAutoupgrade,
      &blink::features::kReducedReferrerGranularity,
      &blink::features::kUACHOverrideBlank,
      &features::kBookmarkTriggerForPrerender2KillSwitch,
      &features::kCertificateTransparencyAskBeforeEnabling,
      &features::kLocationProviderManager,
      &media::kEnableTabMuting,
      &net::features::kPartitionConnectionsByNetworkIsolationKey,
      &omnibox::kAblateSearchProviderWarmup,
#if BUILDFLAG(IS_IOS) || BUILDFLAG(IS_LINUX) || BUILDFLAG(IS_MAC) || \
    BUILDFLAG(IS_WIN)
      &password_manager::features::kSkipUndecryptablePasswords,
#endif
#if !BUILDFLAG(IS_ANDROID)
      &sharing_hub::kDesktopScreenshots,
#endif
  };

  for (const auto* feature : features) {
    ExpectOverriddenFeatureDefault(*feature, true);
  }
}

TEST(FeatureDefaultsTest, DisabledBlinkRuntimeEnabledFeatures) {
  // Please, keep alphabetized.
  const base::Feature* features[] = {
      &blink::features::kAdInterestGroupAPI,
      &blink::features::kAIProofreadingAPI,
      &blink::features::kAIPromptAPI,
      &blink::features::kAIPromptAPIMultimodalInput,
      &blink::features::kAIRewriterAPI,
      &blink::features::kAISummarizationAPI,
      &blink::features::kAIWriterAPI,
      &blink::features::kControlledFrame,
      &blink::features::kExtensionScriptTagging,
      &blink::features::kFencedFramesRuntime,
      &blink::features::kFledge,
      &blink::features::kLanguageDetectionAPI,
      &blink::features::kParakeet,
      &blink::features::kPrerender2,
      &blink::features::kTranslationAPI,
      &blink::features::kUserMediaElement,
  };

  for (const auto* feature : features) {
    EXPECT_FALSE(base::FeatureList::IsEnabled(*feature)) << feature->name;
  }
}

TEST(FeatureDefaultsTest, EnabledBlinkRuntimeEnabledFeatures) {
  // Please, keep alphabetized.
  const base::Feature* features[] = {
      &blink::features::kReduceUserAgentMinorVersion,
  };

  for (const auto* feature : features) {
    EXPECT_TRUE(base::FeatureList::IsEnabled(*feature)) << feature->name;
  }
}

TEST(FeatureDefaultsTest, DefaultFeatureParameters) {
#if !BUILDFLAG(IS_ANDROID)
  EXPECT_EQ(features::kLocationProviderManagerParam.default_value,
            device::mojom::LocationProviderManagerMode::kPlatformOnly);
#endif
}

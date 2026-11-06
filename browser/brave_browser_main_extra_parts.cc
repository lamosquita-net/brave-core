/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "brave/browser/brave_browser_main_extra_parts.h"

#include "base/metrics/histogram_macros.h"
#include "brave/browser/brave_browser_process_impl.h"
#include "components/metrics/metrics_pref_names.h"
#include "components/prefs/pref_service.h"
#include "services/network/public/cpp/shared_url_loader_factory.h"

#if !BUILDFLAG(IS_ANDROID)
#include "brave/browser/importer/brave_importer_p3a.h"
#include "brave/browser/p3a/p3a_core_metrics.h"
#include "chrome/browser/first_run/first_run.h"
#endif  // !BUILDFLAG(IS_ANDROID)

namespace {

// Records default values for some histograms because we want these stats to be
// uploaded anyways. Corresponding components will write new values according
// to their usage scenarios.
//
// For profile specific values, see browser/profiles/profile_util.cc
void RecordInitialP3AValues() {
#if !BUILDFLAG(IS_ANDROID)
  if (first_run::IsChromeFirstRun()) {
    RecordImporterP3A(user_data_importer::ImporterType::TYPE_UNKNOWN);
  }
#endif  // !BUILDFLAG(IS_ANDROID)

  // Record crash reporting status stats.
  const bool crash_reports_enabled = g_browser_process->local_state()->
      GetBoolean(metrics::prefs::kMetricsReportingEnabled);
  UMA_HISTOGRAM_BOOLEAN("Brave.Core.CrashReportsEnabled",
                        crash_reports_enabled);
}

}  // namespace

BraveBrowserMainExtraParts::BraveBrowserMainExtraParts() = default;

BraveBrowserMainExtraParts::~BraveBrowserMainExtraParts() = default;

// Chromium 156 removed the MV2 deprecation-warning silencer (as in 1.98).
void BraveBrowserMainExtraParts::PreProfileInit() {}

void BraveBrowserMainExtraParts::PostBrowserStart() {
  g_brave_browser_process->StartBraveServices();
}

void BraveBrowserMainExtraParts::PreMainMessageLoopRun() {
  RecordInitialP3AValues();

  // The code below is not supported on android.
#if !BUILDFLAG(IS_ANDROID)
  brave::BraveWindowTracker::CreateInstance(g_browser_process->local_state());
#endif  // !BUILDFLAG(IS_ANDROID)
}

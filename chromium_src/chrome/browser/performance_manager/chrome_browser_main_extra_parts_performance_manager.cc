/* Copyright (c) 2024 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

// FlyWeb does not send UMA/UKM, so the performance_manager parts that only
// collect metrics are switched off to save CPU:
// 1. PageResourceMonitor, whose collection is CPU-intensive at startup.
// 2. MetricsProviderDesktop::Initialize(), which starts the efficiency mode
//    and resource usage sampling for UMA.

#include "build/build_config.h"
#include "components/performance_manager/public/metrics/page_resource_monitor.h"

namespace performance_manager::metrics {
namespace {
class StubGraphOwnedDefaultImpl : public GraphOwnedDefaultImpl {};
}  // namespace
}  // namespace performance_manager::metrics

#if !BUILDFLAG(IS_ANDROID)
#include "chrome/browser/performance_manager/metrics/metrics_provider_desktop.h"

namespace performance_manager {

class FakeMetricsProviderDesktop {
 public:
  static FakeMetricsProviderDesktop* GetInstance() {
    static FakeMetricsProviderDesktop instance;
    return &instance;
  }

  void Initialize() {}
};

}  // namespace performance_manager

#define MetricsProviderDesktop FakeMetricsProviderDesktop
#endif  // !BUILDFLAG(IS_ANDROID)

#define PageResourceMonitor StubGraphOwnedDefaultImpl
#include "src/chrome/browser/performance_manager/chrome_browser_main_extra_parts_performance_manager.cc"
#undef PageResourceMonitor

#if !BUILDFLAG(IS_ANDROID)
#undef MetricsProviderDesktop
#endif  // !BUILDFLAG(IS_ANDROID)

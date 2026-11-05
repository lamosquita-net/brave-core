/* Copyright (c) 2023 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "brave/browser/perf/brave_perf_switches.h"
#include "brave/components/speedreader/common/buildflags/buildflags.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/browser/ui/browser.h"
#include "chrome/test/base/in_process_browser_test.h"
#include "components/prefs/pref_service.h"
#include "content/public/test/browser_test.h"
#include "content/public/test/browser_test_utils.h"

#if BUILDFLAG(ENABLE_SPEEDREADER)
#include "brave/browser/speedreader/speedreader_service_factory.h"
#include "brave/components/speedreader/speedreader_service.h"
#endif

class BraveSpeedFeatureProcessorBrowserTest : public InProcessBrowserTest {
 protected:
  void SetUpCommandLine(base::CommandLine* command_line) override {
    InProcessBrowserTest::SetUpCommandLine(command_line);
    command_line->AppendSwitch(
        perf::switches::kEnableBraveFeaturesForPerfTesting);
  }

#if BUILDFLAG(ENABLE_SPEEDREADER)
  bool SpeedreaderIsEnabled() {
    auto* speedreader_service =
        speedreader::SpeedreaderServiceFactory::GetForBrowserContext(
            browser()->profile());
    return speedreader_service->IsEnabledForAllSites();
  }
#endif  // BUILDFLAG(ENABLE_SPEEDREADER)

};

IN_PROC_BROWSER_TEST_F(BraveSpeedFeatureProcessorBrowserTest, PRE_Default) {}

IN_PROC_BROWSER_TEST_F(BraveSpeedFeatureProcessorBrowserTest, Default) {
#if BUILDFLAG(ENABLE_SPEEDREADER)
  EXPECT_TRUE(SpeedreaderIsEnabled());
#endif

}

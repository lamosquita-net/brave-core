/* Copyright (c) 2023 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include <memory>

#include "base/command_line.h"
#include "base/functional/bind.h"
#include "base/memory/raw_ptr.h"
#include "base/run_loop.h"
#include "base/test/simple_test_tick_clock.h"
#include "base/test/test_timeouts.h"
#include "base/time/time.h"
#include "components/captive_portal/content/captive_portal_service.h"
#include "components/captive_portal/core/captive_portal_testing_utils.h"
#include "components/captive_portal/core/captive_portal_types.h"
#include "components/embedder_support/pref_names.h"
#include "components/prefs/pref_registry_simple.h"
#include "components/prefs/testing_pref_service.h"
#include "content/public/test/browser_task_environment.h"
#include "content/public/test/test_browser_context.h"
#include "net/base/net_errors.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace captive_portal {

namespace {

// An observer watches the CaptivePortalDetector.  It tracks the last
// received result and the total number of received results.
class CaptivePortalObserver {
 public:
  explicit CaptivePortalObserver(CaptivePortalService* captive_portal_service)
      : captive_portal_result_(captive_portal_service->last_detection_result()),
        num_results_received_(0),
        captive_portal_service_(captive_portal_service),
        subscription_(captive_portal_service->RegisterCallback(
            base::BindRepeating(&CaptivePortalObserver::Observe,
                                base::Unretained(this)))) {}

  CaptivePortalObserver(const CaptivePortalObserver&) = delete;
  CaptivePortalObserver& operator=(const CaptivePortalObserver&) = delete;

  CaptivePortalResult captive_portal_result() const {
    return captive_portal_result_;
  }

  int num_results_received() const { return num_results_received_; }

 private:
  void Observe(const CaptivePortalService::Results& results) {
    EXPECT_EQ(captive_portal_result_, results.previous_result);
    EXPECT_EQ(captive_portal_service_->last_detection_result(), results.result);

    captive_portal_result_ = results.result;
    ++num_results_received_;
  }

  CaptivePortalResult captive_portal_result_;
  int num_results_received_;

  raw_ptr<CaptivePortalService> captive_portal_service_;

  base::CallbackListSubscription subscription_;
};

}  // namespace

class CaptivePortalServiceTest : public testing::Test,
                                 public CaptivePortalDetectorTestBase {
 public:
  CaptivePortalServiceTest()
      : old_captive_portal_testing_state_(
            CaptivePortalService::get_state_for_testing()) {}

  ~CaptivePortalServiceTest() override {
    CaptivePortalService::set_state_for_testing(
        old_captive_portal_testing_state_);
  }

  // |enable_service| is whether or not the captive portal service itself
  // should be disabled.  This is different from enabling the captive portal
  // detection preference.
  void Initialize(CaptivePortalService::TestingState testing_state) {
    pref_service_.registry()->RegisterBooleanPref(
        embedder_support::kAlternateErrorPagesEnabled, true);

    CaptivePortalService::set_state_for_testing(testing_state);

    browser_context_ = std::make_unique<content::TestBrowserContext>();
    tick_clock_ = std::make_unique<base::SimpleTestTickClock>();
    tick_clock_->Advance(base::TimeTicks::Now() - tick_clock_->NowTicks());
    service_ = std::make_unique<CaptivePortalService>(
        browser_context_.get(), &pref_service_, tick_clock_.get(),
        test_loader_factory());

    // Use no delays for most tests.
    set_initial_backoff_no_portal(base::TimeDelta());
    set_initial_backoff_portal(base::TimeDelta());

    set_detector(service_->captive_portal_detector_.get());
    SetTime(base::Time::Now());

    // Disable jitter, so can check exact values.
    set_jitter_factor(0.0);

    // These values make checking exponential backoff easier.
    set_multiply_factor(2.0);
    set_maximum_backoff(base::Seconds(1600));

    // This means backoff starts after the second "failure", which is the third
    // captive portal test in a row that ends up with the same result.  Since
    // the first request uses no delay, this means the delays will be in
    // the pattern 0, 0, 100, 200, 400, etc.  There are two zeros because the
    // first check never has a delay, and the first check to have a new result
    // is followed by no delay.
    set_num_errors_to_ignore(1);

    EnableCaptivePortalDetectionPreference(true);
  }

  // Sets the captive portal checking preference.
  void EnableCaptivePortalDetectionPreference(bool enabled) {
    pref_service_.SetBoolean(embedder_support::kAlternateErrorPagesEnabled,
                             enabled);
  }

  // Triggers a captive portal check, then simulates the URL request
  // returning with the specified |net_error| and |status_code|.  If |net_error|
  // is not OK, |status_code| is ignored.  Expects the CaptivePortalService to
  // return |expected_result|.
  //
  // |expected_delay_secs| is the expected value of GetTimeUntilNextRequest().
  // The function makes sure the value is as expected, and then simulates
  // waiting for that period of time before running the test.
  //
  // If |response_headers| is non-NULL, the response will use it as headers
  // for the simulate URL request.  It must use single linefeeds as line breaks.
  void RunTest(CaptivePortalResult expected_result,
               int net_error,
               int status_code,
               int expected_delay_secs,
               const char* response_headers) {
    base::TimeDelta expected_delay = base::Seconds(expected_delay_secs);

    ASSERT_EQ(CaptivePortalService::STATE_IDLE, service()->state());
    ASSERT_EQ(expected_delay, GetTimeUntilNextRequest());

    AdvanceTime(expected_delay);
    ASSERT_EQ(base::TimeDelta(), GetTimeUntilNextRequest());

    CaptivePortalObserver observer(service());
    service()->DetectCaptivePortal();

    EXPECT_EQ(CaptivePortalService::STATE_TIMER_RUNNING, service()->state());
    EXPECT_FALSE(FetchingURL());
    ASSERT_TRUE(TimerRunning());

    base::RunLoop().RunUntilIdle();
    EXPECT_EQ(CaptivePortalService::STATE_CHECKING_FOR_PORTAL,
              service()->state());
    ASSERT_TRUE(FetchingURL());
    EXPECT_FALSE(TimerRunning());

    CompleteURLFetch(net_error, status_code, response_headers);

    EXPECT_FALSE(FetchingURL());
    EXPECT_FALSE(TimerRunning());
    EXPECT_EQ(1, observer.num_results_received());
    EXPECT_EQ(expected_result, observer.captive_portal_result());
  }

  // Tests exponential backoff.  Prior to calling, the relevant recheck settings
  // must be set to have a minimum time of 100 seconds, with 2 checks before
  // starting exponential backoff.
  void RunBackoffTest(CaptivePortalResult expected_result,
                      int net_error,
                      int status_code) {
    RunTest(expected_result, net_error, status_code, 0, nullptr);
    RunTest(expected_result, net_error, status_code, 0, nullptr);
    RunTest(expected_result, net_error, status_code, 100, nullptr);
    RunTest(expected_result, net_error, status_code, 200, nullptr);
    RunTest(expected_result, net_error, status_code, 400, nullptr);
    RunTest(expected_result, net_error, status_code, 800, nullptr);
    RunTest(expected_result, net_error, status_code, 1600, nullptr);
    RunTest(expected_result, net_error, status_code, 1600, nullptr);
  }

  // Changes test time for the service and service's captive portal
  // detector.
  void AdvanceTime(const base::TimeDelta& delta) {
    tick_clock_->Advance(delta);
    CaptivePortalDetectorTestBase::AdvanceTime(delta);
  }

  bool TimerRunning() { return service()->TimerRunning(); }

  base::TimeDelta GetTimeUntilNextRequest() {
    return service()->backoff_entry_->GetTimeUntilRelease();
  }

  void set_initial_backoff_no_portal(
      base::TimeDelta initial_backoff_no_portal) {
    service()->recheck_policy().initial_backoff_no_portal_ms =
        initial_backoff_no_portal.InMilliseconds();
  }

  void set_initial_backoff_portal(base::TimeDelta initial_backoff_portal) {
    service()->recheck_policy().initial_backoff_portal_ms =
        initial_backoff_portal.InMilliseconds();
  }

  void set_maximum_backoff(base::TimeDelta maximum_backoff) {
    service()->recheck_policy().backoff_policy.maximum_backoff_ms =
        maximum_backoff.InMilliseconds();
  }

  void set_num_errors_to_ignore(int num_errors_to_ignore) {
    service()->recheck_policy().backoff_policy.num_errors_to_ignore =
        num_errors_to_ignore;
  }

  void set_multiply_factor(double multiply_factor) {
    service()->recheck_policy().backoff_policy.multiply_factor =
        multiply_factor;
  }

  void set_jitter_factor(double jitter_factor) {
    service()->recheck_policy().backoff_policy.jitter_factor = jitter_factor;
  }

  content::BrowserContext* browser_context() { return browser_context_.get(); }

  CaptivePortalService* service() { return service_.get(); }

 private:
  // Stores the initial CaptivePortalService::TestingState so it can be restored
  // after the test.
  const CaptivePortalService::TestingState old_captive_portal_testing_state_;

  content::BrowserTaskEnvironment task_environment_;

  // Note that the construction order of these matters.
  std::unique_ptr<content::TestBrowserContext> browser_context_;
  std::unique_ptr<base::SimpleTestTickClock> tick_clock_;
  TestingPrefServiceSimple pref_service_;
  std::unique_ptr<CaptivePortalService> service_;
};

// FlyWeb: captive portal detection is always off, even with the "resolve
// navigation errors" pref on. Brave's tests here checked that detection worked
// with its own host; FlyWeb removes that behavior on purpose (it told Brave the
// user's IP; macOS detects captive portals itself).
TEST_F(CaptivePortalServiceTest, FlyWebNeverProbes) {
  Initialize(CaptivePortalService::NOT_TESTING);  // pref on
  CaptivePortalObserver observer(service());
  service()->DetectCaptivePortal();
  base::RunLoop().RunUntilIdle();
  EXPECT_FALSE(FetchingURL());
  EXPECT_TRUE(get_probe_url().is_empty());
  // Disabled, the service reports an Internet connection so pages go on.
  EXPECT_EQ(1, observer.num_results_received());
  EXPECT_EQ(RESULT_INTERNET_CONNECTED, observer.captive_portal_result());
}

}  // namespace captive_portal

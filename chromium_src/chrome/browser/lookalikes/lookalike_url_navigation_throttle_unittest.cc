/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "chrome/browser/lookalikes/lookalike_url_navigation_throttle.h"

#include "chrome/test/base/chrome_render_view_host_test_harness.h"
#include "content/public/test/mock_navigation_handle.h"
#include "content/public/test/mock_navigation_throttle_registry.h"
#include "testing/gtest/include/gtest/gtest.h"

class LookalikeThrottleTest : public ChromeRenderViewHostTestHarness {};

// Tests that MaybeCreateAndAdd never adds a throttle as we
// don't want to use this feature. See
// https://github.com/brave/brave-browser/issues/4304
TEST_F(LookalikeThrottleTest, ThrottleDisabled) {
  GURL url("http://docs.googlé.com");
  content::MockNavigationHandle handle(url, main_rfh());
  handle.set_page_transition(ui::PAGE_TRANSITION_TYPED);

  content::MockNavigationThrottleRegistry registry(
      &handle,
      content::MockNavigationThrottleRegistry::RegistrationMode::kHold);

  LookalikeUrlNavigationThrottle::MaybeCreateAndAdd(registry);
  EXPECT_TRUE(registry.throttles().empty());
}

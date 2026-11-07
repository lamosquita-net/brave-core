/* Copyright (c) 2022 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "components/sessions/content/content_serialized_navigation_driver.h"

#include <string>

#include "components/sessions/core/serialized_navigation_entry.h"
#include "components/sessions/core/serialized_navigation_entry_test_helper.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "url/gurl.h"

namespace sessions {

// Tests that PageState data is properly sanitized when post data is present.
TEST(BraveContentSerializedNavigationDriverTest,
     PickleSanitizationWithPostDataForChromePages) {
  ContentSerializedNavigationDriver* driver =
      ContentSerializedNavigationDriver::GetInstance();
  SerializedNavigationEntry navigation =
      SerializedNavigationEntryTestHelper::CreateNavigationForTest();
  ASSERT_TRUE(navigation.has_post_data());

  // When post data is present, the page state should be sanitized.
  EXPECT_EQ(std::string(), driver->GetSanitizedPageStateForPickle(&navigation));

  // Check encoded data is empty for a chrome:// url overridable by extension.
  navigation.set_virtual_url(GURL("chrome://newtab"));
  EXPECT_EQ(std::string(), driver->GetSanitizedPageStateForPickle(&navigation));

  // Check encoded data is empty.
  navigation.set_virtual_url(GURL("chrome://settings"));
  EXPECT_EQ(std::string(), driver->GetSanitizedPageStateForPickle(&navigation));
}

// Tests that PageState data is left unsanitized when post data is absent.
TEST(BraveContentSerializedNavigationDriverTest,
     PickleSanitizationNoPostDataForChromePages) {
  ContentSerializedNavigationDriver* driver =
      ContentSerializedNavigationDriver::GetInstance();
  SerializedNavigationEntry navigation =
      SerializedNavigationEntryTestHelper::CreateNavigationForTest();
  SerializedNavigationEntryTestHelper::SetHasPostData(false, &navigation);
  ASSERT_FALSE(navigation.has_post_data());
  EXPECT_EQ(test_data::kEncodedPageState,
            driver->GetSanitizedPageStateForPickle(&navigation));

  // Check encoded data is empty for a chrome:// url overridable by extension.
  navigation.set_virtual_url(GURL("chrome://newtab"));
  EXPECT_EQ(std::string(), driver->GetSanitizedPageStateForPickle(&navigation));

  // Check encoded data is empty.
  navigation.set_virtual_url(GURL("chrome://settings"));
  EXPECT_EQ(std::string(), driver->GetSanitizedPageStateForPickle(&navigation));
}

// Tests that restored flyweb:// pages (FlyWeb 2.x sessions) are converted to
// chrome:// and lose their stale PageState.
TEST(BraveContentSerializedNavigationDriverTest,
     SanitizeConvertsFlyWebVirtualUrlToChrome) {
  ContentSerializedNavigationDriver* driver =
      ContentSerializedNavigationDriver::GetInstance();
  SerializedNavigationEntry navigation =
      SerializedNavigationEntryTestHelper::CreateNavigationForTest();
  ASSERT_FALSE(navigation.encoded_page_state().empty());

  navigation.set_virtual_url(GURL("flyweb://flags"));
  driver->Sanitize(&navigation);
  EXPECT_EQ(GURL("chrome://flags"), navigation.virtual_url());
  EXPECT_EQ(std::string(), navigation.encoded_page_state());
}

// Tests that restored legacy brave:// pages are converted to chrome://.
TEST(BraveContentSerializedNavigationDriverTest,
     SanitizeConvertsLegacyBraveVirtualUrlToChrome) {
  ContentSerializedNavigationDriver* driver =
      ContentSerializedNavigationDriver::GetInstance();
  SerializedNavigationEntry navigation =
      SerializedNavigationEntryTestHelper::CreateNavigationForTest();

  navigation.set_virtual_url(GURL("brave://settings/privacy"));
  driver->Sanitize(&navigation);
  EXPECT_EQ(GURL("chrome://settings/privacy"), navigation.virtual_url());
  EXPECT_EQ(std::string(), navigation.encoded_page_state());
}

// Tests that other restored pages are left untouched.
TEST(BraveContentSerializedNavigationDriverTest,
     SanitizeLeavesOtherVirtualUrlsAlone) {
  ContentSerializedNavigationDriver* driver =
      ContentSerializedNavigationDriver::GetInstance();
  SerializedNavigationEntry navigation =
      SerializedNavigationEntryTestHelper::CreateNavigationForTest();
  const GURL virtual_url = navigation.virtual_url();
  const std::string page_state = navigation.encoded_page_state();

  driver->Sanitize(&navigation);
  EXPECT_EQ(virtual_url, navigation.virtual_url());
  EXPECT_EQ(page_state, navigation.encoded_page_state());
}

}  // namespace sessions

/* Copyright (c) 2022 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "chrome/browser/net/stub_resolver_config_reader.h"
#include "base/feature_list.h"
#include "build/buildflag.h"
#include "chrome/browser/net/secure_dns_config.h"
#include "chrome/common/pref_names.h"
#include "components/prefs/pref_service.h"
#include "content/public/browser/network_service_instance.h"
#include "net/dns/public/dns_over_https_config.h"
#include "services/network/public/mojom/network_service.mojom.h"

// FlyWeb: secure DNS starts off, so lookups go to the system's DNS only. In
// automatic mode (Chromium's default) a known resolver in the system settings
// (8.8.8.8, 1.1.1.1) made it switch to that provider's DoH endpoint and probe
// it (dns.google, chrome.cloudflare-dns.com). The setting stays in Privacy and
// security. FlyWeb 1.15 did it with the DnsOverHttps feature, which Chromium
// 128 no longer reads: here only the pref default changes (the one call to
// ModeToString() in this file).
namespace {
constexpr net::SecureDnsMode FlyWebDefaultSecureDnsMode(net::SecureDnsMode) {
  return net::SecureDnsMode::kOff;
}
}  // namespace
#define ModeToString(MODE) ModeToString(FlyWebDefaultSecureDnsMode(MODE))
#include "src/chrome/browser/net/stub_resolver_config_reader.cc"
#undef ModeToString

/* Copyright (c) 2022 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "chrome/browser/net/default_dns_over_https_config_source.h"

#include "chrome/browser/net/secure_dns_config.h"
#include "net/dns/public/secure_dns_mode.h"

// FlyWeb: secure DNS starts off, so lookups go to the system's DNS only and
// DoH is used only with a provider the user picks in Privacy and security. In
// automatic mode (Chromium's default) a known resolver in the system settings
// (8.8.8.8, 1.1.1.1) made it switch to that provider's DoH endpoint and probe
// it (dns.google, chrome.cloudflare-dns.com). Only the pref default changes:
// the one call to ModeToString() in this file. Up to 128 (FlyWeb 2.x) the
// default was set in stub_resolver_config_reader.cc; Chromium moved it here,
// and an override left there silently stopped working in the 156 port.
// softmac FlyWeb/tools/brave-inventario/check-overrides-156.sh checks it.
namespace {
constexpr net::SecureDnsMode FlyWebDefaultSecureDnsMode(net::SecureDnsMode) {
  return net::SecureDnsMode::kOff;
}
}  // namespace
#define ModeToString(MODE) ModeToString(FlyWebDefaultSecureDnsMode(MODE))
#include "src/chrome/browser/net/default_dns_over_https_config_source.cc"
#undef ModeToString

/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "net/tools/transport_security_state_generator/input_file_parsers.h"

#include <string_view>
#include <vector>

namespace {
// FlyWeb (T9): no static pins. Brave pinned its own hosts (FlyWeb contacts
// none of them) and the pins went inert once PinsListTimestamp was older than
// 70 days; FlyWeb's own hosts get no pins either (an expired or wrong pin
// makes the service unreachable; HSTS preload and CT are enough). The
// generator reads Chromium's pins file, and enable_static_pins_ stays false as
// in any non-Google Chromium build.

constexpr std::string_view kBraveHstsJson = R"brave_hsts_json({
  "entries": [
    // Critical endpoints that should remain unpinned so that they
    // always work.
    {
      "name": "laptop-updates.brave.com",
      "mode": "force-https",
      "policy": "custom"
    },
    {
      "name": "updates.bravesoftware.com",
      "mode": "force-https",
      "policy": "custom"
    },
    {
      "name": "updates-cdn.bravesoftware.com",
      "mode": "force-https",
      "policy": "custom"
    },

    // Brave
    {
      "name": "adblock-data.s3.brave.com",
      "mode": "force-https",
      "policy": "custom"
    },
    {
      "name": "ai-chat.bsg.brave.com",
      "mode": "force-https",
      "policy": "custom"
    },
    {
      "name": "feedback.brave.com",
      "mode": "force-https",
      "policy": "custom"
    },
    {
      "name": "brave-core-ext.s3.brave.com",
      "mode": "force-https",
      "policy": "custom"
    },
    {
      "name": "brave-today-cdn.brave.com",
      "mode": "force-https",
      "policy": "custom"
    },
    {
      "name": "clients4.brave.com",
      "mode": "force-https",
      "policy": "custom"
    },
    {
      "name": "componentupdater.brave.com",
      "mode": "force-https",
      "policy": "custom"
    },
    {
      "name": "crxdownload.brave.com",
      "mode": "force-https",
      "policy": "custom"
    },
    {
      "name": "devtools.brave.com",
      "mode": "force-https",
      "policy": "custom"
    },
    {
      "name": "dict.brave.com",
      "mode": "force-https",
      "policy": "custom"
    },
    {
      "name": "extensionupdater.brave.com",
      "mode": "force-https",
      "policy": "custom"
    },
    {
      "name": "gaia.brave.com",
      "mode": "force-https",
      "policy": "custom"
    },
    {
      "name": "go-updater.brave.com",
      "mode": "force-https",
      "policy": "custom"
    },
    {
      "name": "mobile-data.s3.brave.com",
      "mode": "force-https",
      "policy": "custom"
    },
    {
      "name": "p2a.brave.com",
      "mode": "force-https",
      "policy": "custom"
    },
    {
      "name": "p2a-json.brave.com",
      "mode": "force-https",
      "policy": "custom"
    },
    {
      "name": "p3a.brave.com",
      "policy": "custom",
      "mode": "force-https"
    },
    {
      "name": "p3a-creative.brave.com",
      "policy": "custom",
      "mode": "force-https"
    },
    {
      "name": "p3a-json.brave.com",
      "policy": "custom",
      "mode": "force-https"
    },
    {
      "name": "p3a.bravesoftware.com",
      "policy": "custom",
      "mode": "force-https"
    },
    {
      "name": "p3a-dev.bravesoftware.com",
      "policy": "custom",
      "mode": "force-https"
    },
    {
      "name": "pcdn.brave.com",
      "mode": "force-https",
      "policy": "custom"
    },
    {
      "name": "redirector.brave.com",
      "mode": "force-https",
      "policy": "custom"
    },
    {
      "name": "safebrowsing.brave.com",
      "mode": "force-https",
      "policy": "custom"
    },
    {
      "name": "safebrowsing2.brave.com",
      "mode": "force-https",
      "policy": "custom"
    },
    {
      "name": "sb-ssl.brave.com",
      "mode": "force-https",
      "policy": "custom"
    },
    {
      "name": "static.brave.com",
      "mode": "force-https",
      "policy": "custom"
    },
    {
      "name": "static1.brave.com",
      "mode": "force-https",
      "policy": "custom"
    },
    {
      "name": "sync-v2.brave.com",
      "policy": "custom",
      "mode": "force-https"
    },
    {
      "name": "sync-v2.brave.software",
      "policy": "custom",
      "mode": "force-https"
    },
    {
      "name": "sync-v2.bravesoftware.com",
      "policy": "custom",
      "mode": "force-https"
    },
    {
      "name": "tor.bravesoftware.com",
      "mode": "force-https",
      "policy": "custom"
    },
    {
      "name": "translate.brave.com",
      "mode": "force-https",
      "policy": "custom"
    },
    {
      "name": "translate-static.brave.com",
      "mode": "force-https",
      "policy": "custom"
    },
    {
      "name": "variations.brave.com",
      "mode": "force-https",
      "policy": "custom"
    },

    // Creators
    {
      "name": "creators.basicattentiontoken.org",
      "mode": "force-https",
      "policy": "custom"
    },
    {
      "name": "creators.brave.com",
      "mode": "force-https",
      "policy": "custom"
    },
    {
      "name": "publishers.basicattentiontoken.org",
      "mode": "force-https",
      "policy": "custom"
    },
    {
      "name": "publishers.brave.com",
      "mode": "force-https",
      "policy": "custom"
    },

    // Wallet
    {
      "name": "goerli-infura.brave.com",
      "mode": "force-https",
      "policy": "custom"
    },
    {
      "name": "sepolia-infura.brave.com",
      "mode": "force-https",
      "policy": "custom"
    },
    {
      "name": "mainnet-infura.brave.com",
      "mode": "force-https",
      "policy": "custom"
    },
    {
      "name": "mainnet-beta-solana.brave.com",
      "mode": "force-https",
      "policy": "custom"
    },
    {
      "name": "mainnet-polygon.brave.com",
      "mode": "force-https",
      "policy": "custom"
    },

    // Rewards
    {
      "name": "anonymous.ads.brave.com",
      "mode": "force-https",
      "policy": "custom"
    },
    {
      "name": "anonymous.ads.bravesoftware.com",
      "mode": "force-https",
      "policy": "custom"
    },
    {
      "name": "api.rewards.brave.com",
      "policy": "custom",
      "mode": "force-https"
    },
    {
      "name": "api.rewards.bravesoftware.com",
      "policy": "custom",
      "mode": "force-https"
    },
    {
      "name": "api.rewards.brave.software",
      "policy": "custom",
      "mode": "force-https"
    },
    {
      "name": "geo.ads.brave.com",
      "mode": "force-https",
      "policy": "custom"
    },
    {
      "name": "geo.ads.bravesoftware.com",
      "mode": "force-https",
      "policy": "custom"
    },
    {
      "name": "grant.rewards.brave.com",
      "policy": "custom",
      "mode": "force-https"
    },
    {
      "name": "grant.rewards.bravesoftware.com",
      "policy": "custom",
      "mode": "force-https"
    },
    {
      "name": "grant.rewards.brave.software",
      "policy": "custom",
      "mode": "force-https"
    },
    {
      "name": "mywallet.ads.brave.com",
      "mode": "force-https",
      "policy": "custom"
    },
    {
      "name": "mywallet.ads.bravesoftware.com",
      "mode": "force-https",
      "policy": "custom"
    },
    {
      "name": "payment.rewards.brave.com",
      "policy": "custom",
      "mode": "force-https"
    },
    {
      "name": "payment.rewards.bravesoftware.com",
      "policy": "custom",
      "mode": "force-https"
    },
    {
      "name": "payment.rewards.brave.software",
      "policy": "custom",
      "mode": "force-https"
    },
    {
      "name": "rewards.brave.com",
      "mode": "force-https",
      "policy": "custom"
    },
    {
      "name": "search.anonymous.brave.com",
      "mode": "force-https",
      "policy": "custom"
    },
    {
      "name": "search.anonymous.bravesoftware.com",
      "mode": "force-https",
      "policy": "custom"
    },
    {
      "name": "static.ads.brave.com",
      "mode": "force-https",
      "policy": "custom"
    },
    {
      "name": "static.ads.bravesoftware.com",
      "mode": "force-https",
      "policy": "custom"
    },

    // Search
    {
      "name": "search.brave.com",
      "mode": "force-https",
      "policy": "custom"
    },
    {
      "name": "cdn.search.brave.com",
      "mode": "force-https",
      "policy": "custom"
    },
    {
      "name": "fg.search.brave.com",
      "mode": "force-https",
      "policy": "custom"
    },
    {
      "name": "imgs.search.brave.com",
      "mode": "force-https",
      "policy": "custom"
    },
    {
      "name": "tiles.search.brave.com",
      "mode": "force-https",
      "policy": "custom"
    },

    // Premium
    {
      "name": "account.brave.com",
      "mode": "force-https",
      "policy": "custom"
    },
    {
      "name": "account.bravesoftware.com",
      "mode": "force-https",
      "policy": "custom"
    },
    {
      "name": "account.brave.software",
      "mode": "force-https",
      "policy": "custom"
    }
 ]})brave_hsts_json";

void KeepOnlyForceHttpsEntries(
    net::transport_security_state::TransportSecurityStateEntries* entries) {
  std::erase_if(*entries,
                [](const auto& entry) { return !entry->force_https; });
}

}  // namespace

#include "src/net/tools/transport_security_state_generator/input_file_parsers.cc"

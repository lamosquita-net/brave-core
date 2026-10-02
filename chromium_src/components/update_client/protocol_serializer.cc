/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "components/update_client/protocol_serializer.h"

#include "brave/components/constants/brave_services_key.h"

#define BuildUpdateCheckExtraRequestHeaders \
  BuildUpdateCheckExtraRequestHeaders_ChromiumImpl
#define MakeProtocolRequest MakeProtocolRequest_ChromiumImpl
#include "src/components/update_client/protocol_serializer.cc"
#undef MakeProtocolRequest
#undef BuildUpdateCheckExtraRequestHeaders

namespace update_client {

// FlyWeb: send only what the update server needs to answer (component IDs
// and versions, browser version, OS name and architecture). Dropped, because
// together they help tell users apart: installed memory and CPU features, the
// exact OS version, the day each component was installed and the day-counter
// "pings" (roll call / active / freshness) used to count users.
protocol_request::Request MakeProtocolRequest(
    const bool is_machine,
    const std::string& session_id,
    const std::string& prod_id,
    const std::string& browser_version,
    const std::string& channel,
    const std::string& os_long_name,
    const std::string& download_preference,
    absl::optional<bool> domain_joined,
    const base::flat_map<std::string, std::string>& additional_attributes,
    const base::flat_map<std::string, std::string>& updater_state_attributes,
    std::vector<protocol_request::App> apps) {
  protocol_request::Request request = MakeProtocolRequest_ChromiumImpl(
      is_machine, session_id, prod_id, browser_version, channel, os_long_name,
      download_preference, domain_joined, additional_attributes,
      updater_state_attributes, std::move(apps));
  request.hw = protocol_request::HW();
  request.os.version.clear();
  request.os.service_pack.clear();
  for (auto& app : request.apps) {
    app.install_date = kDateUnknown;
    app.ping.reset();
  }
  return request;
}

base::flat_map<std::string, std::string> BuildUpdateCheckExtraRequestHeaders(
    const std::string& prod_id,
    const base::Version& browser_version,
    const std::vector<std::string>& ids,
    bool is_foreground) {
  auto headers = BuildUpdateCheckExtraRequestHeaders_ChromiumImpl(
      prod_id, browser_version, ids, is_foreground);
  headers.insert({"BraveServiceKey", BUILDFLAG(BRAVE_SERVICES_KEY)});
  return headers;
}

}  // namespace update_client

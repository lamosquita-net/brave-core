/* Copyright (c) 2026 The FlyWeb Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

// FlyWeb: keep .onion names out of DNS (RFC 7686, section 2). Chromium sends
// them to the system or DoH resolver like any other name; Brave used to stop
// them in its Tor code, which FlyWeb no longer ships. Every resolve request
// (RequestImpl and ServiceEndpointRequestImpl) goes through
// HostResolverManager::ResolveLocally(), whose only call to
// IsCanonicalizedHostCompliant() (or dns_names_util::IsValidDnsName() for
// source=MULTICAST_DNS) decides whether the name is resolvable at all: an
// .onion name now fails there with ERR_NAME_NOT_RESOLVED before any query
// leaves the machine. Proxy-resolved requests never reach this resolver.

#include <string_view>

#include "base/strings/string_util.h"
#include "net/base/url_util.h"
#include "net/dns/dns_names_util.h"

namespace net {
namespace {

bool IsOnionHostname(std::string_view host) {
  if (base::EndsWith(host, ".")) {
    host.remove_suffix(1);
  }
  return base::EqualsCaseInsensitiveASCII(host, "onion") ||
         base::EndsWith(host, ".onion", base::CompareCase::INSENSITIVE_ASCII);
}

bool IsCanonicalizedHostCompliantAndNotOnion(std::string_view host) {
  return IsCanonicalizedHostCompliant(host) && !IsOnionHostname(host);
}

}  // namespace

namespace dns_names_util {
namespace {

bool IsValidDnsNameAndNotOnion(std::string_view dotted_form_name) {
  return IsValidDnsName(dotted_form_name) &&
         !net::IsOnionHostname(dotted_form_name);
}

}  // namespace
}  // namespace dns_names_util
}  // namespace net

#define IsCanonicalizedHostCompliant IsCanonicalizedHostCompliantAndNotOnion
#define IsValidDnsName IsValidDnsNameAndNotOnion

#include "src/net/dns/host_resolver_manager.cc"

#undef IsValidDnsName
#undef IsCanonicalizedHostCompliant

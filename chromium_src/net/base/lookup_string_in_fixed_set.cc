/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "net/base/lookup_string_in_fixed_set.h"

#include <optional>
#include <string_view>

#define LookupSuffixInReversedSet LookupSuffixInReversedSet_ChromiumImpl
#include "src/net/base/lookup_string_in_fixed_set.cc"
#undef LookupSuffixInReversedSet

#include "brave/net/decentralized_dns/constants.h"

namespace net {

// Chromium pulls upstream public suffix list in effective_tld_names.dat and
// generate effective_tld_names.gperf from it. The list is processed by
// net/tools/dafsa/make_dafsa.py which will generate a byte array in
// effective_tld_names-reversed-inc.cc from the list to be used for comparison
// in the run-time. This function is the only function which looks up entries
// in the public suffix list, so we add our special case handling here to avoid
// patching effective_tld_names.gperf directly.
namespace {

// Matches `host` against the Brave `domain` suffix (which starts with a dot)
// and returns the suffix without the leading dot, as the DAFSA lookup would.
std::optional<SuffixMatch> MatchBraveSuffix(std::string_view host,
                                            std::string_view domain) {
  if (!host.ends_with(domain)) {
    return std::nullopt;
  }
  return SuffixMatch{.tags = DomainRuleTags(),
                     .suffix = host.substr(host.size() - (domain.size() - 1))};
}

}  // namespace

std::optional<SuffixMatch> LookupSuffixInReversedSet(
    base::span<const uint8_t> graph,
    bool include_private,
    std::string_view host) {
  // Recognize .crypto(and other ud suffixes) and .eth as known TLDs for
  // decentralized DNS support. With this, when users type *.crypto or *.eth in
  // omnibox, it will be parsed as OmniboxInputType::URL input type instead of
  // OmniboxInputType::UNKNOWN, The first entry in the autocomplete list will be
  // URL instead of search.
  for (auto* unstoppable_domain : decentralized_dns::kUnstoppableDomains) {
    if (auto match = MatchBraveSuffix(host, unstoppable_domain)) {
      return match;
    }
  }
  if (auto match = MatchBraveSuffix(host, decentralized_dns::kEthDomain)) {
    return match;
  }
  if (auto match = MatchBraveSuffix(host, decentralized_dns::kSolDomain)) {
    return match;
  }

  if (include_private) {
    if (auto match =
            MatchBraveSuffix(host, decentralized_dns::kDNSForEthDomain)) {
      return match;
    }
  }

  return LookupSuffixInReversedSet_ChromiumImpl(graph, include_private, host);
}

}  // namespace net

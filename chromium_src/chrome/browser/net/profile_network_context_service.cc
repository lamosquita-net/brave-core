/* Copyright (c) 2023 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

// FlyWeb: Brave exempted its own update hosts from Certificate Transparency.
// FlyWeb does not talk to them, so no host is exempted (only the
// CertificateTransparencyEnforcementDisabledForUrls policy can).
#define BRAVE_PROFILE_NETWORK_CONTEXT_SERVICE_GET_CT_POLICY

#include "src/chrome/browser/net/profile_network_context_service.cc"
#undef BRAVE_PROFILE_NETWORK_CONTEXT_SERVICE_GET_CT_POLICY

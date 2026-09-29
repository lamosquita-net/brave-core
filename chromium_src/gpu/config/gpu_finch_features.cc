/* Copyright (c) 2026 lamosquita. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "src/gpu/config/gpu_finch_features.cc"

#include "base/feature_override.h"

namespace features {

// FlyWeb: WebGPU is off by default. Dawn in this Chromium 116 base carries
// known in-the-wild bugs that let a compromised renderer run code in the GPU
// process (e.g. CVE-2026-5281), and no site in FlyWeb's acceptance set needs
// it. Without WebGPUService the renderer never gets GPU-process access to
// WebGPU, so navigator.gpu stays unavailable.
OVERRIDE_FEATURE_DEFAULT_STATES({{
    {kWebGPUService, base::FEATURE_DISABLED_BY_DEFAULT},
    {kWebGPUBlobCache, base::FEATURE_DISABLED_BY_DEFAULT},
}});

}  // namespace features

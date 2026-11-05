/* Copyright (c) 2023 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "chrome/elevation_service/elevator.h"

#include <windows.h>
#include <winerror.h>

#include <intsafe.h>

#include "base/path_service.h"
#include "base/win/windows_types.h"

#include "src/chrome/elevation_service/elevator.cc"

namespace elevation_service {

HRESULT Elevator::InstallVPNServices() {
  // Perform a trusted source check.
  // This ensures the caller is an executable in `%PROGRAMFILES%`.
  // For more info, see https://github.com/brave/brave-core/pull/24900
  HRESULT hr = ::CoImpersonateClient();
  if (FAILED(hr)) {
    return hr;
  }

  {
    absl::Cleanup revert_to_self = [] { ::CoRevertToSelf(); };

    const auto process = GetCallingProcess();
    if (!process.IsValid()) {
      return kErrorCouldNotObtainCallingProcess;
    }
    const auto validation_data = GenerateValidationData(
        ProtectionLevel::PROTECTION_PATH_VALIDATION, process);
    if (!validation_data.has_value()) {
      return validation_data.error();
    }
    const auto data = std::vector<uint8_t>(validation_data->cbegin(),
                                           validation_data->cend());

    // Note: Validation should always be done using caller impersonation token.
    std::string log_message;
    HRESULT validation_result = ValidateData(process, data, &log_message);
    if (FAILED(validation_result)) {
      return validation_result;
    }
  }
  // End of trusted source check

  return S_OK;
}

}  // namespace elevation_service

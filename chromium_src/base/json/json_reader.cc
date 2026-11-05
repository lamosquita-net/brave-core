/* Copyright (c) 2024 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "src/base/json/json_reader.cc"

namespace base {

JSONReader::Result DecodeJSONInRust(std::string_view json, int options) {
  return JSONReader::ReadAndReturnValueWithError(json, options);
}

}  // namespace base

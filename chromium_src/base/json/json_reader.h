/* Copyright (c) 2024 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */
#ifndef BRAVE_CHROMIUM_SRC_BASE_JSON_JSON_READER_H_
#define BRAVE_CHROMIUM_SRC_BASE_JSON_JSON_READER_H_

#include "src/base/json/json_reader.h"  // IWYU pragma: export

namespace base {

// Chromium 156 always parses JSON in Rust (serde_json_lenient), so the
// BUILD_RUST_JSON_READER buildflag and base/rust_buildflags.h are gone.
BASE_EXPORT JSONReader::Result DecodeJSONInRust(std::string_view json,
                                                int options);

}  // namespace base

#endif  // BRAVE_CHROMIUM_SRC_BASE_JSON_JSON_READER_H_

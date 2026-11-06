// Copyright (c) 2026 The FlyWeb Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

// FlyWeb: type stub for the webpack-built Leo web components bundle
// (chrome://resources/brave/leo.bundle.js, from :leo_web_components). It is
// only imported for its side effects (registering <leo-*> elements). Chromium
// 156 sets noUncheckedSideEffectImports in tools/typescript/tsconfig_base.json,
// so tsc needs something to resolve the import to (TS2882 otherwise);
// tools/typescript/sources.gni maps the bundle URL to this file.
export {};

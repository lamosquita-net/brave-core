/* Copyright (c) 2025 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "chrome/browser/extensions/component_loader_factory.h"

#include "base/memory/ptr_util.h"
#include "base/no_destructor.h"
#include "brave/browser/extensions/brave_component_loader.h"
#include "chrome/browser/extensions/component_loader.h"
#include "chrome/browser/profiles/profile.h"
#include "components/keyed_service/content/browser_context_dependency_manager.h"
#include "extensions/browser/extension_registrar_factory.h"
#include "extensions/browser/extension_system_provider.h"
#include "extensions/browser/extensions_browser_client.h"
#include "extensions/buildflags/buildflags.h"

// Build a BraveComponentLoader instead of a ComponentLoader. Function-like, so
// it only hits `new ComponentLoader(profile)`, not the type name.
#define ComponentLoader(...) BraveComponentLoader(__VA_ARGS__)
#include "src/chrome/browser/extensions/component_loader_factory.cc"
#undef ComponentLoader

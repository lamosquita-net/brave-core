/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_THIRD_PARTY_BLINK_RENDERER_CORE_FARBLING_BRAVE_SESSION_CACHE_H_
#define BRAVE_THIRD_PARTY_BLINK_RENDERER_CORE_FARBLING_BRAVE_SESSION_CACHE_H_

#include <memory>
#include <optional>
#include <string>

#include "base/containers/span.h"
#include "brave/third_party/blink/renderer/bindings/core/webgl/webgl_farbled_extension_handler.h"
#include "brave/third_party/blink/renderer/brave_farbling_constants.h"
#include "brave/third_party/blink/renderer/platform/brave_audio_farbling_helper.h"
#include "components/content_settings/core/common/content_settings_types.h"
#include "third_party/abseil-cpp/absl/random/random.h"
#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/core/execution_context/execution_context.h"
#include "third_party/blink/renderer/core/frame/dom_window.h"
#include "third_party/blink/renderer/platform/wtf/hash_map.h"
#include "third_party/blink/renderer/platform/wtf/text/atomic_string.h"

namespace blink {
class WebContentSettingsClient;
}  // namespace blink

namespace brave {

using blink::DOMWindow;
using blink::ExecutionContext;
using blink::GarbageCollected;
using blink::MakeGarbageCollected;
using blink::Supplement;

enum FarbleKey : uint64_t {
  kNone,
  kWindowInnerWidth,
  kWindowInnerHeight,
  kWindowScreenX,
  kWindowScreenY,
  kPointerScreenX,
  kPointerScreenY,
  kKeyCount
};

typedef absl::randen_engine<uint64_t> FarblingPRNG;

CORE_EXPORT blink::WebContentSettingsClient* GetContentSettingsClientFor(
    ExecutionContext* context,
    bool require_filled_content_settings_rules = false);
CORE_EXPORT BraveFarblingLevel
GetBraveFarblingLevelFor(ExecutionContext* context,
                         ContentSettingsType webcompat_settings_type,
                         BraveFarblingLevel default_value);
CORE_EXPORT bool AllowFingerprinting(
    ExecutionContext* context,
    ContentSettingsType webcompat_settings_type);
CORE_EXPORT bool AllowFontFamily(ExecutionContext* context,
                                 const blink::AtomicString& family_name);
CORE_EXPORT int FarbleInteger(ExecutionContext* context,
                              brave::FarbleKey key,
                              int spoof_value,
                              int min_value,
                              int max_value);
CORE_EXPORT bool BlockScreenFingerprinting(ExecutionContext* context);
CORE_EXPORT int FarbledPointerScreenCoordinate(const DOMWindow* view,
                                               FarbleKey key,
                                               int client_coordinate,
                                               int true_screen_coordinate);

class CORE_EXPORT BraveSessionCache final
    : public GarbageCollected<BraveSessionCache>,
      public Supplement<ExecutionContext> {
 public:
  static const char kSupplementName[];

  explicit BraveSessionCache(ExecutionContext&);
  virtual ~BraveSessionCache() = default;

  static BraveSessionCache& From(ExecutionContext&);
  static void Init();

  BraveFarblingLevel GetBraveFarblingLevel(
      ContentSettingsType webcompat_settings_type);
  void FarbleAudioChannel(base::span<float> dst);
  // |bytes_per_channel| is 1 for 8-bit RGBA, 2 for half float (F16) and 4 for
  // float (F32) pixels.
  void PerturbPixels(const unsigned char* data,
                     size_t size,
                     size_t bytes_per_channel = 1);
  blink::String GenerateRandomString(std::string seed,
                                     blink::wtf_size_t length);
  blink::String FarbledUserAgent(blink::String real_user_agent);
  int FarbledInteger(FarbleKey key,
                     int spoof_value,
                     int min_random_offset,
                     int max_random_offset);
  bool AllowFontFamily(blink::WebContentSettingsClient* settings,
                       const blink::AtomicString& family_name);
  FarblingPRNG MakePseudoRandomGenerator(FarbleKey key = FarbleKey::kNone);
  std::optional<blink::BraveAudioFarblingHelper> GetAudioFarblingHelper() {
    return audio_farbling_helper_;
  }
  // Returns a non owning reference to the underlying webgl/webgl2 handler.
  // Callers must not delete it. If a prior call to create this was already made
  // then this method will crash, otherwise handler is created and its
  // non-owning reference is returned. If you are only looking to get a
  // previously created handler then call the getter method
  // get_webgl_farbled_extension_handler.
  blink::WebGLFarbledExtensionHandler* CreateWebGLFarbledExtensionHandler(
      const bool is_webgl2);

  // Returns a non owning reference to the underlying webgl/webgl2 handler if
  // it's already created. Callers must not delete it.
  blink::WebGLFarbledExtensionHandler* get_webgl_farbled_extension_handler(
      bool is_webgl2) {
    return is_webgl2 ? webgl2_farbled_extension_handler_.get()
                     : webgl_farbled_extension_handler_.get();
  }

 private:
  bool farbling_enabled_;
  uint64_t session_key_;
  uint8_t domain_key_[32];
  blink::HashMap<FarbleKey, int> farbled_integers_;
  BraveFarblingLevel farbling_level_;
  std::optional<blink::BraveAudioFarblingHelper> audio_farbling_helper_;
  // A handler to farble the webgl supported extensions.
  std::unique_ptr<blink::WebGLFarbledExtensionHandler>
      webgl_farbled_extension_handler_;
  // A handler to farble the webgl2 supported extensions.
  std::unique_ptr<blink::WebGLFarbledExtensionHandler>
      webgl2_farbled_extension_handler_;
  blink::HashMap<ContentSettingsType, BraveFarblingLevel> farbling_levels_;

  void PerturbPixelsInternal(const unsigned char* data,
                             size_t size,
                             size_t bytes_per_channel);
};

}  // namespace brave

#endif  // BRAVE_THIRD_PARTY_BLINK_RENDERER_CORE_FARBLING_BRAVE_SESSION_CACHE_H_

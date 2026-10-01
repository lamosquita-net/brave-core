/* Copyright (c) 2026 lamosquita. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "brave/renderer/flyweb/flyweb_polyfills_render_frame_observer.h"

#include <string>

#include "base/no_destructor.h"
#include "components/grit/brave_components_resources.h"
#include "content/public/renderer/render_frame.h"
#include "third_party/blink/public/platform/web_security_origin.h"
#include "third_party/blink/public/platform/web_string.h"
#include "third_party/blink/public/web/web_local_frame.h"
#include "third_party/blink/public/web/web_script_source.h"
#include "ui/base/resource/resource_bundle.h"
#include "url/url_constants.h"

namespace flyweb {

BASE_FEATURE(kFlyWebPolyfills,
             "FlyWebPolyfills",
             base::FEATURE_ENABLED_BY_DEFAULT);

namespace {

// Loaded once per renderer process.
const std::string& PolyfillsSource() {
  static const base::NoDestructor<std::string> source([] {
    auto& bundle = ui::ResourceBundle::GetSharedInstance();
    if (bundle.IsGzipped(IDR_FLYWEB_POLYFILLS_JS)) {
      return bundle.LoadDataResourceString(IDR_FLYWEB_POLYFILLS_JS);
    }
    return std::string(bundle.GetRawDataResource(IDR_FLYWEB_POLYFILLS_JS));
  }());
  return *source;
}

}  // namespace

FlyWebPolyfillsRenderFrameObserver::FlyWebPolyfillsRenderFrameObserver(
    content::RenderFrame* render_frame)
    : content::RenderFrameObserver(render_frame) {}

FlyWebPolyfillsRenderFrameObserver::~FlyWebPolyfillsRenderFrameObserver() =
    default;

void FlyWebPolyfillsRenderFrameObserver::DidClearWindowObject() {
  blink::WebLocalFrame* frame = render_frame()->GetWebFrame();
  if (!frame || frame->IsProvisional()) {
    return;
  }
  // Web pages only (also about:blank and srcdoc frames that inherit a web
  // origin); never internal pages or extensions.
  const blink::WebString protocol = frame->GetSecurityOrigin().Protocol();
  if (protocol != url::kHttpsScheme && protocol != url::kHttpScheme) {
    return;
  }
  frame->ExecuteScript(
      blink::WebScriptSource(blink::WebString::FromUTF8(PolyfillsSource())));
}

void FlyWebPolyfillsRenderFrameObserver::OnDestruct() {
  delete this;
}

}  // namespace flyweb

/* Copyright (c) 2024 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "src/content/browser/picture_in_picture/video_picture_in_picture_window_controller_impl.cc"

namespace content {

void VideoPictureInPictureWindowControllerImpl::RequestFullscreen() {
  DCHECK(active_session_);

  active_session_->GetMediaPlayerRemote()->RequestFullscreen();
  FocusInitiator();
}

}  // namespace content

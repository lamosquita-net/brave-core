// Copyright (c) 2021 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// you can obtain one at http://mozilla.org/MPL/2.0/.

#include "src/ui/views/window/dialog_client_view.cc"

namespace views {

void DialogClientView::IgnoreNextWindowStationaryStateChanged() {
  DCHECK(input_protector_);
  input_protector_->IgnoreNextWindowStationaryStateChanged();
}

}  // namespace views

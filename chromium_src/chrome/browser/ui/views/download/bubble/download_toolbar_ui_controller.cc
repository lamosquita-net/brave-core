/* Copyright (c) 2025 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "chrome/browser/ui/views/download/bubble/download_toolbar_ui_controller.h"

#include <algorithm>
#include <vector>

#include "chrome/browser/download/bubble/download_bubble_update_service.h"
#include "chrome/browser/download/bubble/download_bubble_update_service_factory.h"
#include "chrome/browser/ui/views/frame/browser_view.h"
#include "components/download/public/common/download_item.h"
#include "components/vector_icons/vector_icons.h"
#include "ui/actions/actions.h"
#include "ui/base/models/image_model.h"
#include "ui/color/color_id.h"

namespace {

bool HasInsecureDownloads(BrowserView* browser_view) {
  auto* update_service = DownloadBubbleUpdateServiceFactory::GetForProfile(
      browser_view->GetProfile());
  if (!update_service || !update_service->IsInitialized()) {
    return false;
  }

  std::vector<DownloadUIModel::DownloadUIModelPtr> all_models;
  update_service->GetAllModelsToDisplay(all_models, /*web_app_id=*/nullptr,
                                        /*force_backfill_download_items=*/true);

  return std::ranges::any_of(all_models, [](const auto& model) {
    return (model->GetInsecureDownloadStatus() ==
                download::DownloadItem::InsecureDownloadStatus::BLOCK ||
            model->GetInsecureDownloadStatus() ==
                download::DownloadItem::InsecureDownloadStatus::WARN);
  });
}

// Use a warning icon while there's an insecure download in the download
// models. Called at the end of DownloadToolbarUIController::UpdateIcon().
void UpdateIcon_BraveImpl(BrowserView* browser_view,
                          actions::ActionItem* action_item) {
  if (!action_item || !HasInsecureDownloads(browser_view)) {
    return;
  }

  action_item->SetImage(ui::ImageModel::FromVectorIcon(
      vector_icons::kNotSecureWarningOldIcon,
      ui::kColorAlertMediumSeverityIcon));
}

}  // namespace

#include "src/chrome/browser/ui/views/download/bubble/download_toolbar_ui_controller.cc"

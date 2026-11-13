/* Copyright (c) 2026 The FlyWeb Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "brave/browser/infobars/flyweb_sync_account_infobar_delegate.h"

#include <memory>
#include <utility>

#include "base/functional/callback_helpers.h"
#include "base/i18n/time_formatting.h"
#include "base/strings/string_number_conversions.h"
#include "base/strings/utf_string_conversions.h"
#include "base/time/time.h"
#include "brave/browser/sync/account/flyweb_sync_account_service_factory.h"
#include "brave/components/constants/webui_url_constants.h"
#include "brave/grit/brave_generated_resources.h"
#include "chrome/browser/infobars/confirm_infobar_creator.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/browser/ui/chrome_pages.h"
#include "components/infobars/content/content_infobar_manager.h"
#include "components/infobars/core/infobar.h"
#include "content/public/browser/web_contents.h"
#include "ui/base/l10n/l10n_util.h"
#include "ui/views/vector_icons.h"

namespace {

std::u16string Name(const flyweb_sync::AccountEvent& e) {
  return base::UTF8ToUTF16(e.mac.empty() ? e.model : e.mac);
}

}  // namespace

// static
std::u16string FlyWebSyncAccountInfoBarDelegate::WarningText(
    const flyweb_sync::AccountEvent& e) {
  if (e.type == "mac-anadido") {
    return l10n_util::GetStringFUTF16(IDS_FLYWEB_SYNC_ACCOUNT_WARNING_MAC_ADDED,
                                      Name(e));
  }
  if (e.type == "contrasena-acertada") {
    return l10n_util::GetStringUTF16(
        IDS_FLYWEB_SYNC_ACCOUNT_WARNING_PASSWORD_GUESSED);
  }
  if (e.type == "contrasena-fallos") {
    return l10n_util::GetStringFUTF16(
        IDS_FLYWEB_SYNC_ACCOUNT_WARNING_PASSWORD_FAILURES,
        base::NumberToString16(e.count));
  }
  if (e.type == "contrasena-cambiada") {
    return l10n_util::GetStringUTF16(
        IDS_FLYWEB_SYNC_ACCOUNT_WARNING_PASSWORD_CHANGED);
  }
  if (e.type == "email-cambiado") {
    return l10n_util::GetStringUTF16(
        IDS_FLYWEB_SYNC_ACCOUNT_WARNING_EMAIL_CHANGED);
  }
  if (e.type == "cadena-cambiada") {
    return l10n_util::GetStringUTF16(
        IDS_FLYWEB_SYNC_ACCOUNT_WARNING_CHAIN_CHANGED);
  }
  if (e.type == "cadena-ligada") {
    return l10n_util::GetStringUTF16(
        IDS_FLYWEB_SYNC_ACCOUNT_WARNING_CHAIN_LINKED);
  }
  if (e.type == "aparato-quitado") {
    return l10n_util::GetStringFUTF16(
        IDS_FLYWEB_SYNC_ACCOUNT_WARNING_DEVICE_REMOVED, Name(e));
  }
  if (e.type == "no-he-sido-yo") {
    return l10n_util::GetStringUTF16(IDS_FLYWEB_SYNC_ACCOUNT_WARNING_NOT_ME);
  }
  if (e.type == "borrado-pedido") {
    return l10n_util::GetStringFUTF16(
        IDS_FLYWEB_SYNC_ACCOUNT_WARNING_DELETION_REQUESTED,
        base::LocalizedTimeFormatWithPattern(
            base::Time::FromSecondsSinceUnixEpoch(e.when), "d MMMM"));
  }
  return l10n_util::GetStringUTF16(
      IDS_FLYWEB_SYNC_ACCOUNT_WARNING_DELETION_CANCELLED);
}

// static
void FlyWebSyncAccountInfoBarDelegate::Create(
    content::WebContents* web_contents,
    Profile* profile,
    Kind kind,
    const std::vector<flyweb_sync::AccountEvent>& warnings) {
  if (!web_contents) {
    return;
  }
  // On the page itself the warnings are already shown.
  if (web_contents->GetURL() == chrome::GetSettingsUrl(kBraveSyncSetupPath)) {
    return;
  }
  auto* manager =
      infobars::ContentInfoBarManager::FromWebContents(web_contents);
  if (!manager) {
    return;
  }
  // One bar of each kind: replace an older one.
  for (size_t i = 0; i < manager->infobars().size(); ++i) {
    infobars::InfoBar* bar = manager->infobars()[i];
    if (bar->delegate()->GetIdentifier() ==
        (kind == Kind::kWarnings ? FLYWEB_SYNC_ACCOUNT_WARNING_INFOBAR
                                 : FLYWEB_SYNC_ACCOUNT_NOTICE_INFOBAR)) {
      manager->RemoveInfoBar(bar);
      break;
    }
  }
  std::u16string message;
  if (kind == Kind::kWarnings) {
    if (warnings.empty()) {
      return;
    }
    message = WarningText(warnings.back());
    if (warnings.size() > 1) {
      message += u" ";
      message += l10n_util::GetStringFUTF16(
          IDS_FLYWEB_SYNC_ACCOUNT_WARNING_MORE,
          base::NumberToString16(warnings.size() - 1));
    }
  } else {
    message =
        l10n_util::GetStringUTF16(IDS_FLYWEB_SYNC_ACCOUNT_KEY_CHANGED_INFOBAR);
  }
  manager->AddInfoBar(
      CreateConfirmInfoBar(base::WrapUnique(new FlyWebSyncAccountInfoBarDelegate(
          profile, kind, std::move(message)))));
}

FlyWebSyncAccountInfoBarDelegate::FlyWebSyncAccountInfoBarDelegate(
    Profile* profile,
    Kind kind,
    std::u16string message)
    : profile_(profile), kind_(kind), message_(std::move(message)) {}

FlyWebSyncAccountInfoBarDelegate::~FlyWebSyncAccountInfoBarDelegate() =
    default;

flyweb_sync::AccountService*
FlyWebSyncAccountInfoBarDelegate::GetAccountService() const {
  return flyweb_sync::AccountServiceFactory::GetForBrowserContext(profile_);
}

infobars::InfoBarDelegate::InfoBarIdentifier
FlyWebSyncAccountInfoBarDelegate::GetIdentifier() const {
  return kind_ == Kind::kWarnings ? FLYWEB_SYNC_ACCOUNT_WARNING_INFOBAR
                                  : FLYWEB_SYNC_ACCOUNT_NOTICE_INFOBAR;
}

const gfx::VectorIcon& FlyWebSyncAccountInfoBarDelegate::GetVectorIcon()
    const {
  return views::kInfoIcon;
}

// It stays until the user answers: a warning that goes away with a click on a
// link would be easy to miss (§6.2).
bool FlyWebSyncAccountInfoBarDelegate::ShouldExpire(
    const NavigationDetails& details) const {
  return false;
}

std::u16string FlyWebSyncAccountInfoBarDelegate::GetMessageText() const {
  return message_;
}

int FlyWebSyncAccountInfoBarDelegate::GetButtons() const {
  return kind_ == Kind::kWarnings ? (BUTTON_OK | BUTTON_CANCEL) : BUTTON_NONE;
}

std::u16string FlyWebSyncAccountInfoBarDelegate::GetButtonLabel(
    InfoBarButton button) const {
  return l10n_util::GetStringUTF16(
      button == BUTTON_OK ? IDS_FLYWEB_SYNC_ACCOUNT_WARNING_WAS_ME
                          : IDS_FLYWEB_SYNC_ACCOUNT_WARNING_NOT_ME_BUTTON);
}

std::u16string FlyWebSyncAccountInfoBarDelegate::GetLinkText() const {
  return l10n_util::GetStringUTF16(IDS_FLYWEB_SYNC_ACCOUNT_WARNING_OPEN);
}

GURL FlyWebSyncAccountInfoBarDelegate::GetLinkURL() const {
  return chrome::GetSettingsUrl(kBraveSyncSetupPath);
}

// «He sido yo».
bool FlyWebSyncAccountInfoBarDelegate::Accept() {
  if (auto* account = GetAccountService()) {
    account->DismissWarnings();
  }
  return true;
}

// «No he sido yo»: at once (§6.2), and the page for what comes next.
bool FlyWebSyncAccountInfoBarDelegate::Cancel() {
  if (auto* account = GetAccountService()) {
    account->NotMe(base::DoNothing());
  }
  chrome::ShowSettingsSubPageForProfile(profile_, kBraveSyncSetupPath);
  return true;
}

// Closing the bar is not «He sido yo»: the warnings stay on the page.
void FlyWebSyncAccountInfoBarDelegate::InfoBarDismissed() {}

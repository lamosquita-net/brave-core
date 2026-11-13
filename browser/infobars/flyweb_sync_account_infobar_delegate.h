/* Copyright (c) 2026 The FlyWeb Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_BROWSER_INFOBARS_FLYWEB_SYNC_ACCOUNT_INFOBAR_DELEGATE_H_
#define BRAVE_BROWSER_INFOBARS_FLYWEB_SYNC_ACCOUNT_INFOBAR_DELEGATE_H_

#include <string>
#include <vector>

#include "base/memory/raw_ptr.h"
#include "brave/browser/sync/account/flyweb_sync_account_service.h"
#include "components/infobars/core/confirm_infobar_delegate.h"

class Profile;

namespace content {
class WebContents;
}  // namespace content

// FlyWeb 3.2 (sync-cuenta.md §6.2, §6.3, §11): the bars of the sync account,
// on the pattern of BraveSyncAccountDeletedInfoBarDelegate.
// - kWarnings: «¿Has sido tú?» with the newest event, «He sido yo» (hides the
//   warnings) and «No he sido yo» (invalidates every token and opens the page,
//   which guides to change the password and the key).
// - kKeyChanged: the sync key was changed from another Mac; log in again.
class FlyWebSyncAccountInfoBarDelegate : public ConfirmInfoBarDelegate {
 public:
  enum class Kind { kWarnings, kKeyChanged };

  FlyWebSyncAccountInfoBarDelegate(const FlyWebSyncAccountInfoBarDelegate&) =
      delete;
  FlyWebSyncAccountInfoBarDelegate& operator=(
      const FlyWebSyncAccountInfoBarDelegate&) = delete;
  ~FlyWebSyncAccountInfoBarDelegate() override;

  static void Create(content::WebContents* web_contents,
                     Profile* profile,
                     Kind kind,
                     const std::vector<flyweb_sync::AccountEvent>& warnings);

  // Text of one warning, also for the page's list.
  static std::u16string WarningText(const flyweb_sync::AccountEvent& event);

 private:
  FlyWebSyncAccountInfoBarDelegate(Profile* profile,
                                   Kind kind,
                                   std::u16string message);

  // ConfirmInfoBarDelegate:
  infobars::InfoBarDelegate::InfoBarIdentifier GetIdentifier() const override;
  const gfx::VectorIcon& GetVectorIcon() const override;
  bool ShouldExpire(const NavigationDetails& details) const override;
  std::u16string GetMessageText() const override;
  int GetButtons() const override;
  std::u16string GetButtonLabel(InfoBarButton button) const override;
  std::u16string GetLinkText() const override;
  GURL GetLinkURL() const override;
  bool Accept() override;
  bool Cancel() override;
  void InfoBarDismissed() override;

  flyweb_sync::AccountService* GetAccountService() const;

  raw_ptr<Profile> profile_;
  const Kind kind_;
  const std::u16string message_;
};

#endif  // BRAVE_BROWSER_INFOBARS_FLYWEB_SYNC_ACCOUNT_INFOBAR_DELEGATE_H_

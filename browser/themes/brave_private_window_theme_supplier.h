/* Copyright (c) 2022 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_BROWSER_THEMES_BRAVE_PRIVATE_WINDOW_THEME_SUPPLIER_H_
#define BRAVE_BROWSER_THEMES_BRAVE_PRIVATE_WINDOW_THEME_SUPPLIER_H_

#include "chrome/browser/themes/custom_theme_supplier.h"

// A theme supplier for private window.
class BravePrivateWindowThemeSupplier : public CustomThemeSupplier {
 public:
  BravePrivateWindowThemeSupplier();

  BravePrivateWindowThemeSupplier(const BravePrivateWindowThemeSupplier&) =
      delete;
  BravePrivateWindowThemeSupplier& operator=(
      const BravePrivateWindowThemeSupplier&) = delete;

 protected:
  ~BravePrivateWindowThemeSupplier() override;

  void AddColorMixers(ui::ColorProvider* provider,
                      const ui::ColorProviderKey& key) const override;
};

#endif  // BRAVE_BROWSER_THEMES_BRAVE_PRIVATE_WINDOW_THEME_SUPPLIER_H_

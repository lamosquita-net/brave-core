// Copyright (c) 2022 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// you can obtain one at https://mozilla.org/MPL/2.0/.
import * as React from 'react'
import getPageHandlerInstance from './api/brave_page_handler'

export function useHasDisclaimerDismissed () {
  // We use a tri-state here to help render disclaimer UI only if there are non-null values,
  // which happens after promise is resolved
  const [hasDisclaimerDismissed, setHasDisclaimerDismissed] = React.useState<boolean | null>(null)

  React.useEffect(() => {
    getPageHandlerInstance().pageHandler.getDisclaimerDismissed()
      .then(res => setHasDisclaimerDismissed(res.dismissed))
  }, [])

  return { hasDisclaimerDismissed }
}

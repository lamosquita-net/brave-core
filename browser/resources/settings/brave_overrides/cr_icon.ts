// Copyright (c) 2024 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

import { CrIconElement } from '//resources/cr_elements/cr_icon/cr_icon.js'
import { iconMap, leoIcons } from './iron_icon.js'
import { css } from '//resources/lit/v3_0/lit.rollup.js'
import { injectStyle } from '//resources/brave/lit_overriding.js'

injectStyle(CrIconElement, css`:host {
  --leo-icon-size: var(--iron-icon-width, 24px);
  --leo-icon-color: var(--iron-icon-fill-color, currentColor);
}`)

// FlyWeb: in Chromium 156 the method is the private updateIcon_ (as Brave
// 1.98 does in its cr_icon override), reached through a cast.
interface CrIconInternals {
  updateIcon_: (this: CrIconElement, ...args: unknown[]) => void
}
const proto = CrIconElement.prototype as unknown as CrIconInternals
const old = proto.updateIcon_
proto.updateIcon_ = function (this: CrIconElement, ...args: unknown[]) {
  const removeAllOfType = (type: string) => {
    for (const node of this.shadowRoot.querySelectorAll(type)) node.remove()
  }

  // Upstream's kWebUIRoundedIcons rollout may add a legacy "-old" suffix to
  // the icon name; both refer to the same icon (as in Brave 1.98).
  const unroundedIcon = this.icon.replace(/-old$/, '')
  const name = iconMap[this.icon] ?? iconMap[unroundedIcon]
  if (name || leoIcons.has(this.icon)) {
    removeAllOfType('svg') 

    let leoIcon = this.shadowRoot.querySelector('leo-icon')
    if (!leoIcon) {
      leoIcon = document.createElement('leo-icon')
      this.shadowRoot.append(leoIcon)
    }
    leoIcon.setAttribute('name', name ?? this.icon)
  } else {
    removeAllOfType('leo-icon')
    old.apply(this, args)
  }
}

// Copyright (c) 2025 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

// Elements used by the upstream template, imported explicitly so that
// @webui-eslint/web-component-missing-deps sees them (same imports as the
// upstream file, now kept as -chromium.ts).
import 'chrome://resources/cr_components/managed_footnote/managed_footnote.js';
import 'chrome://resources/cr_elements/cr_toggle/cr_toggle.js';
import './item.js';
import './mv2_deprecation_panel.js';
import './review_panel.js';
import 'chrome://resources/cr_elements/cr_a11y_announcer/cr_a11y_announcer.js';
import 'chrome://resources/cr_elements/i18n_mixin_lit.js';
import 'chrome://resources/js/load_time_data.js';
import 'chrome://resources/lit/v3_0/lit.rollup.js';

import { ExtensionsItemListElement } from './item_list-chromium.js'

(ExtensionsItemListElement.prototype as unknown as { shouldShowEmptyItemsMessage_: () => boolean }).shouldShowEmptyItemsMessage_ = () => true

export * from './item_list-chromium.js'

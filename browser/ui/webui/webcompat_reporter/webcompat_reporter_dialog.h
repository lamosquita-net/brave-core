/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_BROWSER_UI_WEBUI_WEBCOMPAT_REPORTER_WEBCOMPAT_REPORTER_DIALOG_H_
#define BRAVE_BROWSER_UI_WEBUI_WEBCOMPAT_REPORTER_WEBCOMPAT_REPORTER_DIALOG_H_

namespace content {
class WebContents;
}

// FlyWeb: opens flyweb.lamosquita.net's "report a problem" page.
void OpenWebcompatReporterDialog(content::WebContents* initiator);

// Brave's reporter dialog (sends to webcompat_report_api_endpoint). Unused.
void OpenBraveWebcompatReporterDialog(content::WebContents* initiator);

#endif  // BRAVE_BROWSER_UI_WEBUI_WEBCOMPAT_REPORTER_WEBCOMPAT_REPORTER_DIALOG_H_

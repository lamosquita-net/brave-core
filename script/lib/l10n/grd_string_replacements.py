#!/usr/bin/env python3
#
# Copyright (c) 2022 The Brave Authors. All rights reserved.
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this file,
# You can obtain one at http://mozilla.org/MPL/2.0/. */

import re

# Strings we want to replace but that we also replace automatically
# for XTB files
branding_replacements = [
    (r'The Chromium Authors. All rights reserved.',
     r'The Brave Authors. All rights reserved.'),
    (r'Google LLC. All rights reserved.',
     r'The Brave Authors. All rights reserved.'),
    (r'The Chromium Authors', r'Brave Software Inc'),
    (r'Google Chrome', r'Brave'),
    (r'(Google)(?! Play)', r'Brave'),
    (r'Chromium', r'Brave'),
    (r'Chrome', r'Brave'),
    (r'क्रोमियम', 'Brave'), # Chromium in Hindi
]


# Strings we want to replace but that we need to use Transifex for
# to translate the XTB files
default_replacements = [
    (r'Brave Web Store', r'Web Store'),
    (r'You\'re incognito', r'This is a private window'),
    (r'an incognito', r'a private'),
    (r'an Incognito', r'a Private'),
    (r'incognito', r'private'),
    (r'Incognito', r'Private'),
    (r'inco&gnito', r'&private'),
    (r'Inco&gnito', r'&Private'),
    (r'People', r'Profiles'),
    # 'people' but only in the context of profiles, not humans.
    (r'(?<!authenticate )people', r'profiles'),
    (r'(Person)(?!\w)', r'Profile'),
    (r'(person)(?!\w)', r'profile'),
    (r'Bookmarks Bar\n', r'Bookmarks\n'),
    (r'Bookmarks bar\n', r'Bookmarks\n'),
    (r'bookmarks bar\n', r'bookmarks\n'),
]


# Fix up some strings after aggressive first round replacement.
fixup_replacements = [
    (r'Brave Cloud Print', r'Google Cloud Print'),
    (r'Brave Docs', r'Google Docs'),
    (r'Brave Drive', r'Google Drive'),
    (r'Brave OS', r'Chrome OS'),
    (r'BraveOS', r'ChromeOS'),
    (r'Brave Safe Browsing', r'Google Safe Browsing'),
    (r'Safe Browsing \(protects you and your device from dangerous sites\)',
     r'Google Safe Browsing (protects you and your device from dangerous sites)'
    ),
    (r'Sends URLs of some pages you visit to Brave',
     r'Sends URLs of some pages you visit to Google'),
    (r'Google Google', r'Google'),
    (r'Brave Account', r'Brave sync chain'),
    (r'Brave Lens', r'Google Lens'),
    (r'Bravebook', r'Chromebook'),
    (r'Bravecast', r'Chromecast'),
    (r'Brave Cloud', r'Google Cloud'),
    (r'Brave Pay', r'Google Pay'),
    (r'Brave Photos', r'Google Photos'),
    (r'Brave Projects', r'Chromium Projects'),
    (r'BraveVox', r'ChromeVox'),
]


# Replacements for text nodes and neither for inside descriptions nor comments
main_text_only_replacements = [
    # By converting it back first, it makes this idempotent
    ('Copyright \xa9', 'Copyright'),
    ('Copyright', 'Copyright \xa9'),
]


# FlyWeb: the browser is called FlyWeb. Brave's own services keep their name
# (they are Brave's, and FlyWeb disables most of them), as do legal notices.
# Applied after all the replacements above, so Chromium strings end up as
# Chrome -> Brave -> FlyWeb. Idempotent.
flyweb_replacements = [
    (r'\bBrave\b(?!\s+(?:Wallet|Rewards|News|VPN|Vpn|Search|Talk|Ads|Software'
     r'|Authors|Sync|Today|Leo)\b)', r'FlyWeb'),
]

# Messages that name the company are legal notices ("Brave is a registered
# trademark of Brave Software", disclaimers): they keep "Brave" in every
# language. The only exception is the company line of the about pages, which
# names who ships FlyWeb.
FLYWEB_LEGAL_MARKER = 'Brave Software'
FLYWEB_COMPANY = ('Brave Software Inc', 'lamosquita')

_FLYWEB_MESSAGE_RE = re.compile(r'(<(message|translation)\b[^>]*>)(.*?)(</\2>)',
                                re.S)


def _flyweb_plain(text):
    for (pattern, to) in flyweb_replacements:
        text = re.sub(pattern, to, text)
    return text


def flyweb_message_kind(text):
    """'company', 'legal' or None, from a message's English text."""
    if text.strip() == FLYWEB_COMPANY[0]:
        return 'company'
    if FLYWEB_LEGAL_MARKER in text:
        return 'legal'
    return None


def flyweb_rebrand_as(kind, text):
    """Rebrands a message (or its translation) of the given kind."""
    if kind == 'company':
        # Whole text, whitespace kept: translations say e.g. "Brave Authors".
        stripped = text.strip()
        return (text.replace(stripped, FLYWEB_COMPANY[1])
                if stripped else FLYWEB_COMPANY[1])
    if kind == 'legal':
        return text
    return _flyweb_plain(text)


def _flyweb_message(text):
    return flyweb_rebrand_as(flyweb_message_kind(text), text)


def flyweb_rebrand(text):
    """Applies flyweb_replacements to an English message text, or to a whole
    .grd/.grdp file message by message. Idempotent. Translations (.xtb) must
    follow their English message: see flyweb_rebrand_as()."""
    if not _FLYWEB_MESSAGE_RE.search(text):
        return _flyweb_message(text)
    out, pos = [], 0
    for m in _FLYWEB_MESSAGE_RE.finditer(text):
        out.append(_flyweb_plain(text[pos:m.start()]))
        out.append(_flyweb_plain(m.group(1)) + _flyweb_message(m.group(3)) +
                   m.group(4))
        pos = m.end()
    out.append(_flyweb_plain(text[pos:]))
    return ''.join(out)

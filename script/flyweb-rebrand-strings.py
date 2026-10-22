#!/usr/bin/env python3
# Copyright (c) 2026 lamosquita. All rights reserved.
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this file,
# You can obtain one at https://mozilla.org/MPL/2.0/.
"""Rebrands UI strings from Brave to FlyWeb in every translated .grd/.grdp
and keeps their .xtb translations attached.

XTB translations are keyed by a fingerprint of the English message text, so
changing the English text orphans every translation. For each message whose
text changes, this script recomputes the fingerprint (same function Brave's
l10n tooling uses) and remaps it in all .xtb files of the owning .grd, while
applying the same textual replacement to the translations.

Files are edited textually (no XML re-serialization) to keep diffs minimal.
Idempotent: running it twice changes nothing the second time.

Usage: flyweb-rebrand-strings.py [--check]
  --check  report what would change and exit 1 if anything would.
Needs lxml and Chromium's tools/grit (for FP) in the checkout, or FP.py on
PYTHONPATH when run from a bare brave-core clone.
"""

import argparse
import os
import re
import sys

import lxml.etree  # pylint: disable=import-error

BRAVE_ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), '..'))
SRC_ROOT = os.path.dirname(BRAVE_ROOT)
sys.path.insert(1, os.path.join(SRC_ROOT, 'tools', 'grit', 'grit', 'extern'))
sys.path.insert(1, os.path.join(BRAVE_ROOT, 'script'))

# pylint: disable=wrong-import-position
from lib.l10n.grd_string_replacements import (flyweb_message_kind,
                                              flyweb_rebrand,
                                              flyweb_rebrand_as)
from lib.l10n.grd_utils import get_fingerprint_for_xtb  # needs FP

XTB_ID_RE = re.compile(r'(<translation id=")(\d+)(")')
XTB_LANG_RE = re.compile(r'<translationbundle lang="([^"]+)"')
XTB_MSG_RE = re.compile(r'(<translation id="(\d+)"[^>]*>)(.*?)(</translation>)',
                        re.S)


def rebrand(text):
    return flyweb_rebrand(text)


def parse_messages(xml_bytes):
    parser = lxml.etree.XMLParser(remove_blank_text=False,
                                  resolve_entities=False)
    return list(lxml.etree.fromstring(xml_bytes, parser).iter('message'))


def message_fps(xml_bytes):
    return [get_fingerprint_for_xtb(m) for m in parse_messages(xml_bytes)]


def message_kinds(old_bytes, new_fps):
    """{new fingerprint: kind}, the kind judged on the original English."""
    return {fp: flyweb_message_kind(inner_xml(m))
            for m, fp in zip(parse_messages(old_bytes), new_fps)}


def inner_xml(elem):
    # Same text flyweb_rebrand() judges: the message body with its markup.
    return (elem.text or '') + ''.join(
        lxml.etree.tostring(c, encoding='unicode') for c in elem)


def rebrand_xtb(text, kinds):
    """Rebrands each translation as its English message was rebranded."""
    lang = XTB_LANG_RE.search(text).group(1)
    return XTB_MSG_RE.sub(
        lambda m: m.group(1) + flyweb_rebrand_as(
            kinds.get(m.group(2)), m.group(3), lang) + m.group(4),
        text)


def parts_of(path, root):
    """Yields path and, recursively, every <part file=...> it includes."""
    yield path
    for part in root.iter('part'):
        child = os.path.join(os.path.dirname(path), part.get('file'))
        if os.path.exists(child):
            child_root = lxml.etree.parse(child).getroot()
            yield from parts_of(child, child_root)


def process(check):
    fp_maps = {}      # file path -> {old_fp: new_fp}
    kind_maps = {}    # file path -> {new_fp: kind}
    new_content = {}  # file path -> rebranded text (grd/grdp)
    grds = []
    for dirpath, dirnames, filenames in os.walk(BRAVE_ROOT):
        dirnames[:] = [d for d in dirnames
                       if d not in ('node_modules', '.git', 'vendor', 'test',
                                    'tests', 'third_party')]
        grds += [os.path.join(dirpath, f) for f in filenames
                 if f.endswith('.grd')]

    changed_files = set()
    for grd in sorted(grds):
        try:
            grd_root = lxml.etree.parse(grd).getroot()
        except lxml.etree.XMLSyntaxError:
            continue  # empty or generated file
        xtbs = [os.path.join(os.path.dirname(grd), f.get('path'))
                for f in grd_root.iter('file')
                if f.get('path', '').endswith('.xtb')]
        if not xtbs:
            continue  # resources only, no translated strings
        grd_map = {}
        grd_kinds = {}
        for path in parts_of(grd, grd_root):
            if path not in fp_maps:
                with open(path, encoding='utf-8') as f:
                    old = f.read()
                new = rebrand(old)
                old_fps = message_fps(old.encode('utf-8'))
                new_fps = message_fps(new.encode('utf-8'))
                assert len(old_fps) == len(new_fps), path
                fp_maps[path] = {o: n for o, n in zip(old_fps, new_fps)
                                 if o != n}
                kind_maps[path] = message_kinds(old.encode('utf-8'), new_fps)
                if new != old:
                    new_content[path] = new
            grd_map.update(fp_maps[path])
            grd_kinds.update(kind_maps[path])

        for xtb in xtbs:
            if not os.path.exists(xtb):
                continue
            with open(xtb, encoding='utf-8') as f:
                old = f.read()
            new = XTB_ID_RE.sub(
                lambda m: m.group(1) + grd_map.get(m.group(2), m.group(2)) +
                m.group(3), old)
            new = rebrand_xtb(new, grd_kinds)
            if new != old:
                changed_files.add(xtb)
                if not check:
                    with open(xtb, 'w', encoding='utf-8') as f:
                        f.write(new)

    for path, text in new_content.items():
        changed_files.add(path)
        if not check:
            with open(path, 'w', encoding='utf-8') as f:
                f.write(text)

    remapped = sum(len(m) for m in fp_maps.values())
    print(f'{len(changed_files)} files, {remapped} messages remapped')
    return changed_files


def main():
    parser = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    parser.add_argument('--check', action='store_true')
    args = parser.parse_args()
    changed = process(args.check)
    if args.check and changed:
        for path in sorted(changed):
            print(os.path.relpath(path, BRAVE_ROOT))
        sys.exit(1)


if __name__ == '__main__':
    main()

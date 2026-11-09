# Copyright (c) 2024 The Brave Authors. All rights reserved.
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this file,
# You can obtain one at https://mozilla.org/MPL/2.0/.

# WebUI "-chromium" overrides (from Brave 1.98): when
# //brave/chromium_src/<path>/foo.ts exists for an upstream WebUI file
# <path>/foo.ts, the preprocessed copy of foo.ts is replaced by the Brave one.
# If the Brave file imports './foo-chromium.js', the upstream file is kept
# next to it as foo-chromium.ts (see brave_chromium_utils and the ts_library
# and minify_js overrides, which add it to their inputs and manifests).
#
# FlyWeb: Brave 1.98 also runs lit_mangler (*.lit_mangler.ts) here; FlyWeb
# does not ship lit_mangler, so only whole-file overrides are supported.

import argparse
import json
import os
import shutil

import brave_chromium_utils
import override_utils


def purge_overrides(out_folder, in_files):
    """Deletes all overridden upstream files from the preprocess directory - we
       need this so removing an override doesn't break the build."""
    for file in brave_chromium_utils.get_webui_overridden_but_referenced_files(
            out_folder, in_files):
        os.remove(os.path.join(out_folder, file))


def maybe_keep_upstream_version(override_in_folder, out_folder, override_file):
    """Decides whether we should keep the upstream version of a file by looking
       at the override and see if it references the upstream version.
       Returns the path to the upstream file, if we keep it."""
    overridden_name = brave_chromium_utils.get_webui_overriden_file_name(
        override_file)

    # The path to the brave-core override file
    override_in_path = os.path.join(override_in_folder, override_file)

    # The path to the upstream file
    upstream_file_path = os.path.join(out_folder, override_file)

    with open(override_in_path) as f:
        text = f.read()

        # If we reference the upstream file in our overridden file, make sure
        # we keep it around.
        if os.path.basename(overridden_name).replace('.ts', '.js') in text:
            shutil.copy(upstream_file_path,
                        os.path.join(out_folder, overridden_name))
            return overridden_name
    return None


def get_chromium_src_files(in_folder, in_files):
    """Gets all the overrides we have in brave-core for this target"""
    override_files = []

    for file in in_files:
        override_file = brave_chromium_utils.get_chromium_src_override(
            os.path.join(in_folder, file))
        if os.path.exists(override_file):
            override_files.append(file)

    return override_files


# Used to indicate that there are no overrides, so we don't need to reprocess
class NoOverridesException(Exception):
    pass


@override_utils.override_function(globals())
def main(original_function, argv):
    # FlyWeb: the GN template (tools-grit-preprocess_if_expr.gni.patch) passes
    # --brave-depfile so that ninja reruns this action when a chromium_src
    # override changes. 1.98 relies on its siso handler for that; FlyWeb builds
    # with ninja. Strip it before upstream's argparse sees it.
    depfile = None
    if '--brave-depfile' in argv:
        i = argv.index('--brave-depfile')
        depfile = argv[i + 1]
        argv = argv[:i] + argv[i + 2:]

    # Run the preprocessor as normal, to preprocess the upstream files.
    original_function(argv)

    manifest_data = {}
    manifest_path = None
    depfile_output = None
    depfile_inputs = []

    # When we rerun the processor, we detect files which should be overridden.
    @override_utils.override_method(argparse.ArgumentParser)
    # pylint: disable=unused-variable
    def parse_args(self, original_method, argv):
        nonlocal manifest_path, manifest_data, depfile_output, depfile_inputs
        args = original_method(self, argv)
        # First output of the GN action, relative to the build directory.
        # build_webui's preprocess_static_files gets an empty --in-files when
        # its static_files are only images (*.png/*.svg/*.jpg are not
        # preprocessed); its only output is then the manifest. Without this
        # fallback no depfile was written, ninja saw it "missing" and reran
        # the action (and everything after it) on every build.
        if args.in_files:
            depfile_output = os.path.normpath(
                os.path.join(args.out_folder, args.in_files[0]))
        elif args.out_manifest:
            depfile_output = os.path.normpath(args.out_manifest)

        cwd = os.getcwd()
        out_folder = os.path.normpath(os.path.join(cwd, args.out_folder))
        in_folder = os.path.normpath(os.path.join(cwd, args.in_folder))

        purge_overrides(out_folder, args.in_files)
        overrides = get_chromium_src_files(in_folder, args.in_files)

        if len(overrides) == 0:
            # Throw an exception to abort the second call to `main()` early if
            # no overrides were found.
            raise NoOverridesException()

        override_root_folder = brave_chromium_utils.get_chromium_src_override(
            in_folder)
        depfile_inputs = [
            os.path.relpath(os.path.join(override_root_folder, f), cwd)
            for f in overrides
        ]

        if args.out_manifest:
            manifest_path = os.path.join(cwd, args.out_manifest)
            with open(manifest_path, encoding='utf-8') as f:
                manifest_data = json.load(f)

        for override_file in overrides:
            overridden_name = maybe_keep_upstream_version(
                override_root_folder, out_folder, override_file)
            if overridden_name and args.out_manifest:
                manifest_data['files'].append(overridden_name)

        args.in_folder = override_root_folder
        args.in_files = overrides
        return args

    try:
        # Try and run the function again - if we have no overrides this will
        # raise an exception so we don't reprocess the files.
        original_function(argv)

        # We need to update the manifest to include all the -chromium files.
        if manifest_path:
            with open(manifest_path, 'w', encoding='utf-8', newline='\n') as f:
                json.dump(manifest_data, f)

    except NoOverridesException:
        # we throw an exception when there are no overrides, so we don't reparse
        pass

    if depfile and depfile_output:
        # The depfile's directory is not there yet in a clean out dir.
        os.makedirs(os.path.dirname(os.path.abspath(depfile)), exist_ok=True)
        with open(depfile, 'w', encoding='utf-8', newline='\n') as f:
            f.write(depfile_output.replace(' ', '\\ ') + ': ' +
                    ' '.join(i.replace(' ', '\\ ')
                             for i in depfile_inputs) + '\n')

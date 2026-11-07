/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/browser/brave_browser_main_parts_mac.h"

#import <Foundation/Foundation.h>

#include "base/apple/foundation_util.h"
#include "base/files/file_path.h"
#include "base/logging.h"
#include "base/path_service.h"
#include "base/strings/sys_string_conversions.h"
#include "brave/browser/mac/keystone_glue.h"
#include "brave/browser/sparkle_buildflags.h"
#include "chrome/common/chrome_constants.h"
#include "chrome/common/chrome_paths.h"

#if BUILDFLAG(ENABLE_SPARKLE)
#import "brave/browser/mac/sparkle_glue.h"
#endif

namespace {

// FlyWeb: before the first start of a new major on an existing profile, copy
// the whole user data dir to a sibling "<name> antes de <major> (<date>)"
// folder, because the older FlyWeb cannot go back to a profile the newer
// Chromium has opened: its databases are migrated to newer schemas. Going
// back is closing FlyWeb, reinstalling the older version and putting that copy
// back in place. Caches live in ~/Library/Caches and are not copied. Each
// major has its own marker file in the user data dir, written once the copy
// is done (or attempted), so a later start never copies an already migrated
// profile under the older name.
//   2.0 (Chromium 128): "FlyWeb 2 Profile" (FlyWeb 2.x wrote it).
//   3.0 (Chromium 156): "FlyWeb 3 Profile".
void BackUpProfileBeforeFirstStartOf(NSString* marker_file,
                                     NSString* major_version) {
  base::FilePath user_data_dir;
  if (!base::PathService::Get(chrome::DIR_USER_DATA, &user_data_dir)) {
    return;
  }
  NSFileManager* fm = NSFileManager.defaultManager;
  NSString* dir = base::apple::FilePathToNSString(user_data_dir);
  NSString* marker = [dir stringByAppendingPathComponent:marker_file];
  if ([fm fileExistsAtPath:marker]) {
    return;
  }
  // A new install has no Local State yet: nothing to protect.
  NSString* local_state =
      [dir stringByAppendingPathComponent:base::SysUTF8ToNSString(
                                              chrome::kLocalStateFilename)];
  if ([fm fileExistsAtPath:local_state]) {
    NSDateFormatter* formatter = [[NSDateFormatter alloc] init];
    formatter.locale = [NSLocale localeWithLocaleIdentifier:@"en_US_POSIX"];
    formatter.dateFormat = @"yyyy-MM-dd";
    NSString* name =
        [NSString stringWithFormat:@"%@ antes de %@ (%@)",
                                   dir.lastPathComponent, major_version,
                                   [formatter stringFromDate:[NSDate date]]];
    NSString* backup = [dir.stringByDeletingLastPathComponent
        stringByAppendingPathComponent:name];
    if (![fm fileExistsAtPath:backup]) {
      NSError* error = nil;
      // Copies symlinks as symlinks (SingletonLock and friends).
      if (![fm copyItemAtPath:dir toPath:backup error:&error]) {
        LOG(ERROR) << "FlyWeb: profile backup before "
                   << base::SysNSStringToUTF8(major_version) << " failed: "
                   << base::SysNSStringToUTF8(error.localizedDescription);
        [fm removeItemAtPath:backup error:nil];
        // Carry on: blocking the browser would be worse. The marker is still
        // written below.
      } else {
        // The previous instance's lock must not travel with the copy.
        for (NSString* lock in
             @[ @"SingletonLock", @"SingletonSocket", @"SingletonCookie" ]) {
          [fm removeItemAtPath:[backup stringByAppendingPathComponent:lock]
                         error:nil];
        }
      }
    }
  }
  [@"" writeToFile:marker
        atomically:YES
          encoding:NSUTF8StringEncoding
             error:nil];
}

}  // namespace

int BraveBrowserMainPartsMac::PreEarlyInitialization() {
  // Before any profile database is opened (and migrated). Local State has
  // already been read for the feature list, but its writes are deferred.
  // 3.0 only: a 1.x or 2.x profile gets the "antes de 3.0" copy, which is
  // what that older version can reopen.
  BackUpProfileBeforeFirstStartOf(@"FlyWeb 3 Profile", @"3.0");
  return ChromeBrowserMainPartsMac::PreEarlyInitialization();
}

void BraveBrowserMainPartsMac::PreCreateMainMessageLoop() {
  ChromeBrowserMainPartsMac::PreCreateMainMessageLoop();

#if BUILDFLAG(ENABLE_SPARKLE)
  // It would be no-op if udpate is disabled.
  [[SparkleGlue sharedSparkleGlue] registerWithSparkle];
#endif
}

void BraveBrowserMainPartsMac::PostProfileInit(Profile* profile,
                                               bool is_initial_profile) {
  ChromeBrowserMainPartsMac::PostProfileInit(profile, is_initial_profile);

  if (!is_initial_profile) {
    return;
  }

  // Activation of Keystone is not automatic but done in response to the
  // counting and reporting of profiles.
  KeystoneGlue* glue = [KeystoneGlue defaultKeystoneGlue];
  if (glue && ![glue isRegisteredAndActive]) {
    // If profile loading has failed, we still need to handle other tasks
    // like marking of the product as active.
    [glue setRegistrationActive];
  }
}

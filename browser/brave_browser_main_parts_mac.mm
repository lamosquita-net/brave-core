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

// FlyWeb: written in the user data dir once a profile has been opened by 2.x.
NSString* const kFlyWeb2MarkerFile = @"FlyWeb 2 Profile";

// FlyWeb 1.x (Chromium 116) cannot go back to a profile that 2.x (Chromium
// 128) has opened: its databases are migrated to newer schemas. Before the
// first 2.x start on an existing 1.x profile, copy the whole user data dir to
// a sibling "<name> antes de 2.0 (<date>)" folder, so going back to 1.15.2 is
// closing FlyWeb, reinstalling 1.15.2 and putting that copy back in place.
// Caches live in ~/Library/Caches and are not copied.
void BackUpProfileBeforeFirst2xStart() {
  base::FilePath user_data_dir;
  if (!base::PathService::Get(chrome::DIR_USER_DATA, &user_data_dir)) {
    return;
  }
  NSFileManager* fm = NSFileManager.defaultManager;
  NSString* dir = base::apple::FilePathToNSString(user_data_dir);
  NSString* marker = [dir stringByAppendingPathComponent:kFlyWeb2MarkerFile];
  if ([fm fileExistsAtPath:marker]) {
    return;
  }
  // A new install has no Local State yet: nothing to protect.
  NSString* local_state = [dir
      stringByAppendingPathComponent:base::SysUTF8ToNSString(
                                         chrome::kLocalStateFilename)];
  if ([fm fileExistsAtPath:local_state]) {
    NSDateFormatter* formatter = [[NSDateFormatter alloc] init];
    formatter.locale = [NSLocale localeWithLocaleIdentifier:@"en_US_POSIX"];
    formatter.dateFormat = @"yyyy-MM-dd";
    NSString* name = [NSString
        stringWithFormat:@"%@ antes de 2.0 (%@)", dir.lastPathComponent,
                         [formatter stringFromDate:[NSDate date]]];
    NSString* backup = [dir.stringByDeletingLastPathComponent
        stringByAppendingPathComponent:name];
    if (![fm fileExistsAtPath:backup]) {
      NSError* error = nil;
      // Copies symlinks as symlinks (SingletonLock and friends).
      if (![fm copyItemAtPath:dir toPath:backup error:&error]) {
        LOG(ERROR) << "FlyWeb: profile backup before 2.0 failed: "
                   << base::SysNSStringToUTF8(error.localizedDescription);
        [fm removeItemAtPath:backup error:nil];
        // Carry on: blocking the browser would be worse. The marker is still
        // written below, so a later start never copies a profile 2.x has
        // already migrated under the "antes de 2.0" name.
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
  BackUpProfileBeforeFirst2xStart();
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

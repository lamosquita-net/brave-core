/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/browser/brave_browser_main_parts_mac.h"

#import <Foundation/Foundation.h>
#include <errno.h>

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

// FlyWeb: copies a live user data dir. Files that vanish while the copy walks
// the folder (atomic-write temporaries such as variations'
// ".net.lamosquita.flyweb.VariationsSeedV2.XXXXXX", journals) are skipped
// instead of failing the whole copy; any other error still fails it.
@interface FlyWebProfileCopyDelegate : NSObject <NSFileManagerDelegate>
@end

@implementation FlyWebProfileCopyDelegate

- (BOOL)fileManager:(NSFileManager*)fileManager
    shouldCopyItemAtPath:(NSString*)srcPath
                  toPath:(NSString*)dstPath {
  NSString* name = srcPath.lastPathComponent;
  // ImportantFileWriter temporaries: ".<bundle id>.<file>.XXXXXX".
  return !([name hasPrefix:@".net.lamosquita."] ||
           [name hasPrefix:@".org.chromium."] ||
           [name hasPrefix:@".com.brave."]);
}

- (BOOL)fileManager:(NSFileManager*)fileManager
    shouldProceedAfterError:(NSError*)error
          copyingItemAtPath:(NSString*)srcPath
                     toPath:(NSString*)dstPath {
  NSError* underlying = error.userInfo[NSUnderlyingErrorKey];
  const bool vanished =
      ([error.domain isEqualToString:NSCocoaErrorDomain] &&
       (error.code == NSFileNoSuchFileError ||
        error.code == NSFileReadNoSuchFileError)) ||
      ([underlying.domain isEqualToString:NSPOSIXErrorDomain] &&
       underlying.code == ENOENT);
  if (vanished) {
    LOG(WARNING) << "FlyWeb: profile backup skipped a vanished file: "
                 << base::SysNSStringToUTF8(srcPath);
  }
  return vanished;
}

@end

namespace {

// FlyWeb: before the first start of a new major on an existing profile, copy
// the whole user data dir to a sibling "<name> antes de <major> (<date>)"
// folder, because the older FlyWeb cannot go back to a profile the newer
// Chromium has opened: its databases are migrated to newer schemas. Going
// back is closing FlyWeb, reinstalling the older version and putting that copy
// back in place. Caches live in ~/Library/Caches and are not copied.
// Each major has its own marker file in the user data dir, written only once
// the copy is done:
//   2.0 (Chromium 128): "FlyWeb 2 Profile" (FlyWeb 2.x wrote it).
//   3.0 (Chromium 156): "FlyWeb 3 Profile".
// If the copy fails twice, that marker is NOT written; "<marker> sin copia" is,
// so that a later start never copies the profile this start migrates under the
// "antes de" name (it would not open in the older version).
bool CopyProfile(NSString* dir, NSString* backup) {
  NSFileManager* fm = [[NSFileManager alloc] init];
  FlyWebProfileCopyDelegate* delegate =
      [[FlyWebProfileCopyDelegate alloc] init];
  fm.delegate = delegate;
  for (int attempt = 1; attempt <= 2; ++attempt) {
    NSError* error = nil;
    // Copies symlinks as symlinks (SingletonLock and friends).
    if ([fm copyItemAtPath:dir toPath:backup error:&error]) {
      // The previous instance's lock must not travel with the copy.
      for (NSString* lock in
           @[ @"SingletonLock", @"SingletonSocket", @"SingletonCookie" ]) {
        [fm removeItemAtPath:[backup stringByAppendingPathComponent:lock]
                       error:nil];
      }
      return true;
    }
    LOG(ERROR) << "FlyWeb: profile backup attempt " << attempt << " failed: "
               << base::SysNSStringToUTF8(error.localizedDescription);
    [fm removeItemAtPath:backup error:nil];
  }
  return false;
}

void BackUpProfileBeforeFirstStartOf(NSString* marker_file,
                                     NSString* major_version) {
  base::FilePath user_data_dir;
  if (!base::PathService::Get(chrome::DIR_USER_DATA, &user_data_dir)) {
    return;
  }
  NSFileManager* fm = NSFileManager.defaultManager;
  NSString* dir = base::apple::FilePathToNSString(user_data_dir);
  NSString* marker = [dir stringByAppendingPathComponent:marker_file];
  NSString* no_copy_marker =
      [dir stringByAppendingPathComponent:
               [marker_file stringByAppendingString:@" sin copia"]];
  if ([fm fileExistsAtPath:marker] || [fm fileExistsAtPath:no_copy_marker]) {
    return;
  }
  bool copied = true;
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
    // An existing folder of that name is a backup made earlier today.
    if (![fm fileExistsAtPath:backup]) {
      copied = CopyProfile(dir, backup);
    }
  }
  // Carry on either way: blocking the browser would be worse.
  [@"" writeToFile:(copied ? marker : no_copy_marker)
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

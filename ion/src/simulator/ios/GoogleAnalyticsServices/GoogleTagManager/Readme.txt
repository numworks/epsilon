Google Tag Manager iOS SDK

Copyright 2013-2015 Google, Inc. All rights reserved.

================================================================================
DESCRIPTION:

This SDK provides developers with the capability to use Google Tag Manager
to do server-side rule-based customization of configuration variables.

The SDK is packaged as a zip file with a directory (Examples) containing
examples, a directory (Library) containing the headers and the static library,
the change history (CHANGELOG) and this file (README).

Details on how to use this SDK are available at:
  http://developers.google.com/tag-manager/ios

================================================================================
BUILD REQUIREMENTS:

Mac OS X 10.6 or later.
XCode 5.0 or later.
iOS SDK 5.0 or later (6.0 or later for 64-bit).

================================================================================
RUNTIME REQUIREMENTS:

iOS 5.0 or later

Your application must link to the following frameworks:
  CoreData.framework
  Foundation.framework
  SystemConfiguration.framework
  UIKit.framework
  libsqlite3.dylib
  libz.dylib

If you wish to have access to the advertising identifier (IDFA) string and
advertiser tracking enabled flag, the following are also required:
  AdSupport.framework
  libAdIdAccess.a (included)

In order to ensure that the libAdIdAccess.a code doesn't get dead-stripped
from your executable during linking, you'll need to either add the -all_load
or -ObjC flag to the "Other Linker Flags", or, for finer-grained control, add the
-force_load flag (followed by a full pathname to libAdIdAccess.a).


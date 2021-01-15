Google Analytics iOS SDK version 3.17
Copyright 2009 - 2015 Google, Inc. All rights reserved.

================================================================================
DESCRIPTION:

This SDK provides developers with the capability to use Google Analytics
to track iOS application usage.

The SDK is packaged as a set of header files and a static library. Get started
by adding the header files from the Library subdirectory (GAI.h,
GAIDictionaryBuilder.h, etc.) and libGoogleAnalyticsServices.a to your XCode
project.  See below for a list of frameworks/libraries required by this SDK.

See the Examples/CuteAnimals application for an illustration of how to use
automatic screen tracking, event tracking, background dispatching, and uncaught
exception tracking.

You will need a Google Analytics tracking ID to track application usage with the
SDK. It is recommended to create an account for each set of applications that
are to be tracked together, and to use that account's tracking ID in each
application. To create a new tracking ID, go to your admin panel in Google
Analytics and select "New Account". Under "What would you like to track?",
choose "App" and complete the remainder of the form. When you are finished,
click "Get Tracking ID". The tracking ID will be of the form "UA-" followed by a
sequence of numbers and dashes.

You must indicate to your users, either in the app itself or in your terms of
service, that you reserve the right to anonymously track and report a user's
activity inside of your app.

If you wish to enable any Google Analytics features that require idfa
collection, you'll need to link in the additional library libAdIdAccess.a
(provided in this SDK) and add AdSupport.framework to your application.  In
addition, the linker flag:

   -force_load /path/to/libAdIdAccess.a

may be required, depending on your build settings. Finally, you need to set the
property allowIDFACollection to YES on each tracker that will collect idfa.

If you wish to enable iAd install attribution, you'll need to add iAd.framework
to your application.

Implementation Details:

Tracking information is stored in an SQLite database and dispatched to the
Google Analytics servers in a manner set by the developer: periodically at an
interval determined by the developer, immediately when tracking calls are made,
or manually. A battery efficient strategy may be to initiate a dispatch when the
application needs to access the network. Tracking information is dispatched
using HTTP or HTTPS requests to a Google Analytics server.

================================================================================
BUILD REQUIREMENTS:

Mac OS X 10.6 or later.
XCode 5.0 or later
iOS SDK 5.0 or later (6.0 or later for 64-bit).

================================================================================
RUNTIME REQUIREMENTS:

iOS 5.0 or later.

Your app must link the following frameworks:
  CoreData.framework
  SystemConfiguration.framework
  libz.dylib
  libsqlite3.dylib

================================================================================
PACKAGING LIST:

Library/ (contains header and library files to compile and link with)
  GAI.h
  GAIDictionaryBuilder.h
  GAIEcommerceFields.h
  GAIEcommerceProduct.h
  GAIEcommerceProductAction.h
  GAIEcommercePromotion.h
  GAIFields.h
  GAILogger.h
  GAITrackedViewController.h
  GAITracker.h
Examples/ (contains an example tracked application)
Documentation/ (contains documentation)

================================================================================

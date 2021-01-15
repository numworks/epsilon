//
//  AppDelegate.m
//  CuteAnimals
//
//  Copyright 2012 Google, Inc. All rights reserved.
//

#import "AppDelegate.h"
#import "NavController.h"
#import "RootViewController.h"

/******* Set your tracking ID here *******/
static NSString *const kTrackingId = @"UA-XXXXX-Y";
static NSString *const kAllowTracking = @"allowTracking";

@interface AppDelegate ()

// Used for sending Google Analytics traffic in the background.
@property(nonatomic, assign) BOOL okToWait;
@property(nonatomic, copy) void (^dispatchHandler)(GAIDispatchResult result);

- (NSDictionary *)loadImages;

@end

@implementation AppDelegate
- (void)applicationDidBecomeActive:(UIApplication *)application {
  [GAI sharedInstance].optOut =
      ![[NSUserDefaults standardUserDefaults] boolForKey:kAllowTracking];
}

- (BOOL)application:(UIApplication *)application
    didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
  self.images = [self loadImages];
  NSDictionary *appDefaults = @{kAllowTracking: @(YES)};
  [[NSUserDefaults standardUserDefaults] registerDefaults:appDefaults];
  // User must be able to opt out of tracking
  [GAI sharedInstance].optOut =
      ![[NSUserDefaults standardUserDefaults] boolForKey:kAllowTracking];

  // If your app runs for long periods of time in the foreground, you might consider turning
  // on periodic dispatching.  This app doesn't, so it'll dispatch all traffic when it goes
  // into the background instead.  If you wish to dispatch periodically, we recommend a 120
  // second dispatch interval.
  // [GAI sharedInstance].dispatchInterval = 120;
  [GAI sharedInstance].dispatchInterval = -1;

  [GAI sharedInstance].trackUncaughtExceptions = YES;
  self.tracker = [[GAI sharedInstance] trackerWithName:@"CuteAnimals"
                                            trackingId:kTrackingId];

  self.window =
      [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
  // Override point for customization after application launch.
  self.viewController =
      [[RootViewController alloc] initWithNibName:@"RootViewController"
                                            bundle:nil];

  self.navController =
      [[NavController alloc] initWithRootViewController:self.viewController];
  self.navController.delegate = self.navController;

  self.window.rootViewController = self.navController;
  [self.window makeKeyAndVisible];

  return YES;
}

// In case the app was sent into the background when there was no network connection, we will use
// the background data fetching mechanism to send any pending Google Analytics data.  Note that
// this app has turned on background data fetching in the capabilities section of the project.
-(void)application:(UIApplication *)application
    performFetchWithCompletionHandler:(void (^)(UIBackgroundFetchResult))completionHandler
{
  [self sendHitsInBackground];
  completionHandler(UIBackgroundFetchResultNewData);
}

// We'll try to dispatch any hits queued for dispatch as the app goes into the background.
- (void)applicationDidEnterBackground:(UIApplication *)application
{
  [self sendHitsInBackground];
}

// This method sends hits in the background until either we're told to stop background processing,
// we run into an error, or we run out of hits.  We use this to send any pending Google Analytics
// data since the app won't get a chance once it's in the background.
- (void)sendHitsInBackground {
  self.okToWait = YES;
  __weak AppDelegate *weakSelf = self;
  __block UIBackgroundTaskIdentifier backgroundTaskId =
      [[UIApplication sharedApplication] beginBackgroundTaskWithExpirationHandler:^{
      weakSelf.okToWait = NO;
  }];

  if (backgroundTaskId == UIBackgroundTaskInvalid) {
    return;
  }

  self.dispatchHandler = ^(GAIDispatchResult result) {
      // If the last dispatch succeeded, and we're still OK to stay in the background then kick off
      // again.
      if (result == kGAIDispatchGood && weakSelf.okToWait ) {
        [[GAI sharedInstance] dispatchWithCompletionHandler:weakSelf.dispatchHandler];
      } else {
        [[UIApplication sharedApplication] endBackgroundTask:backgroundTaskId];
      }
  };
  [[GAI sharedInstance] dispatchWithCompletionHandler:self.dispatchHandler];
}

- (NSDictionary *)loadImages {
  NSArray *contents = [[NSBundle mainBundle] pathsForResourcesOfType:@"jpg"
                                                         inDirectory:nil];
  if (!contents) {
    NSLog(@"Failed to load directory contents");
    return nil;
  }
  NSMutableDictionary *images = [NSMutableDictionary dictionary];
  for (NSString *file in contents) {
    NSArray *components = [[file lastPathComponent]
                           componentsSeparatedByString:@"-"];
    if (components.count == 0) {
      NSLog(@"Filename doesn't contain dash: %@", file);
      continue;
    }
    UIImage *image = [UIImage imageWithContentsOfFile:file];
    if (!image) {
      NSLog(@"Failed to load file: %@", file);
      continue;
    }
    NSString *prefix = components[0];
    NSMutableArray *categoryImages = images[prefix];
    if (!categoryImages) {
      categoryImages = [NSMutableArray array];
      images[prefix] = categoryImages;
    }
    [categoryImages addObject:image];
  }
  for (NSString *cat in [images allKeys]) {
    NSArray *array = images[cat];
    NSLog(@"Category %@: %u image(s).", cat, array.count);
  }
  return images;
}

@end

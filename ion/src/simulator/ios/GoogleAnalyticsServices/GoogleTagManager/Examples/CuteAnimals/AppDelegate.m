//
//  AppDelegate.m
//  CuteAnimals
//
//  Copyright 2013 Google, Inc. All rights reserved.
//

#if !defined(__has_feature) || !__has_feature(objc_arc)
#error "This file requires ARC support."
#endif

#import "AppDelegate.h"
#import "CustomMacroHandler.h"
#import "CustomTagHandler.h"
#import "NavController.h"
#import "RootViewController.h"
#import "TAGContainer.h"
#import "TAGContainerOpener.h"
#import "TAGLogger.h"
#import "TAGManager.h"

@interface AppDelegate ()<TAGContainerOpenerNotifier>

// Used for sending traffic in the background.
@property(nonatomic, assign) BOOL okToWait;
@property(nonatomic, copy) void (^dispatchHandler)(TAGDispatchResult result);

- (void)setupRootViewController;
- (NSDictionary *)loadImages;

@end

@implementation AppDelegate

@synthesize window = _window;
@synthesize navController = _navController;
@synthesize viewController = _viewController;
@synthesize images = _images;
@synthesize tagManager = _tagManager;
@synthesize container = _container;

- (BOOL)application:(UIApplication *)application
    didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
  self.tagManager = [TAGManager instance];

  // Modify the log level of the logger to print out not only
  // warning and error messages, but also verbose, debug, info messages.
  [self.tagManager.logger setLogLevel:kTAGLoggerLogLevelVerbose];

  // Following provides ability to support preview from Tag Manager.
  // You need to make these calls before opening a container to make
  // preview works.
  NSURL *url = [launchOptions valueForKey:UIApplicationLaunchOptionsURLKey];
  if (url != nil) {
    [self.tagManager previewWithUrl:url];
  }

  // Open a container.
  [TAGContainerOpener openContainerWithId:@"GTM-XXXX"
                               tagManager:self.tagManager
                                 openType:kTAGOpenTypePreferNonDefault
                                  timeout:nil
                                 notifier:self];

  self.images = [self loadImages];

  return YES;
}

- (void)containerAvailable:(TAGContainer *)container {
  // Important note: containerAvailable may be called from a different thread, marshall the
  // notification back to the main thread to avoid a race condition with viewDidAppear.
  dispatch_async(dispatch_get_main_queue(), ^{
    self.container = container;
    // Register two custom function call macros to the container.
    [self.container registerFunctionCallMacroHandler:[[CustomMacroHandler alloc] init]
                                            forMacro:@"increment"];
    [self.container registerFunctionCallMacroHandler:[[CustomMacroHandler alloc] init]
                                            forMacro:@"mod"];
    // Register a custom function call tag to the container.
    [self.container registerFunctionCallTagHandler:[[CustomTagHandler alloc] init]
                                            forTag:@"custom_tag"];
    [self setupRootViewController];
  });
}

- (void)setupRootViewController {
  self.window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
  self.viewController = [[RootViewController alloc] initWithNibName:@"RootViewController"
                                                             bundle:nil];

  self.navController = [[NavController alloc] initWithRootViewController:self.viewController];
  self.navController.delegate = self.navController;

  self.viewController.navController = self.navController;
  self.window.rootViewController = self.navController;
  [self.window makeKeyAndVisible];
}

- (BOOL)application:(UIApplication *)application
            openURL:(NSURL *)url
  sourceApplication:(NSString *)sourceApplication
         annotation:(id)annotation {
  if ([self.tagManager previewWithUrl:url]) {
    return YES;
  }

  // Code to handle other urls.

  return NO;
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
// we run into an error, or we run out of hits.
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

  self.dispatchHandler = ^(TAGDispatchResult result) {
      // If the last dispatch succeeded, and we're still OK to stay in the background then kick off
      // again.
      if (result == kTAGDispatchGood && weakSelf.okToWait ) {
        [[TAGManager instance] dispatchWithCompletionHandler:weakSelf.dispatchHandler];
      } else {
        [[UIApplication sharedApplication] endBackgroundTask:backgroundTaskId];
      }
  };
  [[TAGManager instance] dispatchWithCompletionHandler:self.dispatchHandler];
}

- (NSDictionary *)loadImages {
  NSArray *contents = [[NSBundle mainBundle] pathsForResourcesOfType:@"jpg"
                                                         inDirectory:nil];
  if (!contents) {
    NSLog(@"Failed to load directory contents");
    return nil;
  }
  NSMutableDictionary *images = [NSMutableDictionary dictionaryWithCapacity:0];
  for (NSString *file in contents) {
    NSArray *components = [[file lastPathComponent] componentsSeparatedByString:@"-"];
    if (components.count == 0) {
      NSLog(@"Filename doesn't contain dash: %@", file);
      continue;
    }
    UIImage *image = [UIImage imageWithContentsOfFile:file];
    if (!image) {
      NSLog(@"Failed to load file: %@", file);
      continue;
    }
    NSString *prefix = [components objectAtIndex:0];
    NSMutableArray *categoryImages = [images objectForKey:prefix];
    if (!categoryImages) {
      categoryImages = [NSMutableArray arrayWithCapacity:0];
      [images setObject:categoryImages
                 forKey:prefix];
    }
    [categoryImages addObject:image];
  }
  for (NSString *cat in [images allKeys]) {
    NSArray *array = [images objectForKey:cat];
    NSLog(@"Category %@: %u image(s).", cat, array.count);
  }
  return images;
}

@end

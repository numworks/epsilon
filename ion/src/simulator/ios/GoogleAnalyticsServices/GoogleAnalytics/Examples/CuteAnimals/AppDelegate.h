//
//  AppDelegate.h
//  CuteAnimals
//
//  Copyright 2012 Google, Inc. All rights reserved.
//

#import <UIKit/UIKit.h>

#import "GAI.h"

@class RootViewController;
@class NavController;

@interface AppDelegate : UIResponder <UIApplicationDelegate>

@property(nonatomic, strong) UIWindow *window;
@property(nonatomic, strong) NavController *navController;
@property(nonatomic, strong) RootViewController *viewController;
@property(nonatomic, strong) id<GAITracker> tracker;
@property(nonatomic, strong) NSDictionary *images;

@end

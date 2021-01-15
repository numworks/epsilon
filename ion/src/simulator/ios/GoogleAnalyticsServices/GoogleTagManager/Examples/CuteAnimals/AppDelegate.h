//
//  AppDelegate.h
//  CuteAnimals
//
//  Copyright 2013 Google, Inc. All rights reserved.
//

#import <UIKit/UIKit.h>

@class RootViewController;
@class NavController;
@class TAGManager;
@class TAGContainer;

@interface AppDelegate : UIResponder <UIApplicationDelegate>

@property(nonatomic, retain) UIWindow *window;
@property(nonatomic, retain) NavController *navController;
@property(nonatomic, retain) RootViewController *viewController;
@property(nonatomic, retain) NSDictionary *images;
@property(nonatomic, retain) TAGManager *tagManager;
@property(nonatomic, retain) TAGContainer *container;

@end

//
//  Utils.m
//  CuteAnimals
//
//  Copyright 2013 Google, Inc. All rights reserved.
//

#if !defined(__has_feature) || !__has_feature(objc_arc)
#error "This file requires ARC support."
#endif

#import "AppDelegate.h"
#import "TAGDataLayer.h"
#import "TAGManager.h"
#import "Utils.h"

@implementation Utils

+ (void)pushOpenScreenEventWithScreenName:(NSString *)screenName {
  AppDelegate *delegate = [UIApplication sharedApplication].delegate;
  [delegate.tagManager.dataLayer push:@{@"event": @"openScreen",
                                        @"screenName": screenName}];
}

+ (void)pushCloseScreenEventWithScreenName:(NSString *)screenName {
  AppDelegate *delegate = [UIApplication sharedApplication].delegate;
  [delegate.tagManager.dataLayer push:@{@"event": @"closeScreen",
                                        @"screenName": screenName}];
}

@end

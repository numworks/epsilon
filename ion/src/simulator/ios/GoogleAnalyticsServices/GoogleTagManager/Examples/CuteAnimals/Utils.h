//
//  Utils.h
//  CuteAnimals
//
//  Copyright 2013 Google, Inc. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface Utils : NSObject

/**
 * Push an "openScreen" event with the given screen name. Tags that
 * match that event will fire.
 */
+ (void)pushOpenScreenEventWithScreenName:(NSString *)screenName;

/**
 * Push a "closeScreen" event with the given screen name. Tags that
 * match that event will fire.
 */
+ (void)pushCloseScreenEventWithScreenName:(NSString *)screenName;

@end

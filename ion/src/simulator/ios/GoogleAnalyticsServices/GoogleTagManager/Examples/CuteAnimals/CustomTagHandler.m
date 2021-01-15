//
//  CustomTagHandler.m
//  CuteAnimals
//
//  Copyright 2013 Google, Inc. All rights reserved.
//

#if !defined(__has_feature) || !__has_feature(objc_arc)
#error "This file requires ARC support."
#endif

#import "CustomTagHandler.h"

@implementation CustomTagHandler

- (void)execute:(NSString *)tagName parameters:(NSDictionary *)parameters {
  NSLog(@"Custom function call tag :%@ is fired", tagName);
  // Other code firing this custom tag.
}

@end

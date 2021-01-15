//
//  CustomMacroHandler.m
//  CuteAnimals
//
//  Copyright 2013 Google, Inc. All rights reserved.
//

#if !defined(__has_feature) || !__has_feature(objc_arc)
#error "This file requires ARC support."
#endif

#import "CustomMacroHandler.h"

@implementation CustomMacroHandler

- (id)valueForMacro:(NSString *)macroName
         parameters:(NSDictionary *)parameters {
  if ([macroName isEqual:@"increment"]) {
    self.numOfCalls++;
    return [NSString stringWithFormat:@"%d", self.numOfCalls];
  } else if ([macroName isEqual:@"mod"]) {
    NSString *value1 = parameters[@"key1"];
    NSString *value2 = parameters[@"key2"];
    return [NSNumber numberWithInt:([value1 intValue] % [value2 intValue])];
  } else {
    NSString *message =
        [NSString stringWithFormat:@"Custom macro name: %@ is not supported", macroName];
    @throw [NSException exceptionWithName:@"IllegalArgumentException"
                                   reason:message
                                 userInfo:nil];
  }
}

@end

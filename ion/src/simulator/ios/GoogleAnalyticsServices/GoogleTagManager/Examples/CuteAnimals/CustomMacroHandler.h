//
//  CustomMacroHandler.h
//  CuteAnimals
//
//  Copyright 2013 Google, Inc. All rights reserved.
//

#import "TAGContainer.h"

@interface CustomMacroHandler : NSObject <TAGFunctionCallMacroHandler>

@property(nonatomic) NSUInteger numOfCalls;

@end

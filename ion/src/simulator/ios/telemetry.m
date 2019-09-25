#include "../shared/platform.h"

#import <GAI.h>
#import <GAIDictionaryBuilder.h>
#import <GAIFields.h>

void IonSimulatorTelemetryInit() {
  [[GAI sharedInstance] trackerWithTrackingId:@"UA-93775823-3"];
}

void IonSimulatorTelemetryEvent(const char * eventName) {
  id<GAITracker> tracker = [GAI sharedInstance].defaultTracker;
  [tracker set:kGAIScreenName value:[NSString stringWithUTF8String:eventName]];
  [tracker send:[[GAIDictionaryBuilder createScreenView] build]];
}

void IonSimulatorTelemetryDeinit() {
}

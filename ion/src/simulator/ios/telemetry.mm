#include "../shared/telemetry.h"

#import <GAI.h>
#import <GAIDictionaryBuilder.h>
#import <GAIFields.h>

namespace Ion {
namespace Simulator {
namespace Telemetry {

void init() {
  [[GAI sharedInstance] trackerWithTrackingId:@"UA-93775823-3"];
}

void shutdown() {
}

}
}
}

static inline NSString * NS(const char * s) {
  if (s != nullptr) {
    return [NSString stringWithUTF8String:s];
  } else {
    return nil;
  }
}

namespace Ion {
namespace Telemetry {

void reportScreen(const char * screenName) {
  id<GAITracker> tracker = [GAI sharedInstance].defaultTracker;
  [tracker set:kGAIScreenName value:NS(screenName)];
  [tracker send:[[GAIDictionaryBuilder createScreenView] build]];
}

void reportEvent(const char * category, const char * action, const char * label) {
  id<GAITracker> tracker = [GAI sharedInstance].defaultTracker;
  [tracker send:[[GAIDictionaryBuilder
    createEventWithCategory:NS(category)
    action:NS(action)
    label:NS(label)
    value:nil] build]];
}

}
}

#ifndef ION_SIMULATOR_EVENTS_H
#define ION_SIMULATOR_EVENTS_H

#include <ion/events.h>

namespace Ion {
namespace Simulator {
namespace Events {

void dumpEventCount(int i);
void logAfter(int numberOfEvents);

}
}

namespace Events {

char * sharedExternalTextBuffer();
constexpr Event ExternalText = Event::Special(6);

}
}

#endif

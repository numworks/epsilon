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

static constexpr int sharedExternalTextBufferSize = 2;
char * sharedExternalTextBuffer();

}
}

#endif

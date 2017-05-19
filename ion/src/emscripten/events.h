#ifndef ION_EMSCRIPTEN_EVENTS_H
#define ION_EMSCRIPTEN_EVENTS_H

#include <ion/events.h>

extern "C" {
void IonEventsEmscriptenPushEvent(int e);
}

namespace Ion {
namespace Events {
namespace Emscripten {

void init();

}
}
}

#endif

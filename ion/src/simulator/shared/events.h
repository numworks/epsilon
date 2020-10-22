#ifndef ION_SIMULATOR_EVENTS_H
#define ION_SIMULATOR_EVENTS_H

#include <ion/events.h>
#include <SDL.h>

namespace Ion {
namespace Simulator {
namespace Events {

void dumpEventCount(int i);
void logAfter(int numberOfEvents);

}
}

namespace Events {

static constexpr size_t sharedExternalTextBufferSize = sizeof(SDL_TextInputEvent::text);
char * sharedExternalTextBuffer();
constexpr Event ExternalText = Event::Special(6);

}
}

#endif

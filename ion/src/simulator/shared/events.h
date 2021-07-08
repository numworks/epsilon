#ifndef ION_SIMULATOR_EVENTS_H
#define ION_SIMULATOR_EVENTS_H

#include <ion/events.h>
#include <SDL.h>

namespace Ion {
namespace Events {

static constexpr size_t sharedExternalTextBufferSize = sizeof(SDL_TextInputEvent::text);
char * sharedExternalTextBuffer();

}
}

#endif

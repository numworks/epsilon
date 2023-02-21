#ifndef ION_SIMULATOR_EVENTS_H
#define ION_SIMULATOR_EVENTS_H

#include <SDL.h>
#include <ion/events.h>

namespace Ion {
namespace Events {

constexpr static size_t sharedExternalTextBufferSize =
    sizeof(SDL_TextInputEvent::text);
char* sharedExternalTextBuffer();

}  // namespace Events
}  // namespace Ion

#endif

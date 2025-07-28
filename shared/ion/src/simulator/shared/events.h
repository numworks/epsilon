#ifndef ION_SIMULATOR_EVENTS_H
#define ION_SIMULATOR_EVENTS_H

#include <SDL.h>
#include <ion/events.h>
#include <ion/keyboard/event_data.h>

namespace Ion {
namespace Events {

constexpr static size_t sharedExternalTextBufferSize = EventData::k_maxDataSize;
char* sharedExternalTextBuffer();

}  // namespace Events
}  // namespace Ion

#endif

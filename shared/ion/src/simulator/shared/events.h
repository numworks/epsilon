#pragma once

#include <SDL.h>
#include <ion/events.h>
#include <ion/keyboard/event_data.h>

namespace Ion {
namespace Events {

/* Buffer chosen to be large enough to hold a python script that would
 * fill the storage. */
constexpr static size_t sharedExternalTextBufferSize = 65536;
char* sharedExternalTextBuffer();

}  // namespace Events
}  // namespace Ion

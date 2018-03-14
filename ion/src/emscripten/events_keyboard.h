#ifndef ION_EMSCRIPTEN_EVENTS_KEYBOARD_H
#define ION_EMSCRIPTEN_EVENTS_KEYBOARD_H

#include <ion/events.h>

extern "C" {
void IonEventsEmscriptenPushKey(int keyNumber);
void IonEventsEmscriptenPushEvent(int eventNumber);
}

namespace Ion {
namespace Events {
namespace Emscripten {

void init();

}
}
}

#endif

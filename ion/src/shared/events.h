#ifndef ION_SHARED_EVENTS_H
#define ION_SHARED_EVENTS_H

#include <ion/events.h>
#include <ion/keyboard.h>

namespace Ion {
namespace Events {

Event sharedGetEvent(int * timeout);

/* Platform specific functions */
bool handlePreemption(bool stalling);
Event getPlatformEvent();
void didPressNewKey(Keyboard::Key key);
Keyboard::State popKeyboardState();
bool waitForInterruptingEvent(int maximumDelay, int * timeout);

}
}

#endif

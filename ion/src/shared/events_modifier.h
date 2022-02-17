#ifndef ION_SHARED_EVENTS_MODIFIER_H
#define ION_SHARED_EVENTS_MODIFIER_H

#include <ion/events.h>

namespace Ion {
namespace Events {

void removeShift();
bool isShiftActive();
bool isAlphaActive();
bool isLockActive();
void updateModifiersFromEvent(Event e, Keyboard::State state);
int longPressFactor();
void resetLongPress();
void resetRepetition();
void incrementLongPress();
void incrementRepetition();

}
}

#endif

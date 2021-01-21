#ifndef ION_SHARED_EVENTS_MODIFIER_H
#define ION_SHARED_EVENTS_MODIFIER_H

#include <ion/events.h>

namespace Ion {
namespace Events {

void removeShift();
bool isShiftActive();
bool isAlphaActive();
bool isLockActive();
void resetLongRepetition();
void incrementRepetitionFactor();
void setLongRepetition(int longRepetition);
void updateModifiersFromEvent(Event e);

}
}

#endif

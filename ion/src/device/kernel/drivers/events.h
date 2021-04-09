#ifndef ION_DEVICE_KERNEL_EVENTS_H
#define ION_DEVICE_KERNEL_EVENTS_H

#include <ion/events.h>

namespace Ion {
namespace Device {
namespace Events {

void init();
void shutdown();
void initInterruptions();
void shutdownInterruptions();

Ion::Events::Event getEvent(int * timeout);
Ion::Events::Event getPlatformEvent();

/* The functions copyTextSecure and isDefinedSecure could have been methods of
 * the class 'Event' but, since they're used as SVCallHandlers, it would be
 * unsafe to rely on the fact that these methods are'n't in the vtable and thus
 * don't jump to an address specified at run time. That would make the code
 * quite vulnerable: by forging an event with a custom vtable, a attacker could
 * execute some code with kernel privilege. */
size_t copyTextSecure(uint8_t eventId, char * buffer, size_t bufferSize);
bool isDefinedSecure(uint8_t eventId);

void stall();

bool setPendingKeyboardStateIfPreemtive(Ion::Keyboard::State keyboardState);

}
}
}

#endif

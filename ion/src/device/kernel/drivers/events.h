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
void stall();

bool setPendingKeyboardStateIfPreemtive(Ion::Keyboard::State keyboardState);

}
}
}

#endif

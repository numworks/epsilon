#ifndef ION_DEVICE_KERNEL_EVENTS_H
#define ION_DEVICE_KERNEL_EVENTS_H

#include <ion/events.h>

namespace Ion {
namespace Device {
namespace Events {

Ion::Events::Event getEvent(int * timeout);
Ion::Events::Event getPlatformEvent();
uint64_t lastEventTime();

}
}
}

#endif

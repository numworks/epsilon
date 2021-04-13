#ifndef ION_DEVICE_SHARED_BACKLIGHT_H
#define ION_DEVICE_SHARED_BACKLIGHT_H

#include <stdint.h>

namespace Ion {
namespace Device {
namespace Backlight {

void initGPIO();
void shutdown();
void sendPulses(int n);

}
}
}

#endif

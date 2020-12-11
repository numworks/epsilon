#ifndef ION_DEVICE_SHARED_BACKLIGHT_H
#define ION_DEVICE_SHARED_BACKLIGHT_H

#include <stdint.h>

namespace Ion {
namespace Device {
namespace Backlight {

void init();
bool isInitialized();
void shutdown();
void suspend();
void resume();

uint8_t level();
void setLevel(uint8_t level);

void sendPulses(int n);

}
}
}

#endif

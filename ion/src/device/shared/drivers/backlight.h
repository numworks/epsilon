#ifndef ION_DEVICE_SHARED_BACKLIGHT_H
#define ION_DEVICE_SHARED_BACKLIGHT_H

#include <stdint.h>

namespace Ion {
namespace Device {
namespace Backlight {

void init();
void initGPIO();
void shutdown();
void sendPulses(int n);

void setBrightness(uint8_t b);
uint8_t brightness();

bool isInitialized();
uint8_t level();
void setLevel(uint8_t level);

}
}
}

#endif

#ifndef ION_DEVICE_KERNEL_BACKLIGHT_H
#define ION_DEVICE_KERNEL_BACKLIGHT_H

#include <shared/drivers/backlight.h>

namespace Ion {
namespace Device {
namespace Backlight {

void setBrightness(uint8_t b);
uint8_t brightness();

void init();
bool isInitialized();
uint8_t level();
void setLevel(uint8_t level);

}
}
}

#endif

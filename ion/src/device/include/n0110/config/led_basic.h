#ifndef ION_DEVICE_BOOTLOADER_DRIVERS_CONFIG_N0110_H
#define ION_DEVICE_BOOTLOADER_DRIVERS_CONFIG_N0110_H

#include <regs/regs.h>

namespace Ion {
namespace Device {
namespace LED {
namespace Config {

using namespace Regs;

constexpr static GPIOPin RGBPins[] = {
  GPIOPin(GPIOB, 4), // Red
  GPIOPin(GPIOB, 5), // Green
  GPIOPin(GPIOB, 0) // Blue
};

}
}
}
}

#endif

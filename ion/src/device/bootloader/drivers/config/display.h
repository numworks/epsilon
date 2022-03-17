#ifndef ION_DEVICE_N0110_CONFIG_DISPLAY_H
#define ION_DEVICE_N0110_CONFIG_DISPLAY_H

#include <regs/regs.h>

namespace Ion {
namespace Device {
namespace Display {
namespace Config {

using namespace Regs;

constexpr static GPIOPin FSMCPins[] = {
  GPIOPin(GPIOD, 0), GPIOPin(GPIOD, 1), GPIOPin(GPIOD, 4), GPIOPin(GPIOD, 5),
  GPIOPin(GPIOD, 7), GPIOPin(GPIOD, 8), GPIOPin(GPIOD, 9), GPIOPin(GPIOD, 10),
  GPIOPin(GPIOD, 11), GPIOPin(GPIOD, 14), GPIOPin(GPIOD, 15), GPIOPin(GPIOE, 7),
  GPIOPin(GPIOE, 8), GPIOPin(GPIOE, 9), GPIOPin(GPIOE, 10), GPIOPin(GPIOE, 11),
  GPIOPin(GPIOE, 12), GPIOPin(GPIOE, 13), GPIOPin(GPIOE, 14), GPIOPin(GPIOE, 15),
};

constexpr static GPIOPin PowerPin = GPIOPin(GPIOC, 8);
constexpr static GPIOPin ResetPin = GPIOPin(GPIOE, 1);
constexpr static GPIOPin ExtendedCommandPin = GPIOPin(GPIOD, 6);
constexpr static GPIOPin TearingEffectPin = GPIOPin(GPIOB, 11);

constexpr static DMA DMAEngine = DMA2;
constexpr static int DMAStream = 0;

constexpr static int HCLKFrequencyInMHz = 192;

constexpr static bool DisplayInversion = true;

}
}
}
}

#endif

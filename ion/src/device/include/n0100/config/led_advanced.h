#ifndef ION_DEVICE_N0100_KERNEL_DRIVERS_CONFIG_LED_H
#define ION_DEVICE_N0100_KERNEL_DRIVERS_CONFIG_LED_H

#include <regs/regs.h>

namespace Ion {
namespace Device {
namespace LED {
namespace Config {

using namespace Regs;

constexpr static int RedChannel = 2;
constexpr static int GreenChannel = 4;
constexpr static int BlueChannel = 3;

constexpr static AFGPIOPin RGBPins[] = {
  AFGPIOPin(GPIOC, 7,  GPIO::AFR::AlternateFunction::AF2, GPIO::PUPDR::Pull::None, GPIO::OSPEEDR::OutputSpeed::Low), // RED
  AFGPIOPin(GPIOB, 1,  GPIO::AFR::AlternateFunction::AF2, GPIO::PUPDR::Pull::None, GPIO::OSPEEDR::OutputSpeed::Low), // GREEN
  AFGPIOPin(GPIOB, 0,  GPIO::AFR::AlternateFunction::AF2, GPIO::PUPDR::Pull::None, GPIO::OSPEEDR::OutputSpeed::Low) // BLUE
};

}
}
}
}

#endif


#ifndef ION_DEVICE_N0100_CONFIG_LED_H
#define ION_DEVICE_N0100_CONFIG_LED_H

#include <regs/regs.h>

namespace Ion {
namespace Device {
namespace LED {
namespace Config {

using namespace Regs;

constexpr static AFGPIOPin RGBPins[] = {
  AFGPIOPin(GPIOB, 0,  GPIO::AFR::AlternateFunction::AF2, GPIO::PUPDR::Pull::None, GPIO::OSPEEDR::OutputSpeed::High),
  AFGPIOPin(GPIOB, 1,  GPIO::AFR::AlternateFunction::AF2, GPIO::PUPDR::Pull::None, GPIO::OSPEEDR::OutputSpeed::High),
  AFGPIOPin(GPIOC, 7,  GPIO::AFR::AlternateFunction::AF2, GPIO::PUPDR::Pull::None, GPIO::OSPEEDR::OutputSpeed::High),
};

}
}
}
}

#endif

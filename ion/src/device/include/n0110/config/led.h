#ifndef ION_DEVICE_INCLUDE_N0110_CONFIG_LED_H
#define ION_DEVICE_INCLUDE_N0110_CONFIG_LED_H

#include <regs/regs.h>

namespace Ion {
namespace Device {
namespace LED {
namespace Config {

using namespace Regs;

#warning Do same computation than for N0120?
constexpr uint16_t PWMPeriod = 40000;

static constexpr int RedChannel = 1;
static constexpr int GreenChannel = 2;
static constexpr int BlueChannel = 3;

constexpr static AFGPIOPin RGBPins[] = {
  AFGPIOPin(GPIOB, 4,  GPIO::AFR::AlternateFunction::AF2, GPIO::PUPDR::Pull::None, GPIO::OSPEEDR::OutputSpeed::Low), // RED
  AFGPIOPin(GPIOB, 5,  GPIO::AFR::AlternateFunction::AF2, GPIO::PUPDR::Pull::None, GPIO::OSPEEDR::OutputSpeed::Low), // GREEN
  AFGPIOPin(GPIOB, 0,  GPIO::AFR::AlternateFunction::AF2, GPIO::PUPDR::Pull::None, GPIO::OSPEEDR::OutputSpeed::Low) // BLUE
};

}
}
}
}

#endif

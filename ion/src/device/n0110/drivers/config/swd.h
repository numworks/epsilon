#ifndef ION_DEVICE_N0110_CONFIG_SWD_H
#define ION_DEVICE_N0110_CONFIG_SWD_H

#include <regs/regs.h>

namespace Ion {
namespace Device {
namespace SWD {
namespace Config {

using namespace Regs;

constexpr static AFGPIOPin Pins[] = {
  AFGPIOPin(GPIOA, 13, GPIO::AFR::AlternateFunction::AF0, GPIO::PUPDR::Pull::None, GPIO::OSPEEDR::OutputSpeed::High),
  AFGPIOPin(GPIOA, 14, GPIO::AFR::AlternateFunction::AF0, GPIO::PUPDR::Pull::None, GPIO::OSPEEDR::OutputSpeed::High),
  AFGPIOPin(GPIOB, 3,  GPIO::AFR::AlternateFunction::AF0, GPIO::PUPDR::Pull::None, GPIO::OSPEEDR::OutputSpeed::High),
};

}
}
}
}

#endif

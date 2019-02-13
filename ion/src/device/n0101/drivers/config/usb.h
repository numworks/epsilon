#ifndef ION_DEVICE_N0101_CONFIG_USB_H
#define ION_DEVICE_N0101_CONFIG_USB_H

#include <regs/regs.h>

namespace Ion {
namespace Device {
namespace USB {
namespace Config {

using namespace Regs;

constexpr static AFGPIOPin VbusPin = AFGPIOPin(GPIOA, 9, GPIO::AFR::AlternateFunction::AF10, GPIO::PUPDR::Pull::None, GPIO::OSPEEDR::OutputSpeed::High);
constexpr static AFGPIOPin DmPin = AFGPIOPin(GPIOA, 11, GPIO::AFR::AlternateFunction::AF10, GPIO::PUPDR::Pull::None, GPIO::OSPEEDR::OutputSpeed::High);
constexpr static AFGPIOPin DpPin = AFGPIOPin(GPIOA, 12, GPIO::AFR::AlternateFunction::AF10, GPIO::PUPDR::Pull::None, GPIO::OSPEEDR::OutputSpeed::High);

}
}
}
}

#endif

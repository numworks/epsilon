#ifndef ION_DEVICE_N0100_CONFIG_USB_H
#define ION_DEVICE_N0100_CONFIG_USB_H

#include <regs/regs.h>

namespace Ion {
namespace Device {
namespace USB {
namespace Config {

using namespace Regs;

constexpr static AFGPIOPin VbusPin(GPIOA, 9, GPIO::AFR::AlternateFunction::AF10, GPIO::PUPDR::Pull::None, GPIO::OSPEEDR::OutputSpeed::Fast);
constexpr static AFGPIOPin DmPin(GPIOA, 11, GPIO::AFR::AlternateFunction::AF10, GPIO::PUPDR::Pull::None, GPIO::OSPEEDR::OutputSpeed::Fast);
constexpr static AFGPIOPin DpPin(GPIOA, 12, GPIO::AFR::AlternateFunction::AF10, GPIO::PUPDR::Pull::None, GPIO::OSPEEDR::OutputSpeed::Fast);

constexpr static const char * InterfaceStringDescriptor = "@Flash/0x08000000/04*016Kg,01*064Kg,07*128Kg";

}
}
}
}

#endif

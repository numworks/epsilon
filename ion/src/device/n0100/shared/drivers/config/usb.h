#ifndef ION_DEVICE_N0100_CONFIG_USB_H
#define ION_DEVICE_N0100_CONFIG_USB_H

#include <regs/regs.h>
#include <drivers/config/board.h>
#include <drivers/config/internal_flash.h>

namespace Ion {
namespace Device {
namespace USB {
namespace Config {

using namespace Regs;

constexpr static AFGPIOPin VbusPin(GPIOA, 9, GPIO::AFR::AlternateFunction::AF10, GPIO::PUPDR::Pull::None, GPIO::OSPEEDR::OutputSpeed::Fast);
constexpr static AFGPIOPin DmPin(GPIOA, 11, GPIO::AFR::AlternateFunction::AF10, GPIO::PUPDR::Pull::None, GPIO::OSPEEDR::OutputSpeed::Fast);
constexpr static AFGPIOPin DpPin(GPIOA, 12, GPIO::AFR::AlternateFunction::AF10, GPIO::PUPDR::Pull::None, GPIO::OSPEEDR::OutputSpeed::Fast);

static_assert(InternalFlash::Config::StartAddress + Board::Config::BootloaderTotalSize == 0x08004000, "Bootloader's memory area should not be accessible via DFU");

constexpr static const char * InterfaceFlashStringDescriptor = "@Internal Flash/0x08004000/03*016Kg,01*064Kg,07*128Kg";
constexpr static int BCDDevice = 0x0100;

}
}
}
}

#endif

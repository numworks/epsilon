#ifndef ION_DEVICE_N0100_CONFIG_USB_H
#define ION_DEVICE_N0100_CONFIG_USB_H

#include <drivers/config/board.h>
#include <drivers/config/internal_flash.h>
#include <drivers/config/usb_pins.h>

namespace Ion {
namespace Device {
namespace USB {
namespace Config {

static_assert(InternalFlash::Config::StartAddress + Board::Config::BootloaderSize == 0x08004000, "Bootloader's memory area should not be accessible via DFU");

constexpr static const char * InterfaceFlashStringDescriptor = "@Internal Flash/0x08004000/03*016Kg,01*064Kg,07*128Kg";
constexpr static int BCDDevice = 0x0100;

}
}
}
}

#endif

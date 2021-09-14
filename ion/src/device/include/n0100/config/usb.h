#ifndef ION_DEVICE_N0100_CONFIG_USB_H
#define ION_DEVICE_N0100_CONFIG_USB_H

#include <drivers/config/board.h>
#include <drivers/config/internal_flash.h>
#include <drivers/config/usb_pins.h>

namespace Ion {
namespace Device {
namespace USB {
namespace Config {

constexpr static const char * InterfaceFlashStringDescriptor = "@Internal Flash/0x08000000/04*016Kg,01*064Kg,07*128Kg";
constexpr static const char * InterfaceSRAMStringDescriptor = "@SRAM/0x20000000/01*220Ke";
constexpr static int BCDDevice = 0x0100;

}
}
}
}

#endif

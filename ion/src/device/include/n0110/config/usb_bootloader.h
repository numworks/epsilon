#ifndef ION_DEVICE_N0110_CONFIG_USB_H
#define ION_DEVICE_N0110_CONFIG_USB_H

namespace Ion {
namespace Device {
namespace USB {
namespace Config {

constexpr static const char * InterfaceFlashStringDescriptor = "@Flash/0x90000000/08*004Kg,01*032Kg,63*064Kg,64*064Kg";
constexpr static const char * InterfaceSRAMStringDescriptor = "@SRAM/0x20000000/01*128Ke";

constexpr static int BCDDevice = 0x0110;

}
}
}
}

#endif

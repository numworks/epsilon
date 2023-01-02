#ifndef ION_DEVICE_N0115_CONFIG_USB_USERLAND_H
#define ION_DEVICE_N0115_CONFIG_USB_USERLAND_H

namespace Ion {
namespace Device {
namespace USB {
namespace Config {

/* TODO:
 * The userland section of the running slot is said to be writable when it is
 * not. However, the limitation between the userland and the external apps is
 * 'complex' to generate. It has to be done for the descriptors to be fully
 * accurate.
 * So far, we just drop the running kernel from the flashable sectors to ensure
 * the upgrade process not to try to upgrade the running kernel + userland.
 * Indeed, they belong to the same binary, so dropping the kernel also
 * eliminates the userland. It's a dirty hack! */

constexpr static const char * InterfaceFlashStringDescriptorAuthenticatedSlotA = "@Flash/0x90010000/63*064Kg,64*064Kg";
constexpr static const char * InterfaceFlashStringDescriptorAuthenticatedSlotB = "@Flash/0x90000000/08*004Kg,01*032Kg,63*064Kg/0x90410000/63*064Kg";
constexpr static const char * InterfaceFlashStringDescriptorThirdPartySlotA = "@Flash/0x90010000/63*064Kg";
constexpr static const char * InterfaceFlashStringDescriptorThirdPartySlotB = "@Flash/0x90410000/63*064Kg";
constexpr static const char * InterfaceSRAMStringDescriptor = "@SRAM/0x20000000/01*252Ke";

}
}
}
}

#endif

#ifndef ION_DEVICE_N0110_CONFIG_USB_USERLAND_H
#define ION_DEVICE_N0110_CONFIG_USB_USERLAND_H

namespace Ion {
namespace Device {
namespace USB {
namespace Config {

/* When on Slot X, the starting address is offset by 0x00020000
 *  - The first 0x00010000 bytes are excluded to prevent overwriting the kernel
 *    of the current slot.
 *  - The second 0x00010000 bytes are excluded to prevent overwriting the
 *    userland of the current slot.
 *    > For the userland, only the first section is made unwritable, which
 *      prevents the whole userland of being overwritten while ensuring that
 *      the external apps section can still be overwritten. The external apps
 *      section is aligned on 64k, so it's guaranteed to start at least at
 *      offset 0x00020000.
 */

constexpr static const char* InterfaceFlashStringDescriptorAuthenticatedSlotA =
    "@Flash/0x90020000/62*064Kg,64*064Kg";
constexpr static const char* InterfaceFlashStringDescriptorAuthenticatedSlotB =
    "@Flash/0x90000000/08*004Kg,01*032Kg,63*064Kg/0x90420000/62*064Kg";
constexpr static const char* InterfaceFlashStringDescriptorThirdPartySlotA =
    "@Flash/0x90020000/62*064Kg";
constexpr static const char* InterfaceFlashStringDescriptorThirdPartySlotB =
    "@Flash/0x90420000/62*064Kg";
constexpr static const char* InterfaceSRAMStringDescriptor =
    "@SRAM/0x20000000/01*252Ke";

}  // namespace Config
}  // namespace USB
}  // namespace Device
}  // namespace Ion

#endif

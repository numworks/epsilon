#ifndef ION_DEVICE_N0110_CONFIG_USB_USERLAND_H
#define ION_DEVICE_N0110_CONFIG_USB_USERLAND_H

namespace Ion {
namespace Device {
namespace USB {
namespace Config {

/* When on Slot X, the starting address is offset by 0x00030000, i.e. 3 sections
 * of 0x00010000 bytes.
 *  - The first section is excluded to prevent overwriting the kernel of the
 *    current slot.
 *  - The second and third sections are excluded to prevent overwriting the
 *    extra data and/or the userland of the current slot.
 *    The layout can be:
 *    > Kernel / Extra data / Userland
 *      In this case, the second section contains the extra data and the third
 *      is the first section of the userland.
 *    > Kernel / Userland
 *      In this case, the second and third sections both belong to the userland
 *      which is always at least 2 sections long.
 * Only the one or two first sections of the userland are made unwritable, which
 * prevents the whole userland from being overwritten while ensuring that the
 * external apps section can still be overwritten. The external apps section is
 * aligned on 64k, so it's guaranteed to start at least at offset 0x00030000.
 */

constexpr static const char* InterfaceFlashStringDescriptorAuthenticatedSlotA =
    "@Flash/0x90030000/61*064Kg,64*064Kg";
constexpr static const char* InterfaceFlashStringDescriptorAuthenticatedSlotB =
    "@Flash/0x90000000/08*004Kg,01*032Kg,63*064Kg/0x90430000/61*064Kg";
constexpr static const char* InterfaceFlashStringDescriptorThirdPartySlotA =
    "@Flash/0x90030000/61*064Kg";
constexpr static const char* InterfaceFlashStringDescriptorThirdPartySlotB =
    "@Flash/0x90430000/61*064Kg";
constexpr static const char* InterfaceSRAMStringDescriptor =
    "@SRAM/0x20000000/01*252Ke";

}  // namespace Config
}  // namespace USB
}  // namespace Device
}  // namespace Ion

#endif

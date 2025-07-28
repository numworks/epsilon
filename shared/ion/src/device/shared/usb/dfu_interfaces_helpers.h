#include <shared/usb/dfu_interface.h>
#include <shared/usb/dfu_interfaces.h>
#include <shared/usb/stack/descriptor/dfu_functional_descriptor.h>
#include <shared/usb/stack/descriptor/interface_descriptor.h>
#include <shared/usb/stack/descriptor/string_descriptor.h>

namespace Ion::Device::USB {

constexpr DFUFunctionalDescriptor k_dfuFunctionalDescriptor(
    0b0011, /* bmAttributes:
             * - bitWillDetach: If true, the device will perform a bus
             *   detach-attach sequence when it receives a DFU_DETACH
             *   request. The host must not issue a USB Reset.
             * - bitManifestationTolerant: if true, the device is able
             * to communicate via USB after Manifestation phase. The
             *   manifestation phase implies a reset in the calculator,
             * so, even if the device is still plugged, it needs to be
             *   re-enumerated to communicate.
             * - bitCanUpload
             * - bitCanDnload */
    0,      /* wDetachTimeOut: Time, in milliseconds, that the device in APP
             * mode will wait after receipt of the DFU_DETACH request before
             * switching to DFU mode. It does not apply to the calculator.*/
    2048,   /* wTransferSize: Maximum number of bytes that the device can
             * accept per control-write transaction */
    0x0100  // bcdDFUVersion
);

constexpr InterfaceDescriptor helper(int id, const Descriptor* next) {
  return InterfaceDescriptor(
      0,    // bInterfaceNumber
      id,   // bAlternateSetting
      0,    // bNumEndpoints: Other than endpoint 0
      0xFE, /* bInterfaceClass: DFU
             * (http://www.usb.org/developers/defined_class) */
      1,    // bInterfaceSubClass: DFU
      2,    /* bInterfaceProtocol: DFU Mode (not DFU Runtime, which would be
             * 1) */
      id + k_interfaceStringDescriptorsOffset,  // iInterface: Index of the
                                                // Interface string, see
                                                // m_descriptor
      next);
}

}  // namespace Ion::Device::USB

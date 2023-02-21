#ifndef ION_DEVICE_SHARED_USB_STACK_WEBUSB_PLATFORM_DESCRIPTOR_H
#define ION_DEVICE_SHARED_USB_STACK_WEBUSB_PLATFORM_DESCRIPTOR_H

#include "platform_device_capability_descriptor.h"

namespace Ion {
namespace Device {
namespace USB {

class WebUSBPlatformDescriptor : public PlatformDeviceCapabilityDescriptor {
 public:
  constexpr WebUSBPlatformDescriptor(uint8_t bVendorCode, uint8_t iLandingPage)
      : PlatformDeviceCapabilityDescriptor(k_webUSBUUID),
        m_bcdVersion(0x0100),
        m_bVendorCode(bVendorCode),
        m_iLandingPage(iLandingPage) {}
  constexpr static uint8_t BLength() {
    return PlatformDeviceCapabilityDescriptor::BLength() + sizeof(uint16_t) +
           2 * sizeof(uint8_t);
  }

 protected:
  void push(Channel* c) const override;
  uint8_t bLength() const override { return BLength(); }

 private:
  /* Little-endian encoding of {3408B638-09A9-47A0-8BFD-A0768815B665}.
   * See https://wicg.github.io/webusb/#webusb-platform-capability-descriptor */
  constexpr static uint8_t k_webUSBUUID[] = {0x38, 0xB6, 0x08, 0x34, 0xA9, 0x09,
                                             0xA0, 0x47, 0x8B, 0xFD, 0xA0, 0x76,
                                             0x88, 0x15, 0xB6, 0x65};
  uint16_t m_bcdVersion;
  uint8_t m_bVendorCode;
  uint8_t m_iLandingPage;
};

}  // namespace USB
}  // namespace Device
}  // namespace Ion

#endif

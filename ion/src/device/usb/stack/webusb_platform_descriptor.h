#ifndef ION_DEVICE_USB_STACK_WEBUSB_PLATFORM_DESCRIPTOR_H
#define ION_DEVICE_USB_STACK_WEBUSB_PLATFORM_DESCRIPTOR_H

#include "device_capability_descriptor.h"

namespace Ion {
namespace USB {
namespace Device {

class WebUSBPlatformDescriptor : public DeviceCapabilityDescriptor {
public:
  constexpr WebUSBPlatformDescriptor(uint8_t bVendorCode, uint8_t iLandingPage) :
    DeviceCapabilityDescriptor(0x05),
    m_bReserved(0),
    m_platformCapabilityUUID{
      /* Little-endian encoding of {3408b638-09a9-47a0-8bfd-a0768815b665}.
       * See https://wicg.github.io/webusb/#webusb-platform-capability-descriptor */
      0x38, 0xB6, 0x08, 0x34, 0xA9, 0x09, 0xA0, 0x47,
      0x8B, 0xFD, 0xA0, 0x76, 0x88, 0x15, 0xB6, 0x65},
    m_bcdVersion(0x0100),
    m_bVendorCode(bVendorCode),
    m_iLandingPage(iLandingPage)
  {
  }
protected:
  void push(Channel * c) const override;
  virtual uint8_t bLength() const override;
private:
  constexpr static uint8_t k_platformCapabilityUUIDSize = 16;
  uint8_t m_bReserved;
  uint8_t m_platformCapabilityUUID[k_platformCapabilityUUIDSize];
  uint16_t m_bcdVersion;
  uint8_t m_bVendorCode;
  uint8_t m_iLandingPage;
};

}
}
}

#endif

#ifndef ION_DEVICE_USB_STACK_WINUSB_PLATFORM_DESCRIPTOR_H
#define ION_DEVICE_USB_STACK_WINUSB_PLATFORM_DESCRIPTOR_H

#include "platform_device_capability_descriptor.h"

namespace Ion {
namespace USB {
namespace Device {

/* We add Microsoft OS Descriptors to automatically use WinUSB drivers when the
 * calculator is plugged to a Windows computer.
 * Different sets of descriptors can be sent to different Windows versions, but
 * we use one descriptor set only. */

class WinUSBPlatformDescriptor : public PlatformDeviceCapabilityDescriptor {
public:
  constexpr WinUSBPlatformDescriptor(uint16_t wMSOSDescriptorSetTotalLength, uint8_t bMS_VendorCode) :
    PlatformDeviceCapabilityDescriptor(k_winUSBUUID),
    m_dwWindowsVersion(0x06030000), // Windows 8.1, it cannot be lower
    m_wMSOSDescriptorSetTotalLength(wMSOSDescriptorSetTotalLength),
    m_bMS_VendorCode(bMS_VendorCode),
    m_bAltEnumCode(0) // No alternative enumeration descriptors set
  {
  }
protected:
  void push(Channel * c) const override;
  virtual uint8_t bLength() const override;
private:
  /* Little-endian encoding of {D8DD60DF-4589-4CC7-9CD2-659D9E648A9F}.
   * See Microsoft OS 2.0 Descriptors Specification. */
  constexpr static uint8_t k_winUSBUUID[] = {
    0xDF, 0x60, 0xDD, 0xD8, 0x89, 0x45, 0xC7, 0x4C,
    0x9C, 0xD2, 0x65, 0x9D, 0x9E, 0x64, 0x8A, 0x9F};
  uint32_t m_dwWindowsVersion; // Minimum version of Windows that the descriptor set can be applied to
  uint16_t m_wMSOSDescriptorSetTotalLength;
  uint8_t m_bMS_VendorCode;
  uint8_t m_bAltEnumCode;
};

}
}
}

#endif

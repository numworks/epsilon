#ifndef ION_DEVICE_SHARED_USB_STACK_BOS_DESCRIPTOR_H
#define ION_DEVICE_SHARED_USB_STACK_BOS_DESCRIPTOR_H

#include "descriptor.h"
#include "device_capability_descriptor.h"

namespace Ion {
namespace Device {
namespace USB {

class BOSDescriptor : public Descriptor {
public:
  constexpr BOSDescriptor(
      uint16_t wTotalLength,
      uint8_t bNumDeviceCapabilities,
      const DeviceCapabilityDescriptor * deviceCapabilities) :
    Descriptor(0x0F),
    m_wTotalLength(wTotalLength),
    m_bNumDeviceCaps(bNumDeviceCapabilities),
    m_deviceCapabilities(deviceCapabilities)
  {
  }
protected:
  void push(Channel * c) const override;
  uint8_t bLength() const override;
private:
  uint16_t m_wTotalLength;
  uint8_t m_bNumDeviceCaps;
  const DeviceCapabilityDescriptor * m_deviceCapabilities;
};

}
}
}

#endif

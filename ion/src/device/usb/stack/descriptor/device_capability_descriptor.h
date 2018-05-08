#ifndef ION_DEVICE_USB_STACK_DEVICE_CAPABLITY_DESCRIPTOR_H
#define ION_DEVICE_USB_STACK_DEVICE_CAPABLITY_DESCRIPTOR_H

#include "descriptor.h"

namespace Ion {
namespace USB {
namespace Device {

class BOSDescriptor;

class DeviceCapabilityDescriptor : public Descriptor {
  friend class BOSDescriptor;
public:
  constexpr DeviceCapabilityDescriptor(uint8_t bDeviceCapabilityType) :
    Descriptor(0x10),
    m_bDeviceCapabilityType(bDeviceCapabilityType)
  {
  }
protected:
  void push(Channel * c) const override;
  virtual uint8_t bLength() const override;
private:
  uint8_t m_bDeviceCapabilityType;
};

}
}
}

#endif

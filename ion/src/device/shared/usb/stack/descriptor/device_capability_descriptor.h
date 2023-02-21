#ifndef ION_DEVICE_SHARED_USB_STACK_DEVICE_CAPABLITY_DESCRIPTOR_H
#define ION_DEVICE_SHARED_USB_STACK_DEVICE_CAPABLITY_DESCRIPTOR_H

#include "descriptor.h"

namespace Ion {
namespace Device {
namespace USB {

class BOSDescriptor;

class DeviceCapabilityDescriptor : public Descriptor {
  friend class BOSDescriptor;

 public:
  constexpr DeviceCapabilityDescriptor(uint8_t bDeviceCapabilityType)
      : Descriptor(0x10), m_bDeviceCapabilityType(bDeviceCapabilityType) {}
  constexpr static uint8_t BLength() {
    return Descriptor::BLength() + sizeof(uint8_t);
  }

 protected:
  void push(Channel* c) const override;
  uint8_t bLength() const override { return BLength(); }

 private:
  uint8_t m_bDeviceCapabilityType;
};

}  // namespace USB
}  // namespace Device
}  // namespace Ion

#endif

#ifndef ION_DEVICE_SHARED_USB_STACK_PLATFORM_DEVICE_CAPABLITY_DESCRIPTOR_H
#define ION_DEVICE_SHARED_USB_STACK_PLATFORM_DEVICE_CAPABLITY_DESCRIPTOR_H

#include "device_capability_descriptor.h"

namespace Ion {
namespace Device {
namespace USB {

class PlatformDeviceCapabilityDescriptor : public DeviceCapabilityDescriptor {
 public:
  constexpr PlatformDeviceCapabilityDescriptor(
      const uint8_t platformCapabilityUUID[])
      : DeviceCapabilityDescriptor(0x05),
        m_bReserved(0),
        m_platformCapabilityUUID{
            platformCapabilityUUID[0],  platformCapabilityUUID[1],
            platformCapabilityUUID[2],  platformCapabilityUUID[3],
            platformCapabilityUUID[4],  platformCapabilityUUID[5],
            platformCapabilityUUID[6],  platformCapabilityUUID[7],
            platformCapabilityUUID[8],  platformCapabilityUUID[9],
            platformCapabilityUUID[10], platformCapabilityUUID[11],
            platformCapabilityUUID[12], platformCapabilityUUID[13],
            platformCapabilityUUID[14], platformCapabilityUUID[15]} {}
  constexpr static uint8_t BLength() {
    return DeviceCapabilityDescriptor::BLength() + sizeof(uint8_t) +
           k_platformCapabilityUUIDSize * sizeof(uint8_t);
  }

 protected:
  void push(Channel* c) const override;
  uint8_t bLength() const override { return BLength(); }

 private:
  constexpr static uint8_t k_platformCapabilityUUIDSize = 16;
  uint8_t m_bReserved;
  uint8_t m_platformCapabilityUUID[k_platformCapabilityUUIDSize];
};

}  // namespace USB
}  // namespace Device
}  // namespace Ion

#endif

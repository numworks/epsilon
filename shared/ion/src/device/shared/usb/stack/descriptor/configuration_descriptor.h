#ifndef ION_DEVICE_SHARED_USB_STACK_CONFIGURATION_DESCRIPTOR_H
#define ION_DEVICE_SHARED_USB_STACK_CONFIGURATION_DESCRIPTOR_H

#include "descriptor.h"
#include "interface_descriptor.h"

namespace Ion {
namespace Device {
namespace USB {

class ConfigurationDescriptor : public Descriptor {
 public:
  constexpr ConfigurationDescriptor(uint16_t wTotalLength,
                                    uint8_t bNumInterfaces,
                                    uint8_t bConfigurationValue,
                                    uint8_t iConfiguration,
                                    uint8_t bmAttributes, uint8_t bMaxPower,
                                    const InterfaceDescriptor* interfaces)
      : Descriptor(0x02),
        m_wTotalLength(wTotalLength),
        m_bNumInterfaces(bNumInterfaces),
        m_bConfigurationValue(bConfigurationValue),
        m_iConfiguration(iConfiguration),
        m_bmAttributes(bmAttributes),
        m_bMaxPower(bMaxPower),
        m_interfaces(interfaces) {}
  constexpr static uint8_t BLength() {
    return Descriptor::BLength() + sizeof(uint16_t) + 5 * sizeof(uint8_t);
  }

 protected:
  void push(Channel* c) const override;
  uint8_t bLength() const override { return BLength(); }

 private:
  uint16_t m_wTotalLength;
  uint8_t m_bNumInterfaces;
  uint8_t m_bConfigurationValue;
  uint8_t m_iConfiguration;
  uint8_t m_bmAttributes;
  uint8_t m_bMaxPower;
  const InterfaceDescriptor* m_interfaces;
};

}  // namespace USB
}  // namespace Device
}  // namespace Ion

#endif

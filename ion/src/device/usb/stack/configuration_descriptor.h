#ifndef ION_DEVICE_USB_STACK_CONFIGURATION_DESCRIPTOR_H
#define ION_DEVICE_USB_STACK_CONFIGURATION_DESCRIPTOR_H

#include "descriptor.h"
#include "interface_descriptor.h"

namespace Ion {
namespace USB {
namespace Device {

class ConfigurationDescriptor : public Descriptor {
public:
  constexpr ConfigurationDescriptor(
      uint16_t wTotalLength,
      uint8_t bNumInterfaces,
      uint8_t bConfigurationValue,
      uint8_t iConfiguration,
      uint8_t bmAttributes,
      uint8_t bMaxPower,
      const InterfaceDescriptor * interfaces) :
    Descriptor(DescriptorType, DescriptorSize),
    m_wTotalLength(wTotalLength),
    m_bNumInterfaces(bNumInterfaces),
    m_bConfigurationValue(bConfigurationValue),
    m_iConfiguration(iConfiguration),
    m_bmAttributes(bmAttributes),
    m_bMaxPower(bMaxPower),
    m_interfaces(interfaces)
  {
  }
  uint16_t copy(void * target, size_t maxSize) const override;
private:
  constexpr static uint8_t DescriptorSize = sizeof(Descriptor) + sizeof(uint16_t) + 5*sizeof(uint8_t);
  constexpr static uint8_t DescriptorType = 0x02;
  uint16_t m_wTotalLength;
  uint8_t m_bNumInterfaces;
  uint8_t m_bConfigurationValue;
  uint8_t m_iConfiguration;
  uint8_t m_bmAttributes;
  uint8_t m_bMaxPower;
  const InterfaceDescriptor * m_interfaces;
} __attribute__((packed));

static_assert(sizeof(ConfigurationDescriptor) == sizeof(Descriptor) + sizeof(uint16_t) + 5 * sizeof(uint8_t) + sizeof(void *));

}
}
}

#endif

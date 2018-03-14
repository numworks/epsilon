#ifndef ION_DEVICE_USB_STACK_INTERFACE_DESCRIPTOR_H
#define ION_DEVICE_USB_STACK_INTERFACE_DESCRIPTOR_H

#include "descriptor.h"

namespace Ion {
namespace USB {
namespace Device {

class InterfaceDescriptor : public Descriptor {
public:
  constexpr InterfaceDescriptor(
      uint8_t bInterfaceNumber,
      uint8_t bAlternateSetting,
      uint8_t bNumEndpoints,
      uint8_t bInterfaceClass,
      uint8_t bInterfaceSubClass,
      uint8_t bInterfaceProtocol,
      uint8_t iInterface) :
    Descriptor(DescriptorSize, DescriptorType),
    m_bInterfaceNumber(bInterfaceNumber),
    m_bAlternateSetting(bAlternateSetting),
    m_bNumEndpoints(bNumEndpoints),
    m_bInterfaceClass(bInterfaceClass),
    m_bInterfaceSubClass(bInterfaceSubClass),
    m_bInterfaceProtocol(bInterfaceProtocol),
    m_iInterface(iInterface)
  {
  }
private:
  constexpr static uint8_t DescriptorSize = Descriptor::sizeOfAttributes() + 7*sizeof(uint8_t);
  constexpr static uint8_t DescriptorType = 0x04;
  uint8_t m_bInterfaceNumber;
  uint8_t m_bAlternateSetting;
  uint8_t m_bNumEndpoints;
  uint8_t m_bInterfaceClass;
  uint8_t m_bInterfaceSubClass;
  uint8_t m_bInterfaceProtocol;
  uint8_t m_iInterface;
} __attribute__((packed));

static_assert(sizeof(InterfaceDescriptor) == sizeof(Descriptor) + 7*sizeof(uint8_t));

}
}
}

#endif

#ifndef ION_DEVICE_USB_STACK_STRING_DESCRIPTOR_H
#define ION_DEVICE_USB_STACK_STRING_DESCRIPTOR_H

#include "descriptor.h"

namespace Ion {
namespace USB {
namespace Device {

class StringDescriptor : public Descriptor {
public:
  constexpr StringDescriptor(const char * string) :
    Descriptor(Descriptor::sizeOfAttributes() +2*constexprstrlen(string), DescriptorType), // The script is returned in UTF-16, hence the multiplication.
    m_string(string)
  {
  }
  uint16_t copy(void * target, size_t maxSize) const override;
private:
  static constexpr size_t constexprstrlen(const char * str) {
    return (*str == 0) ? 0 : constexprstrlen(str + 1) + 1;
  }
  constexpr static uint8_t DescriptorType = 0x03;
  const char * m_string;
};

}
}
}

#endif

#ifndef ION_DEVICE_USB_STACK_STRING_DESCRIPTOR_H
#define ION_DEVICE_USB_STACK_STRING_DESCRIPTOR_H

#include "descriptor.h"

namespace Ion {
namespace USB {
namespace Device {

class StringDescriptor : public Descriptor {
public:
  constexpr StringDescriptor(const char * string) :
    Descriptor(0x03),
    m_string(string)
  {
  }
protected:
  void push(Channel * c) const override;
  virtual uint8_t bLength() const override;
private:
  const char * m_string;
};

}
}
}

#endif

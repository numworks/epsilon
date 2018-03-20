#ifndef ION_DEVICE_USB_STACK_STRING_DESCRIPTOR_H
#define ION_DEVICE_USB_STACK_STRING_DESCRIPTOR_H

#include "descriptor.h"

namespace Ion {
namespace USB {
namespace Device {

class StringDescriptor : public Descriptor {
public:
  StringDescriptor() : Descriptor(0x03) { }
protected:
  void push(Channel * c) const override;
  virtual uint8_t bLength() const override;
  virtual const char * string() const = 0;
};

}
}
}

#endif

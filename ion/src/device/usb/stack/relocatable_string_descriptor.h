#ifndef ION_DEVICE_USB_STACK_RELOCATABLE_STRING_DESCRIPTOR_H
#define ION_DEVICE_USB_STACK_RELOCATABLE_STRING_DESCRIPTOR_H

#include "string_descriptor.h"
#include <string.h>

namespace Ion {
namespace USB {
namespace Device {

template <int T>
class RelocatableStringDescriptor : public StringDescriptor {
public:
  RelocatableStringDescriptor(const char * string) :
    StringDescriptor()
  {
    strlcpy(m_string, string, T);
  }
protected:
  const char * string() const override {
    return m_string;
  }
private:
  char m_string[T];
};

}
}
}

#endif

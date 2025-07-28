#ifndef ION_DEVICE_SHARED_USB_STACK_STRING_DESCRIPTOR_H
#define ION_DEVICE_SHARED_USB_STACK_STRING_DESCRIPTOR_H

#include "descriptor.h"

namespace Ion {
namespace Device {
namespace USB {

class StringDescriptor : public Descriptor {
 public:
  constexpr StringDescriptor(const char* string)
      : Descriptor(0x03), m_string(string) {}

 protected:
  void push(Channel* c) const override;
  uint8_t bLength() const override;

 private:
  const char* m_string;
};

}  // namespace USB
}  // namespace Device
}  // namespace Ion

#endif

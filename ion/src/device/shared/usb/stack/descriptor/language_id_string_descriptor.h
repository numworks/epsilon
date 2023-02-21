#ifndef ION_DEVICE_SHARED_USB_STACK_LANGUAGE_ID_STRING_DESCRIPTOR_H
#define ION_DEVICE_SHARED_USB_STACK_LANGUAGE_ID_STRING_DESCRIPTOR_H

#include "descriptor.h"

namespace Ion {
namespace Device {
namespace USB {

// For now this LanguageIDStringDescriptor only ever returns American English

class LanguageIDStringDescriptor : public Descriptor {
 public:
  constexpr LanguageIDStringDescriptor() : Descriptor(0x03) {}

 protected:
  void push(Channel* c) const override;
  uint8_t bLength() const override;
};

}  // namespace USB
}  // namespace Device
}  // namespace Ion

#endif

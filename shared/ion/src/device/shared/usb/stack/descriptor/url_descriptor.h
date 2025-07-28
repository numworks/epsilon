#ifndef ION_DEVICE_SHARED_USB_STACK_URL_DESCRIPTOR_H
#define ION_DEVICE_SHARED_USB_STACK_URL_DESCRIPTOR_H

#include "descriptor.h"

namespace Ion {
namespace Device {
namespace USB {

class URLDescriptor : public Descriptor {
 public:
  enum class Scheme { HTTP = 0, HTTPS = 1, IncludedInURL = 255 };

  constexpr URLDescriptor(Scheme scheme, const char* url)
      : Descriptor(0x03), m_bScheme((uint8_t)scheme), m_string(url) {}

 protected:
  void push(Channel* c) const override;
  uint8_t bLength() const override;

 private:
  uint8_t m_bScheme;
  const char* m_string;
};

}  // namespace USB
}  // namespace Device
}  // namespace Ion

#endif

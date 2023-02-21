#ifndef ION_DEVICE_SHARED_USB_STACK_MICROSOFT_OS_STRING_DESCRIPTOR_H
#define ION_DEVICE_SHARED_USB_STACK_MICROSOFT_OS_STRING_DESCRIPTOR_H

#include "string_descriptor.h"

namespace Ion {
namespace Device {
namespace USB {

class MicrosoftOSStringDescriptor : public StringDescriptor {
 public:
  constexpr MicrosoftOSStringDescriptor(uint8_t bMSVendorCode)
      : StringDescriptor("MSFT100"),
        m_bMSVendorCode(bMSVendorCode),
        m_bPad(0) {}

 protected:
  void push(Channel* c) const override;
  uint8_t bLength() const override;

 private:
  uint8_t m_bMSVendorCode;
  uint8_t m_bPad;
};

}  // namespace USB
}  // namespace Device
}  // namespace Ion

#endif

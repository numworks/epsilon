#ifndef ION_DEVICE_SHARED_USB_STACK_DESCRIPTOR_H
#define ION_DEVICE_SHARED_USB_STACK_DESCRIPTOR_H

#include "../streamable.h"

namespace Ion {
namespace Device {
namespace USB {

class InterfaceDescriptor;

class Descriptor : public Streamable {
  friend class InterfaceDescriptor;

 public:
  constexpr Descriptor(uint8_t bDescriptorType)
      : m_bDescriptorType(bDescriptorType) {}
  uint8_t type() const { return m_bDescriptorType; }
  constexpr static uint8_t BLength() { return 2 * sizeof(uint8_t); }

 protected:
  void push(Channel* c) const override;
  virtual uint8_t bLength() const { return BLength(); }

 private:
  uint8_t m_bDescriptorType;
};

}  // namespace USB
}  // namespace Device
}  // namespace Ion

#endif

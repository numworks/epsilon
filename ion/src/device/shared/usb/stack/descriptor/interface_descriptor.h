#ifndef ION_DEVICE_SHARED_USB_STACK_INTERFACE_DESCRIPTOR_H
#define ION_DEVICE_SHARED_USB_STACK_INTERFACE_DESCRIPTOR_H

#include "descriptor.h"

namespace Ion {
namespace Device {
namespace USB {

class ConfigurationDescriptor;

class InterfaceDescriptor : public Descriptor {
  friend class ConfigurationDescriptor;

 public:
  constexpr InterfaceDescriptor(uint8_t bInterfaceNumber,
                                uint8_t bAlternateSetting,
                                uint8_t bNumEndpoints, uint8_t bInterfaceClass,
                                uint8_t bInterfaceSubClass,
                                uint8_t bInterfaceProtocol, uint8_t iInterface,
                                Descriptor* additionalDescriptor)
      : Descriptor(0x04),
        m_bInterfaceNumber(bInterfaceNumber),
        m_bAlternateSetting(bAlternateSetting),
        m_bNumEndpoints(bNumEndpoints),
        m_bInterfaceClass(bInterfaceClass),
        m_bInterfaceSubClass(bInterfaceSubClass),
        m_bInterfaceProtocol(bInterfaceProtocol),
        m_iInterface(iInterface),
        m_additionalDescriptor(additionalDescriptor)
  /* There could be more than one additional descriptor, but we do not need
   * this for now. */
  {}
  constexpr static uint8_t BLength() {
    return Descriptor::BLength() + 7 * sizeof(uint8_t);
  }

 protected:
  void push(Channel* c) const override;
  uint8_t bLength() const override { return BLength(); }

 private:
  uint8_t m_bInterfaceNumber;
  uint8_t m_bAlternateSetting;
  uint8_t m_bNumEndpoints;
  uint8_t m_bInterfaceClass;
  uint8_t m_bInterfaceSubClass;
  uint8_t m_bInterfaceProtocol;
  uint8_t m_iInterface;
  const Descriptor* m_additionalDescriptor;
};

}  // namespace USB
}  // namespace Device
}  // namespace Ion

#endif

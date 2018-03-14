#ifndef ION_DEVICE_USB_STACK_DESCRIPTOR_H
#define ION_DEVICE_USB_STACK_DESCRIPTOR_H

#include <stdint.h>
#include <stddef.h>

namespace Ion {
namespace USB {
namespace Device {

class Descriptor {
public:
  constexpr Descriptor(uint8_t bLength, uint8_t bDescriptorType) :
    m_bLength(bLength),
    m_bDescriptorType(bDescriptorType)
  {
  }
  uint16_t copyHeaderOnly(void * target, size_t maxSize) const;
  virtual uint16_t copy(void * target, size_t maxSize) const;
  uint8_t type() const { return m_bDescriptorType; }
protected:
  constexpr static uint8_t sizeOfAttributes() { return 2*sizeof(uint8_t); }
  uint8_t m_bLength;
private:
  uint8_t m_bDescriptorType;
} __attribute__((packed));

/* We assert that the class is packed (no padding has been inserted between its
 * attributes). The size of this class should be the size of its vtable pointer
 * (which exists because there is a virtual method), plus the sum of the sizes
 * of its attributes. */
static_assert(sizeof(Descriptor) == sizeof(void *) + 2*sizeof(uint8_t));
}
}
}

#endif

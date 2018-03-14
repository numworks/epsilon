#include "descriptor.h"
#include <string.h>

static inline size_t min(size_t a, size_t b) {
  return (a < b) ? a : b;
}

namespace Ion {
namespace USB {
namespace Device {

uint16_t Descriptor::copyHeaderOnly(void * target, size_t maxSize) const {
  return Descriptor::copy(target, min(maxSize, sizeof(m_bLength) + sizeof(m_bDescriptorType)));
}

uint16_t Descriptor::copy(void * target, size_t maxSize) const {
  uint16_t size = min(m_bLength, maxSize);
  /* Descriptor has a virtual function, its memory space thus starts with a
   * vtable. To copy the object, start copying at the address of the first
   * attribute, here m_blength. */
  memcpy(target, &m_bLength, size);
  return size;
}

}
}
}

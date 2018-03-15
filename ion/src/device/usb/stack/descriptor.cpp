#include "descriptor.h"
#include <string.h>

static inline size_t min(size_t a, size_t b) {
  return (a < b) ? a : b;
}

namespace Ion {
namespace USB {
namespace Device {

uint16_t Descriptor::copy(void * target, size_t maxSize) const {
  Channel c(target, maxSize);
  push(&c);
  return maxSize - c.sizeLeft();
}

void Descriptor::push(Channel * c) const {
  c->push(bLength());
  c->push(m_bDescriptorType);
}

}
}
}

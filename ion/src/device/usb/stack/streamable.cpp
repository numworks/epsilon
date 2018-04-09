#include "streamable.h"

namespace Ion {
namespace USB {
namespace Device {

uint16_t Streamable::copy(void * target, size_t maxSize) const {
  Channel c(target, maxSize);
  push(&c);
  return maxSize - c.sizeLeft();
}

}
}
}

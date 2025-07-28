#include "streamable.h"

namespace Ion {
namespace Device {
namespace USB {

uint16_t Streamable::copy(void* target, size_t maxSize) const {
  Channel c(target, maxSize);
  push(&c);
  return maxSize - c.sizeLeft();
}

}  // namespace USB
}  // namespace Device
}  // namespace Ion

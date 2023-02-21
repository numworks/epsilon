#include "descriptor.h"

#include <string.h>

namespace Ion {
namespace Device {
namespace USB {

void Descriptor::push(Channel* c) const {
  c->push(bLength());
  c->push(m_bDescriptorType);
}

}  // namespace USB
}  // namespace Device
}  // namespace Ion

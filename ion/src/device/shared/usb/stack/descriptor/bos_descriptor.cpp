#include "bos_descriptor.h"

namespace Ion {
namespace Device {
namespace USB {

void BOSDescriptor::push(Channel* c) const {
  Descriptor::push(c);
  c->push(m_wTotalLength);
  c->push(m_bNumDeviceCaps);
  for (uint8_t i = 0; i < m_bNumDeviceCaps; i++) {
    m_deviceCapabilities[i].push(c);
  }
}

}  // namespace USB
}  // namespace Device
}  // namespace Ion

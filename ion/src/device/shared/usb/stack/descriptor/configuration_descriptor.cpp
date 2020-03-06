#include "configuration_descriptor.h"

namespace Ion {
namespace Device {
namespace USB {

void ConfigurationDescriptor::push(Channel * c) const {
  Descriptor::push(c);
  c->push(m_wTotalLength);
  c->push(m_bNumInterfaces);
  c->push(m_bConfigurationValue);
  c->push(m_iConfiguration);
  c->push(m_bmAttributes);
  c->push(m_bMaxPower);
  for (uint8_t i = 0; i < m_bNumInterfaces; i++) {
    m_interfaces[i].push(c);
  }
}

uint8_t ConfigurationDescriptor::bLength() const {
  return Descriptor::bLength() + sizeof(uint16_t) + 5*sizeof(uint8_t);
}

}
}
}

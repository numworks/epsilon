#include "interface_descriptor.h"

namespace Ion {
namespace USB {
namespace Device {

void InterfaceDescriptor::push(Channel * c) const {
  Descriptor::push(c);
  c->push(m_bInterfaceNumber);
  c->push(m_bAlternateSetting);
  c->push(m_bNumEndpoints);
  c->push(m_bInterfaceClass);
  c->push(m_bInterfaceSubClass);
  c->push(m_bInterfaceProtocol);
  c->push(m_iInterface);
  if (m_additionalDescriptor != nullptr) {
    m_additionalDescriptor->push(c);
  }
}

uint8_t InterfaceDescriptor::bLength() const {
  return Descriptor::bLength() + 7*sizeof(uint8_t);
}

}
}
}

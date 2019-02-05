#include "dfu_functional_descriptor.h"

namespace Ion {
namespace Device {
namespace USB {

void DFUFunctionalDescriptor::push(Channel * c) const {
  Descriptor::push(c);
  c->push(m_bmAttributes);
  c->push(m_wDetachTimeOut);
  c->push(m_wTransferSize);
  c->push(m_bcdDFUVersion);
}

uint8_t DFUFunctionalDescriptor::bLength() const {
  return Descriptor::bLength() + sizeof(uint8_t) + 3*sizeof(uint16_t);
}

}
}
}

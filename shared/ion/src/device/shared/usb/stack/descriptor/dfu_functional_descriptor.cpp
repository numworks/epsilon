#include "dfu_functional_descriptor.h"

namespace Ion {
namespace Device {
namespace USB {

void DFUFunctionalDescriptor::push(Channel* c) const {
  Descriptor::push(c);
  c->push(m_bmAttributes);
  c->push(m_wDetachTimeOut);
  c->push(m_wTransferSize);
  c->push(m_bcdDFUVersion);
}

}  // namespace USB
}  // namespace Device
}  // namespace Ion

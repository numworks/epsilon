#include "microsoft_os_string_descriptor.h"

namespace Ion {
namespace USB {
namespace Device {

void MicrosoftOSStringDescriptor::push(Channel * c) const {
  StringDescriptor::push(c);
  c->push(m_bMSVendorCode);
  c->push(m_bPad);
}

uint8_t MicrosoftOSStringDescriptor::bLength() const {
  return StringDescriptor::bLength() + 2 * sizeof(uint8_t);
}

}
}
}

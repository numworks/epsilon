#include "url_descriptor.h"

#include <string.h>

namespace Ion {
namespace Device {
namespace USB {

void URLDescriptor::push(Channel* c) const {
  Descriptor::push(c);
  c->push(m_bScheme);
  const char* stringPointer = m_string;
  while (*stringPointer != 0) {
    c->push(*stringPointer);
    stringPointer++;
  }
}

uint8_t URLDescriptor::bLength() const {
  // The script is returned in UTF-8.
  return Descriptor::bLength() + sizeof(uint8_t) + strlen(m_string);
}

}  // namespace USB
}  // namespace Device
}  // namespace Ion

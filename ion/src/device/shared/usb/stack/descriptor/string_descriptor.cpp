#include "string_descriptor.h"

#include <string.h>

namespace Ion {
namespace Device {
namespace USB {

void StringDescriptor::push(Channel* c) const {
  Descriptor::push(c);
  const char* stringPointer = m_string;
  while (*stringPointer != 0) {
    uint16_t stringAsUTF16CodePoint = *stringPointer;
    c->push(stringAsUTF16CodePoint);
    stringPointer++;
  }
}

uint8_t StringDescriptor::bLength() const {
  // The script is returned in UTF-16, hence the multiplication.
  return Descriptor::bLength() + 2 * strlen(m_string);
}

}  // namespace USB
}  // namespace Device
}  // namespace Ion

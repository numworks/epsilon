#include "language_id_string_descriptor.h"
#include <string.h>

namespace Ion {
namespace USB {
namespace Device {

void LanguageIDStringDescriptor::push(Channel * c) const {
  Descriptor::push(c);
  c->push((uint16_t)(0x0409));
}

uint8_t LanguageIDStringDescriptor::bLength() const {
  return Descriptor::bLength() + sizeof(uint16_t);
}

}
}
}

#include "language_id_string_descriptor.h"
#include <string.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))

namespace Ion {
namespace USB {
namespace Device {

void LanguageIDStringDescriptor::push(Channel * c) const {
  Descriptor::push(c);
  for (uint8_t i = 0; i<m_languageIDCount; i++) {
    c->push(m_languageIDs[i]);
  }
}

uint8_t LanguageIDStringDescriptor::bLength() const {
  return Descriptor::bLength() + m_languageIDCount * sizeof(uint16_t);
}

}
}
}

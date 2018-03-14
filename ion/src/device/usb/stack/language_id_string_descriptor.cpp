#include "language_id_string_descriptor.h"
#include <string.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))

namespace Ion {
namespace USB {
namespace Device {

uint16_t LanguageIDStringDescriptor::copy(void * target, size_t maxSize) const {
  uint16_t size = MIN(maxSize, sizeof(Descriptor));
  memcpy(target, &m_bLength, size);
  for (uint8_t i = 0; i < m_languageIDCount; i++) {
    uint16_t languageIDsize = MIN(maxSize - sizeof(Descriptor), sizeof(uint16_t));
    memcpy(((uint8_t *)target) + sizeof(Descriptor) + i*sizeof(uint16_t), m_languageIDs+i, languageIDsize);
    size += languageIDsize;
  }
  return size;
}

}
}
}

#include "calculator.h"

namespace Ion {
namespace USB {
namespace Device {

constexpr char Calculator::serialNumber[];
constexpr uint16_t Calculator::languageIDEnglishUS;

Descriptor * Calculator::descriptor(uint8_t type, uint8_t index) {
  int typeCount = 0;
  for (size_t i=0; i<sizeof(m_descriptors)/sizeof(m_descriptors[0]); i++) {
    Descriptor * descriptor = m_descriptors[i];
    if (descriptor->type() != type) {
      continue;
    }
    if (typeCount == index) {
      return descriptor;
    } else {
      typeCount++;
    }
  }
  return nullptr;
}

}
}
}

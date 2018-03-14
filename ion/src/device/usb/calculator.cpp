#include "calculator.h"

namespace Ion {
namespace USB {
namespace Device {

constexpr char Calculator::serialNumber[];
constexpr uint16_t Calculator::languageIDEnglishUS;

Descriptor * Calculator::descriptor(uint8_t type, uint8_t index) {
  for (int i = 0; i<k_numberOfDescriptors; i++) {
    if (m_descriptors[i]->type() == type) {
      // Find the index-ed descriptor of type "type"
      if (index == 0) {
        return m_descriptors[i];
      }
      index--;
    }
  }
  return nullptr;
}

}
}
}

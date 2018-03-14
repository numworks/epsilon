#ifndef ION_DEVICE_USB_STACK_LANGUAGE_ID_STRING_DESCRIPTOR_H
#define ION_DEVICE_USB_STACK_LANGUAGE_ID_STRING_DESCRIPTOR_H

#include "descriptor.h"

namespace Ion {
namespace USB {
namespace Device {

class LanguageIDStringDescriptor : public Descriptor {
public:
  constexpr LanguageIDStringDescriptor(const uint16_t * languageIDs, uint8_t languageIDCount) :
    Descriptor(Descriptor::sizeOfAttributes() + languageIDCount * sizeof(uint16_t), DescriptorType),
    m_languageIDs(languageIDs),
    m_languageIDCount(languageIDCount)
  {
  }
  uint16_t copy(void * target, size_t maxSize) const override;
private:
  constexpr static uint8_t DescriptorType = 0x03;
  const uint16_t * m_languageIDs;
  uint8_t m_languageIDCount;
};

}
}
}

#endif

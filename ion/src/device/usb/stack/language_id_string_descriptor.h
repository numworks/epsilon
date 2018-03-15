#ifndef ION_DEVICE_USB_STACK_LANGUAGE_ID_STRING_DESCRIPTOR_H
#define ION_DEVICE_USB_STACK_LANGUAGE_ID_STRING_DESCRIPTOR_H

#include "descriptor.h"

namespace Ion {
namespace USB {
namespace Device {

class LanguageIDStringDescriptor : public Descriptor {
public:
  constexpr LanguageIDStringDescriptor(const uint16_t * languageIDs, uint8_t languageIDCount) :
    Descriptor(0x03),
    m_languageIDs(languageIDs),
    m_languageIDCount(languageIDCount)
  {
  }
protected:
  void push(Channel * c) const override;
  virtual uint8_t bLength() const override;
private:
  const uint16_t * m_languageIDs;
  uint8_t m_languageIDCount;
};

}
}
}

#endif

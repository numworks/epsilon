#ifndef ION_DEVICE_SHARED_USB_STACK_DFU_FUNCTIONAL_DESCRIPTOR_H
#define ION_DEVICE_SHARED_USB_STACK_DFU_FUNCTIONAL_DESCRIPTOR_H

#include "descriptor.h"

namespace Ion {
namespace Device {
namespace USB {

class DFUFunctionalDescriptor : public Descriptor {
public:
  constexpr DFUFunctionalDescriptor(
      uint8_t bmAttributes,
      uint16_t wDetachTimeOut,
      uint16_t wTransferSize,
      uint16_t bcdDFUVersion) :
    Descriptor(0x21),
    m_bmAttributes(bmAttributes),
    m_wDetachTimeOut(wDetachTimeOut),
    m_wTransferSize(wTransferSize),
    m_bcdDFUVersion(bcdDFUVersion)
  {
  }
protected:
  void push(Channel * c) const override;
  uint8_t bLength() const override;
private:
  uint8_t m_bmAttributes;
  uint16_t m_wDetachTimeOut;
  uint16_t m_wTransferSize;
  uint16_t m_bcdDFUVersion;
};

}
}
}

#endif

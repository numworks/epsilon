#ifndef ION_DEVICE_SHARED_USB_STACK_EXTENDED_COMPAT_ID_DESCRIPTOR_H
#define ION_DEVICE_SHARED_USB_STACK_EXTENDED_COMPAT_ID_DESCRIPTOR_H

#include "../streamable.h"

namespace Ion {
namespace Device {
namespace USB {

/* We use this descriptor to tell the Windows OS that the device should be
 * treated as a WinUSB device. The Extended Compat ID Descriptor can set
 * differents compat IDs according to the interface and function of the device,
 * but we assume there is only one. */

class ExtendedCompatIDDescriptor : public Streamable {
 public:
  constexpr static uint8_t Index = 0x0004;
  ExtendedCompatIDDescriptor(const char* compatibleID);

 protected:
  void push(Channel* c) const override;

 private:
  constexpr static uint8_t k_reserved1Size = 7;
  constexpr static uint8_t k_compatibleIDSize = 8;
  constexpr static uint8_t k_reserved2Size = 6;
  // Header
  uint32_t m_dwLength;  // The length, in bytes, of the complete extended compat
                        // ID descriptor
  uint16_t m_bcdVersion;  // The descriptor’s version number, in binary coded
                          // decimal format
  uint16_t m_wIndex;      // An index that identifies the particular OS feature
                          // descriptor
  uint8_t m_bCount;       // The number of function sections
  uint8_t m_reserved1[k_reserved1Size];
  // Function
  uint8_t m_bFirstInterfaceNumber;  // The interface or function number
  uint8_t m_bReserved;
  uint8_t m_compatibleID[k_compatibleIDSize];
  uint8_t m_subCompatibleID[k_compatibleIDSize];
  uint8_t m_reserved2[k_reserved2Size];
};

}  // namespace USB
}  // namespace Device
}  // namespace Ion

#endif

#ifndef ION_DEVICE_SHARED_USB_SETUP_PACKET_H
#define ION_DEVICE_SHARED_USB_SETUP_PACKET_H

#include <stdint.h>

namespace Ion {
namespace Device {
namespace USB {

class SetupPacket {
 public:
  enum class TransactionType {
    SetupTransaction,
    InTransaction,
    OutTransaction
  };

  enum class RequestType { Standard = 0, Class = 1, Vendor = 2 };

  enum class RecipientType {
    Device = 0,
    Interface = 1,
    Endpoint = 2,
    Other = 3
  };

  constexpr SetupPacket()
      : m_bmRequestType(0),
        m_bRequest(0),
        m_wValue(0),
        m_wIndex(0),
        m_wLength(0) {}

  SetupPacket(void* buffer);
  TransactionType followingTransaction() const;
  RequestType requestType() const;
  RecipientType recipientType() const;
  int descriptorIndex();
  int descriptorType();
  uint8_t bmRequestType() { return m_bmRequestType; }
  uint8_t bRequest() { return m_bRequest; }
  uint16_t wValue() { return m_wValue; }
  uint16_t wIndex() { return m_wIndex; }
  uint16_t wLength() { return m_wLength; }

 private:
  uint8_t m_bmRequestType;
  uint8_t m_bRequest;
  uint16_t m_wValue;
  uint16_t m_wIndex;
  uint16_t m_wLength;
};

static_assert(sizeof(SetupPacket) == 8, "SetupData must be packed");

}  // namespace USB
}  // namespace Device
}  // namespace Ion

#endif

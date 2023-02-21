#include "setup_packet.h"

#include <string.h>

namespace Ion {
namespace Device {
namespace USB {

SetupPacket::SetupPacket(void* buffer) {
  memcpy(this, buffer, sizeof(SetupPacket));
}

SetupPacket::TransactionType SetupPacket::followingTransaction() const {
  if (m_wLength == 0 || (m_bmRequestType & 0b10000000) != 0) {
    return TransactionType::InTransaction;
  } else {
    return TransactionType::OutTransaction;
  }
}

SetupPacket::RequestType SetupPacket::requestType() const {
  return (RequestType)((m_bmRequestType & 0b01100000) >> 5);
}

SetupPacket::RecipientType SetupPacket::recipientType() const {
  return (RecipientType)(m_bmRequestType & 0b00001111);
}

int SetupPacket::descriptorIndex() { return m_wValue & 0xFF; }

int SetupPacket::descriptorType() { return m_wValue >> 8; }

}  // namespace USB
}  // namespace Device
}  // namespace Ion

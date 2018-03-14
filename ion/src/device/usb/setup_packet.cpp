#include "setup_packet.h"
#include <string.h>

namespace Ion {
namespace USB {
namespace Device {

SetupPacket::SetupPacket(void * buffer) {
  memcpy(this, buffer, sizeof(SetupPacket));
}

SetupPacket::TransactionType SetupPacket::followingTransaction() {
  if (m_wLength == 0 || (m_bmRequestType & 0x80) != 0) {
    return TransactionType::InTransaction;
  } else {
    return TransactionType::OutTransaction;
  }
}

int SetupPacket::descriptorIndex() {
  return m_wValue & 0xFF;
}

int SetupPacket::descriptorType() {
  return m_wValue >> 8;
}

}
}
}

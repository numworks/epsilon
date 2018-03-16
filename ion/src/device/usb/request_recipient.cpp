#include "request_recipient.h"

namespace Ion {
namespace USB {
namespace Device {

bool RequestRecipient::processSetupRequest(SetupPacket * request, uint8_t * transferBuffer, uint16_t * transferBufferLength, uint16_t transferBufferMaxLength) {
  if (request->followingTransaction() == SetupPacket::TransactionType::InTransaction) {
  // There is no data stage in this transaction, or the data stage will be in IN direction.
    if (!processSetupInRequest(request, transferBuffer, transferBufferLength, transferBufferMaxLength)) {
      m_ep0->stallTransaction();
      return false;
    }
    if (*transferBufferLength > 0) {
      // TODO: Update Endpoint0 state ???
      m_ep0->computeZeroLengthPacketNeeded();
    } else {
      m_ep0->setState(Endpoint0::State::StatusIn);
    }
    m_ep0->sendSomeData();
  } else {
    // The following transaction will be an OUT transaction.
    m_ep0->clearForOutTransactions(request->wLength());
  }
  return true;
}

}
}
}

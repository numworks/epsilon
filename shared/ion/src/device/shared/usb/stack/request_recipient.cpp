#include "request_recipient.h"

namespace Ion {
namespace Device {
namespace USB {

bool RequestRecipient::processSetupRequest(SetupPacket* request,
                                           uint8_t* transferBuffer,
                                           uint16_t* transferBufferLength,
                                           uint16_t transferBufferMaxLength) {
  if (request->followingTransaction() ==
      SetupPacket::TransactionType::InTransaction) {
    // There is no data stage in this transaction, or the data stage will be in
    // IN direction.
    if (!processSetupInRequest(request, transferBuffer, transferBufferLength,
                               transferBufferMaxLength)) {
      m_ep0->stallTransaction();
      return false;
    }
    if (*transferBufferLength > 0) {
      m_ep0->computeZeroLengthPacketNeeded();
      m_ep0->sendSomeData();
    } else {
      m_ep0->sendSomeData();
      // On seeing a zero length packet, sendSomeData changed endpoint0 state to
      // LastDataIn, but it should be StatusIn as there was no data stage.
      m_ep0->setState(Endpoint0::State::StatusIn);
    }
  } else {
    // The following transaction will be an OUT transaction.
    m_ep0->clearForOutTransactions(request->wLength());
  }
  return true;
}

}  // namespace USB
}  // namespace Device
}  // namespace Ion

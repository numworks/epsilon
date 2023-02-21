#ifndef ION_DEVICE_SHARED_USB_ENDPOINT0_H
#define ION_DEVICE_SHARED_USB_ENDPOINT0_H

#include "setup_packet.h"

namespace Ion {
namespace Device {
namespace USB {

class RequestRecipient;

class Endpoint0 {
 public:
  enum class State {
    Idle,
    Stalled,
    DataIn,
    LastDataIn,
    StatusIn,
    DataOut,
    LastDataOut,
    StatusOut,
  };

  constexpr static int k_maxPacketSize = 64;
  constexpr static uint16_t MaxTransferSize = 2048;

  constexpr Endpoint0(RequestRecipient* device, RequestRecipient* interface)
      : m_forceNAK(false),
        m_bufferOffset(0),
        m_transferBufferLength(0),
        m_receivedPacketSize(0),
        m_zeroLengthPacketNeeded(false),
        m_requestRecipients{device, interface},
        m_state(State::Idle),
        m_largeBuffer{0} {}
  void setup();
  void setupOut();
  void setOutNAK(bool nak);
  void enableOut();
  void reset();
  bool NAKForced() const { return m_forceNAK; }
  void readAndDispatchSetupPacket();
  void processINpacket();
  void processOUTpacket();
  void flushTxFifo();
  void flushRxFifo();
  void setReceivedPacketSize(uint16_t size) { m_receivedPacketSize = size; }
  void discardUnreadData();
  void stallTransaction();
  void computeZeroLengthPacketNeeded();
  void setState(State state) { m_state = state; }
  void sendSomeData();  // Writes the next data packet and updates the state.
  void clearForOutTransactions(uint16_t wLength);

 private:
  int receiveSomeData();
  uint16_t readPacket(void* buffer, uint16_t length);
  uint16_t writePacket(const void* buffer, uint16_t length);

  bool m_forceNAK;
  // When sending large data stored in the buffer, the offset keeps tracks of
  // which data packet should be sent next.
  int m_bufferOffset;
  uint16_t m_transferBufferLength;
  uint16_t m_receivedPacketSize;
  bool m_zeroLengthPacketNeeded;
  SetupPacket m_request;
  RequestRecipient* m_requestRecipients[2];
  State m_state;
  uint8_t m_largeBuffer[MaxTransferSize];
};

}  // namespace USB
}  // namespace Device
}  // namespace Ion

#endif

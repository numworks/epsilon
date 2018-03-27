#ifndef ION_DEVICE_USB_ENDPOINT0_H
#define ION_DEVICE_USB_ENDPOINT0_H

#include "setup_packet.h"

namespace Ion {
namespace USB {
namespace Device {

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
  constexpr static int MaxTransferSize = 2048;

  //constexpr Endpoint0(RequestRecipient * device, RequestRecipient * interface) :
  Endpoint0(RequestRecipient * device, RequestRecipient * interface) :
    m_forceNAK(false),
    m_bufferOffset(0),
    m_transferBufferLength(0),
    m_receivedPacketSize(0),
    m_zeroLengthPacketNeeded(false),
    m_request(),
    m_requestRecipients{device, interface},
    m_state(State::Idle),
    m_largeBuffer{0},
    m_largeBufferDEBUG{0},
    m_bufferIndex(8)
  {
     m_largeBufferDEBUG[0] = 'D';
     m_largeBufferDEBUG[1] = 'E';
     m_largeBufferDEBUG[2] = 'B';
     m_largeBufferDEBUG[3] = 'U';
     m_largeBufferDEBUG[4] = 'G';
     m_largeBufferDEBUG[5] = 'L';
     m_largeBufferDEBUG[6] = 'E';
     m_largeBufferDEBUG[7] = 'A';
     m_largeBufferDEBUG[1015] = 'E';
     m_largeBufferDEBUG[1016] = 'N';
     m_largeBufferDEBUG[1017] = 'D';
     m_largeBufferDEBUG[1018] = 'B';
     m_largeBufferDEBUG[1019] = 'U';
     m_largeBufferDEBUG[1020] = 'F';
     m_largeBufferDEBUG[1021] = 'F';
     m_largeBufferDEBUG[1022] = 'E';
     m_largeBufferDEBUG[1023] = 'R';
  }
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
  void sendSomeData(); // Write a chunk of data in the TxFifo.
  void clearForOutTransactions(uint16_t wLength);

private:
  constexpr static int k_largeBufferDEBUGLength = 1024;

  uint16_t receiveSomeData();
  uint16_t readPacket(void * buffer, uint16_t length);
  uint16_t writePacket(const void * buffer, uint16_t length);

  bool m_forceNAK;
  int m_bufferOffset; // When sending large data stored in the buffer, the offset keeps tracks of which data packet should be sent next.
  uint16_t m_transferBufferLength;
  uint16_t m_receivedPacketSize;
  bool m_zeroLengthPacketNeeded;
  SetupPacket m_request;
  RequestRecipient * m_requestRecipients[2];
  State m_state;
  uint8_t m_largeBuffer[MaxTransferSize];
  char m_largeBufferDEBUG[1024];
  int m_bufferIndex;
};

}
}
}

#endif

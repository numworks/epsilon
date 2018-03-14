#ifndef ION_DEVICE_USB_ENDPOINT0_H
#define ION_DEVICE_USB_ENDPOINT0_H

#include "setup_packet.h"

namespace Ion {
namespace USB {
namespace Device {

class Device;

class Endpoint0 {

public:
  constexpr static int k_maxPacketSize = 64;
  constexpr Endpoint0(Device * device) :
    m_forceNAK(false),
    m_bufferOffset(0),
    m_transferBufferLength(0),
    m_receivedPacketSize(0),
    m_zeroLengthPacketNeeded(false),
    m_device(device),
    m_request(),
    m_state(State::Idle),
    m_largeBuffer{0}
  {
  }
  void setup();
  void setupOut();
  void setOutNAK(bool nak);
  void enableOut();
  void reset();
  bool NAKForced() const { return m_forceNAK; }
  void processSETUPpacket();
  void processINpacket();
  void processOUTpacket();
  void flushTxFifo();
  void flushRxFifo();
  void setReceivedPacketSize(uint16_t size) { m_receivedPacketSize = size; }
  void discardUnreadData();

private:
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

  // USB Standard Request Codes
  constexpr static uint8_t k_requestGetStatus       = 0;
  constexpr static uint8_t k_requestClearFeature    = 1;
  constexpr static uint8_t k_requestSetFeature      = 3;
  constexpr static uint8_t k_requestSetAddress      = 5;
  constexpr static uint8_t k_requestGetDescriptor   = 6;
  constexpr static uint8_t k_requestSetDescriptor   = 7;
  constexpr static uint8_t k_requestGetConfiguration = 8;
  constexpr static uint8_t k_requestSetConfiguration = 9;
  constexpr static uint8_t k_requestGetInterface    = 10;
  constexpr static uint8_t k_requestSetInterface    = 11;
  constexpr static uint8_t k_requestSetSynchFrame   = 12;

  constexpr static int k_largeBufferLength = 2048;

  bool processSETUPInRequest();
  void sendSomeData(); // Write a chunk of data in the TxFifo.
  uint16_t receiveSomeData();
  uint16_t readPacket(void * buffer, uint16_t length);
  uint16_t writePacket(const void * buffer, uint16_t length);
  void computeZeroLengthPacketNeeded();
  void stallTransaction();

  bool m_forceNAK;
  int m_bufferOffset; // When sending large data stored in the buffer, the offset keeps tracks of which data packet should be sent next.
  uint16_t m_transferBufferLength;
  uint16_t m_receivedPacketSize;
  bool m_zeroLengthPacketNeeded;
  Device * m_device;
  SetupPacket m_request;
  State m_state;
  uint8_t m_largeBuffer[2048];
};

}
}
}

#endif

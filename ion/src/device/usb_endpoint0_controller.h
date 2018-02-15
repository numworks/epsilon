#ifndef ION_DEVICE_USB_ENDPOINT0_CONTROLLER_H
#define ION_DEVICE_USB_ENDPOINT0_CONTROLLER_H

#include "ion.h"

namespace Ion {
namespace USB {
namespace Device {

class USBDevice;

class Endpoint0Controller {
public:
  constexpr static int k_maxPacketSize = 64;
  Endpoint0Controller();
  void setup();
  void setupOut();
  void reset();
  void enableOUT(bool shouldEnable);
  void sendChunk(USBDevice * device);
  int receiveChunk(USBDevice * device);
  uint16_t writePacket(USBDevice * device, const void * buffer, uint16_t length);
  uint16_t readPacket(USBDevice * device, void * buffer, uint16_t length);
  void stallTransaction(USBDevice * device);
  void setOutNAK(bool nak);
  bool forceNAK() const { return m_forceNAK; }
  void flushTxFifo();
  void flushRxFifo();
private:
  bool m_forceNAK;
};

}
}
}

#endif

#ifndef ION_DEVICE_USB_INTERFACE_H
#define ION_DEVICE_USB_INTERFACE_H

#include "setup_packet.h"

namespace Ion {
namespace USB {
namespace Device {

class Interface {
public:
  virtual bool processSetupRequest(SetupPacket * request, uint8_t * transferBuffer, uint16_t * transferBufferLength, uint16_t transferBufferMaxLength) {
    return false;
  }
  virtual void wholeDataReceivedCallback() {
  }
};

}
}
}

#endif

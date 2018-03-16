#ifndef ION_DEVICE_USB_REQUEST_RECIPIENT_H
#define ION_DEVICE_USB_REQUEST_RECIPIENT_H

#include "endpoint0.h"
#include "setup_packet.h"

namespace Ion {
namespace USB {
namespace Device {

class RequestRecipient {
public:
  RequestRecipient(Endpoint0 * ep0):
    m_ep0(ep0)
  {
  }
  bool processSetupRequest(SetupPacket * request, uint8_t * transferBuffer, uint16_t * transferBufferLength, uint16_t transferBufferMaxLength);
protected:
  virtual bool processSetupInRequest(SetupPacket * request, uint8_t * transferBuffer, uint16_t * transferBufferLength, uint16_t transferBufferMaxLength) = 0;
  Endpoint0 * m_ep0;
};

}
}
}

#endif

#ifndef ION_DEVICE_SHARED_USB_REQUEST_RECIPIENT_H
#define ION_DEVICE_SHARED_USB_REQUEST_RECIPIENT_H

#include "endpoint0.h"
#include "setup_packet.h"

namespace Ion {
namespace Device {
namespace USB {

class RequestRecipient {
 public:
  RequestRecipient(Endpoint0* ep0) : m_ep0(ep0) {}
  bool processSetupRequest(SetupPacket* request, uint8_t* transferBuffer,
                           uint16_t* transferBufferLength,
                           uint16_t transferBufferMaxLength);
  virtual void wholeDataReceivedCallback(SetupPacket* request,
                                         uint8_t* transferBuffer,
                                         uint16_t* transferBufferLength) {}
  virtual void wholeDataSentCallback(SetupPacket* request,
                                     uint8_t* transferBuffer,
                                     uint16_t* transferBufferLength) {}

 protected:
  virtual bool processSetupInRequest(SetupPacket* request,
                                     uint8_t* transferBuffer,
                                     uint16_t* transferBufferLength,
                                     uint16_t transferBufferMaxLength) = 0;
  Endpoint0* m_ep0;
};

}  // namespace USB
}  // namespace Device
}  // namespace Ion

#endif

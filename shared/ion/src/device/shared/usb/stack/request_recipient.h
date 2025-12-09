#pragma once

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
  // Triggered between LastDataOut and StatusOut
  virtual void wholeDataReceivedCallback(SetupPacket* request,
                                         uint8_t* transferBuffer,
                                         uint16_t* transferBufferLength) {}
  // Triggered between LastDataIn and StatusIn
  virtual void wholeDataSentCallback(SetupPacket* request,
                                     uint8_t* transferBuffer,
                                     uint16_t* transferBufferLength) {}
  // Triggered after StatusOut or StatusIn
  virtual void idleCallback(SetupPacket* request) {}

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

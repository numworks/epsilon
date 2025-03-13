#include "device.h"

#include <assert.h>

#include <algorithm>

namespace Ion {
namespace Device {
namespace USB {

void Device::flushFIFOs() { m_ep0.reset(); }

bool Device::processSetupInRequest(SetupPacket* request,
                                   uint8_t* transferBuffer,
                                   uint16_t* transferBufferLength,
                                   uint16_t transferBufferMaxLength) {
  // Device only handles standard requests.
  if (request->requestType() != SetupPacket::RequestType::Standard) {
    return false;
  }
  switch (request->bRequest()) {
    case (int)Request::GetStatus:
      return getStatus(transferBuffer, transferBufferLength,
                       transferBufferMaxLength);
    case (int)Request::SetAddress:
      // Make sure the request is address is valid.
      assert(request->wValue() < 128);
      /* According to the reference manual, the address should be set after the
       * Status stage of the current transaction, but this is not true.
       * It should be set here, after the Data stage. */
      setAddress(request->wValue());
      *transferBufferLength = 0;
      return true;
    case (int)Request::GetDescriptor:
      return getDescriptor(request, transferBuffer, transferBufferLength,
                           transferBufferMaxLength);
    case (int)Request::SetConfiguration:
      *transferBufferLength = 0;
      return setConfiguration(request);
    case (int)Request::GetConfiguration:
      return getConfiguration(transferBuffer, transferBufferLength);
  }
  return false;
}

bool Device::getStatus(uint8_t* transferBuffer, uint16_t* transferBufferLength,
                       uint16_t transferBufferMaxLength) {
  *transferBufferLength = std::min<uint16_t>(2, transferBufferMaxLength);
  for (int i = 0; i < *transferBufferLength; i++) {
    transferBuffer[i] = 0;  // No remote wakeup, not self-powered.
  }
  return true;
}

bool Device::getDescriptor(SetupPacket* request, uint8_t* transferBuffer,
                           uint16_t* transferBufferLength,
                           uint16_t transferBufferMaxLength) {
  Descriptor* wantedDescriptor =
      descriptor(request->descriptorType(), request->descriptorIndex());
  if (wantedDescriptor == nullptr) {
    return false;
  }
  *transferBufferLength =
      wantedDescriptor->copy(transferBuffer, transferBufferMaxLength);
  return true;
}

bool Device::getConfiguration(uint8_t* transferBuffer,
                              uint16_t* transferBufferLength) {
  *transferBufferLength = 1;
  transferBuffer[0] = getActiveConfiguration();
  return true;
}

bool Device::setConfiguration(SetupPacket* request) {
  // We support one configuration only
  setActiveConfiguration(request->wValue());
  /* There is one configuration only, we no need to set it again, just reset the
   * endpoint. */
  m_ep0.reset();
  return true;
}

}  // namespace USB
}  // namespace Device
}  // namespace Ion

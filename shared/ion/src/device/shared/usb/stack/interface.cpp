#include "interface.h"

namespace Ion {
namespace Device {
namespace USB {

static inline uint16_t minUint16T(uint16_t x, uint16_t y) {
  return x < y ? x : y;
}

bool Interface::processSetupInRequest(SetupPacket* request,
                                      uint8_t* transferBuffer,
                                      uint16_t* transferBufferLength,
                                      uint16_t transferBufferMaxLength) {
  if (request->requestType() != SetupPacket::RequestType::Standard) {
    return false;
  }
  switch (request->bRequest()) {
    case (int)Request::GetStatus:
      return getStatus(transferBuffer, transferBufferLength,
                       transferBufferMaxLength);
    case (int)Request::SetInterface:
      return setInterface(request, transferBufferLength);
    case (int)Request::GetInterface:
      return getInterface(transferBuffer, transferBufferLength,
                          transferBufferMaxLength);
    case (int)Request::ClearFeature:
      return clearFeature(transferBufferLength);
    case (int)Request::SetFeature:
      return setFeature(transferBufferLength);
  }
  return false;
}

bool Interface::getStatus(uint8_t* transferBuffer,
                          uint16_t* transferBufferLength,
                          uint16_t transferBufferMaxLength) {
  *transferBufferLength = minUint16T(2, transferBufferMaxLength);
  for (int i = 0; i < *transferBufferLength; i++) {
    transferBuffer[i] = 0;  // Reserved, must be set to 0
  }
  return true;
}

bool Interface::getInterface(uint8_t* transferBuffer,
                             uint16_t* transferBufferLength,
                             uint16_t transferBufferMaxLength) {
  *transferBufferLength = minUint16T(1, transferBufferMaxLength);
  if (*transferBufferLength > 0) {
    transferBuffer[0] = getActiveInterfaceAlternative();
  }
  return true;
}

bool Interface::setInterface(SetupPacket* request,
                             uint16_t* transferBufferLength) {
  // We support one interface only
  setActiveInterfaceAlternative(request->wValue());
  // There is one interface alternative only, we no need to set it again.
  *transferBufferLength = 0;
  return true;
}

bool Interface::clearFeature(uint16_t* transferBufferLength) {
  // Not needed for now
  *transferBufferLength = 0;
  return true;
}

bool Interface::setFeature(uint16_t* transferBufferLength) {
  // Not needed for now
  *transferBufferLength = 0;
  return true;
}

}  // namespace USB
}  // namespace Device
}  // namespace Ion

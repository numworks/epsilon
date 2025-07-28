#ifndef ION_DEVICE_SHARED_USB_INTERFACE_H
#define ION_DEVICE_SHARED_USB_INTERFACE_H

#include "endpoint0.h"
#include "request_recipient.h"
#include "setup_packet.h"

namespace Ion {
namespace Device {
namespace USB {

class Interface : public RequestRecipient {
 public:
  Interface(Endpoint0* ep0) : RequestRecipient(ep0) {}

 protected:
  virtual void setActiveInterfaceAlternative(
      uint8_t interfaceAlternativeIndex) = 0;
  virtual uint8_t getActiveInterfaceAlternative() = 0;
  bool processSetupInRequest(SetupPacket* request, uint8_t* transferBuffer,
                             uint16_t* transferBufferLength,
                             uint16_t transferBufferMaxLength) override;

 private:
  // USB Standard Interface Request Codes
  enum class Request {
    GetStatus = 0,
    ClearFeature = 1,
    SetFeature = 3,
    GetInterface = 10,
    SetInterface = 11,
  };
  bool getStatus(uint8_t* transferBuffer, uint16_t* transferBufferLength,
                 uint16_t transferBufferMaxLength);
  bool getInterface(uint8_t* transferBuffer, uint16_t* transferBufferLength,
                    uint16_t transferBufferMaxLength);
  bool setInterface(SetupPacket* request, uint16_t* transferBufferLength);
  bool clearFeature(uint16_t* transferBufferLength);
  bool setFeature(uint16_t* transferBufferLength);
};

}  // namespace USB
}  // namespace Device
}  // namespace Ion

#endif

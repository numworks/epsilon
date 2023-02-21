#ifndef ION_DEVICE_SHARED_USB_DEVICE_H
#define ION_DEVICE_SHARED_USB_DEVICE_H

#include "descriptor/descriptor.h"
#include "endpoint0.h"
#include "interface.h"
#include "request_recipient.h"
#include "setup_packet.h"

namespace Ion {
namespace Device {
namespace USB {

// We only handle control transfers, on EP0.
class Device : public RequestRecipient {
 public:
  Device(Interface* interface)
      : RequestRecipient(&m_ep0),
        m_ep0(this, interface),
        m_resetOnDisconnect(false) {}
  void poll();
  bool isSoftDisconnected() const;
  void detach();
  virtual void leave(uint32_t leaveAddress) = 0;
  bool resetOnDisconnect() { return m_resetOnDisconnect; }
  void setResetOnDisconnect(bool reset) { m_resetOnDisconnect = reset; }

 protected:
  virtual Descriptor* descriptor(uint8_t type, uint8_t index) = 0;
  virtual void setActiveConfiguration(uint8_t configurationIndex) = 0;
  virtual uint8_t getActiveConfiguration() = 0;
  bool processSetupInRequest(SetupPacket* request, uint8_t* transferBuffer,
                             uint16_t* transferBufferLength,
                             uint16_t transferBufferMaxLength) override;
  Endpoint0 m_ep0;

 private:
  // USB Standard Device Request Codes
  enum class Request {
    GetStatus = 0,
    ClearFeature = 1,
    SetFeature = 3,
    SetAddress = 5,
    GetDescriptor = 6,
    SetDescriptor = 7,
    GetConfiguration = 8,
    SetConfiguration = 9,
  };

  enum class TransactionType { Setup, In, Out };

  void setAddress(uint8_t address);
  bool getStatus(uint8_t* transferBuffer, uint16_t* transferBufferLength,
                 uint16_t transferBufferMaxLength);
  bool getDescriptor(SetupPacket* request, uint8_t* transferBuffer,
                     uint16_t* transferBufferLength,
                     uint16_t transferBufferMaxLength);
  bool getConfiguration(uint8_t* transferBuffer,
                        uint16_t* transferBufferLength);
  bool setConfiguration(SetupPacket* request);

  bool m_resetOnDisconnect;
};

}  // namespace USB
}  // namespace Device
}  // namespace Ion

#endif

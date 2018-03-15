#ifndef ION_DEVICE_USB_DEVICE_H
#define ION_DEVICE_USB_DEVICE_H

#include "stack/descriptor.h"
#include "endpoint0.h"
#include "setup_packet.h"

namespace Ion {
namespace USB {
namespace Device {

/* We only handle control transfers, on EP0 then. */
class Device {

public:
  constexpr Device(Interface * interface) :
    m_ep0(this, interface)
  {
  }

  void init();
  void shutdown();
  void poll();

  //virtual bool controlTransfer(struct usb_setup_data *req, uint8_t **buf, uint16_t *len);/*, void (**complete)(usbd_device *usbd_dev, struct usb_setup_data *req));*/

  virtual bool processSetupRequest(SetupPacket * request, uint8_t * transferBuffer, uint16_t * transferBufferLength, uint16_t transferBufferMaxLength);

protected:
  virtual Descriptor * descriptor(uint8_t type, uint8_t index) = 0;
  virtual void setActiveConfiguration(uint8_t configurationIndex) = 0;
  virtual uint8_t getActiveConfiguration() = 0;
  Endpoint0 m_ep0;
private:

  // USB Standard Request Codes
  enum class Request {
    GetStatus       = 0,
    ClearFeature    = 1,
    SetFeature      = 3,
    SetAddress      = 5,
    GetDescriptor   = 6,
    SetDescriptor   = 7,
    GetConfiguration = 8,
    SetConfiguration = 9,
    GetInterface    = 10,
    SetInterface    = 11,
    SetSynchFrame   = 12
  };

  enum class TransactionType {
    Setup,
    In,
    Out
  };

  bool processSetupInRequest(SetupPacket * request, uint8_t * transferBuffer, uint16_t * transferBufferLength, uint16_t transferBufferMaxLength);
  void setAddress(uint8_t address);
  bool getStatus(uint8_t * transferBuffer, uint16_t * transferBufferLength);
  bool getDescriptor(SetupPacket * request, uint8_t * transferBuffer, uint16_t * transferBufferLength, uint16_t transferBufferMaxLength);
  bool getConfiguration(uint8_t * transferBuffer, uint16_t * transferBufferLength);
  bool setConfiguration(SetupPacket * request);
};

}
}
}

#endif

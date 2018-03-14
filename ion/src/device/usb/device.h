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
  constexpr Device() :
    m_ep0(this)
  {
  }

  void init();
  void shutdown();
  void poll();

  //virtual bool controlTransfer(struct usb_setup_data *req, uint8_t **buf, uint16_t *len);/*, void (**complete)(usbd_device *usbd_dev, struct usb_setup_data *req));*/

  void setAddress(uint8_t address);
  bool getStatus(uint8_t * transferBuffer, uint16_t * transferBufferLength);
  bool getDescriptor(SetupPacket * request, uint8_t * transferBuffer, uint16_t * transferBufferLength, uint16_t transferBufferMaxLength);
  bool setConfiguration(SetupPacket * request);

protected:
  virtual Descriptor * descriptor(uint8_t type, uint8_t index) = 0;

private:

  enum class TransactionType {
    Setup,
    In,
    Out
  };

  //virtual bool processSetupRequest(SetupPacket * request, uint8_t * transferBuffer, uint16_t * transferBufferLength, uint16_t * transferBufferMaxLength);
  //bool processInSetupRequest(SetupPacket request, uint8_t * transferBuffer, uint16_t * transferBufferLength, uint16_t * transferBufferMaxLength);
  Endpoint0 m_ep0;
};

}
}
}

#endif

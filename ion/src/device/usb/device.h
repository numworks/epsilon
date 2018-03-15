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

  virtual void processSetupRequest(SetupPacket * request, uint8_t * transferBuffer, uint16_t * transferBufferLength, uint16_t transferBufferMaxLength);

protected:
  virtual Descriptor * descriptor(uint8_t type, uint8_t index) = 0;

private:

  // USB Standard Request Codes
  constexpr static uint8_t k_requestGetStatus       = 0;
  constexpr static uint8_t k_requestClearFeature    = 1;
  constexpr static uint8_t k_requestSetFeature      = 3;
  constexpr static uint8_t k_requestSetAddress      = 5;
  constexpr static uint8_t k_requestGetDescriptor   = 6;
  constexpr static uint8_t k_requestSetDescriptor   = 7;
  constexpr static uint8_t k_requestGetConfiguration = 8;
  constexpr static uint8_t k_requestSetConfiguration = 9;
  constexpr static uint8_t k_requestGetInterface    = 10;
  constexpr static uint8_t k_requestSetInterface    = 11;
  constexpr static uint8_t k_requestSetSynchFrame   = 12;

  enum class TransactionType {
    Setup,
    In,
    Out
  };

  bool processSetupInRequest(SetupPacket * request, uint8_t * transferBuffer, uint16_t * transferBufferLength, uint16_t transferBufferMaxLength);
  void setAddress(uint8_t address);
  bool getStatus(uint8_t * transferBuffer, uint16_t * transferBufferLength);
  bool getDescriptor(SetupPacket * request, uint8_t * transferBuffer, uint16_t * transferBufferLength, uint16_t transferBufferMaxLength);
  bool setConfiguration(SetupPacket * request);

  Endpoint0 m_ep0;
};

}
}
}

#endif

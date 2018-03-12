#ifndef ION_DEVICE_USB_DEVICE_H
#define ION_DEVICE_USB_DEVICE_H

#include "regs/regs.h"
#include "ion.h"

namespace Ion {
namespace USB {


/*  Pin | Role              | Mode                  | Function
 * -----+-------------------+-----------------------+----------
 *  PA9 | VBUS              | Input, pulled down//TODO    | Low = unplugged, high = plugged
 * PA11 | USB D-            | Alternate Function 10 |
 * PA12 | USB D+            | Alternate Function 10 |
 */

constexpr static GPIOPin VbusPin = GPIOPin(GPIOA, 9);
constexpr static GPIOPin DmPin = GPIOPin(GPIOA, 11);
constexpr static GPIOPin DpPin = GPIOPin(GPIOA, 12);


/* We only handle control transfers, on EP0 then. */

class Device {
public:
  Device(uint16_t vid, uint16_t pid, const char * serialNumber, const char * vendor, tableau_de_device_capabilities, les_configurations /* e.g. DFU, HID, MSC, etc... */);
  void init();
  void shutdown();
  void poll();

  virtual bool controlTransfer(struct usb_setup_data *req, uint8_t **buf, uint16_t *len);/*, void (**complete)(usbd_device *usbd_dev, struct usb_setup_data *req));*/

private:
  void initGPIO();
  uint16_t m_vid;
  uint16_t m_pid;
  const char * m_serialNumber;
  const char * m_vendor;
  Endpoint0 m_ep0;
};

}
}

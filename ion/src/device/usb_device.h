#ifndef ION_DEVICE_USB_USB_DEVICE_H
#define ION_DEVICE_USB_USB_DEVICE_H

#include "regs/regs.h"
#include "ion.h"
#include "usb_control_transfer_controller.h"
#include "usb_endpoint0_controller.h"

namespace Ion {
namespace USB {
namespace Device {

class USBDevice {
public:
  USBDevice();
  void init();
  uint16_t receivedPacketSize() { return m_receivedPacketSize; }
  void setReceivedPacketSize(uint16_t size) { m_receivedPacketSize = size; }
  void setAddress(uint8_t address);
  ControlTransferController * controlTransferController() { return &m_controlTransferController; }
private:
  enum class USBTransaction {
    In,
    Out,
    Setup
  };
  void poll();
  Endpoint0Controller m_endpoint0Controller;
  ControlTransferController m_controlTransferController;
  uint16_t m_receivedPacketSize;
};

}
}
}

#endif

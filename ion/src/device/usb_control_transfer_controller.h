#ifndef ION_DEVICE_USB_CONTROL_TRANSFER_CONTROLLER_H
#define ION_DEVICE_USB_CONTROL_TRANSFER_CONTROLLER_H

#include "regs/regs.h"
#include "ion.h"
#include "usb_data_structures.h"
#include "usb_windows_os_string_delegate.h"
#include "usb_web_usb_delegate.h"

namespace Ion {
namespace USB {
namespace Device {

class USBDevice;
class Endpoint0Controller;

class ControlTransferController {
public:
  enum class State {
    Idle,
    Stalled,
    DataIn,
    LastDataIn,
    StatusIn,
    DataOut,
    LastDataOut,
    StatusOut,
  };

  ControlTransferController(USBDevice * usbDevice, Endpoint0Controller * endpoint0Controller);

  // Transaction callbacks
  void didFinishControlSetupTransaction();
  void didFinishControlInTransaction();
  void didFinishControlOutTransaction();

  // Data buffer
  uint8_t * controlTransferBuffer() { return m_transferBuffer; }
  void setControlTransferBuffer(uint8_t * buffer);
  uint16_t controlTransferBufferLength() { return m_transferBufferLength; }
  void setControlTransferBufferLength(uint16_t length);

  // Transfer state
  void setState(State state);

  // Setup Data
  SetupData setupData() { return m_setupData; }

  // IN data information
  bool zeroLengthPacketNeeded() { return m_zeroLengthPacketNeeded; }
  bool computeZeroLengthPacketNeeded(uint16_t dataLength, uint16_t dataExpectedLength, uint8_t endpointMaxPacketSize);
  void setZeroLengthPacketNeeded(bool isNeeded);

  // OUT data information
  uint16_t receivedPacketSize() const { return m_receivedPacketSize; }
  void setReceivedPacketSize(uint16_t size);

private:
  static constexpr uint16_t k_setupDataLength = 8;
  static constexpr uint16_t k_transferBufferInitLength = 5*64; //TODO why 5*64? (from libopencm3/tests/gadget-zero/usb-gadget0.c)

  void didFinishControlSetupBeforeInTransaction();
  void didFinishControlSetupBeforeOutTransaction();
  int getDescriptor();
  uint16_t buildConfigDescriptor(uint8_t index);
  int setConfiguration();
  int getStatus();
  int controlRequestDispatch();
  int controlStandardRequest();
  int controlCustomSetup();
  State m_state;
  USBDevice * m_usbDevice;
  Endpoint0Controller * m_endpoint0Controller;
  SetupData m_setupData;
  uint16_t m_receivedPacketSize;
  uint8_t * m_transferBuffer; // Pointer to the buffer used for control transactions.
  uint16_t m_transferBufferLength;
  uint8_t m_transferBufferInit[k_transferBufferInitLength]; // Buffer used to receive data from the host or to build data to send to the host.
  bool m_zeroLengthPacketNeeded;
  WindowsOSStringDelegate m_windowsOSStringDelegate;
  WebUSBDelegate m_webUSBDelegate;
};

}
}
}

#endif

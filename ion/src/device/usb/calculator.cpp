#include "calculator.h"
#include "../regs/regs.h"
#include "../keyboard.h"
#include <ion/usb.h>

namespace Ion {
namespace USB {
namespace Device {

bool Calculator::PollAndReset(bool exitWithKeyboard) {
  char serialNumber[Ion::SerialNumberLength+1];
  Ion::getSerialNumber(serialNumber);
  Calculator c(serialNumber);

  /* Leave DFU mode if the Back key is pressed, the calculator unplugged or the
   * USB core soft-disconnected. */
  Ion::Keyboard::Key exitKey = Ion::Keyboard::Key::A6;
  uint8_t exitKeyRow = Ion::Keyboard::Device::rowForKey(exitKey);
  uint8_t exitKeyColumn = Ion::Keyboard::Device::columnForKey(exitKey);

  Ion::Keyboard::Device::activateRow(exitKeyRow);

  while (!(exitWithKeyboard && Ion::Keyboard::Device::columnIsActive(exitKeyColumn)) &&
      Ion::USB::isPlugged() &&
      !c.isSoftDisconnected()) {
    c.poll();
  }
  if (!c.isSoftDisconnected()) {
    c.detach();
  }
  return c.resetOnDisconnect();
}

Descriptor * Calculator::descriptor(uint8_t type, uint8_t index) {
  /* Special case: Microsoft OS String Descriptor should be returned when
   * searching for string descriptor at index 0xEE. */
  if (type == m_microsoftOSStringDescriptor.type() && index == 0xEE) {
    return &m_microsoftOSStringDescriptor;
  }
  int typeCount = 0;
  for (size_t i=0; i<sizeof(m_descriptors)/sizeof(m_descriptors[0]); i++) {
    Descriptor * descriptor = m_descriptors[i];
    if (descriptor->type() != type) {
      continue;
    }
    if (typeCount == index) {
      return descriptor;
    } else {
      typeCount++;
    }
  }
  return nullptr;
}

bool Calculator::processSetupInRequest(SetupPacket * request, uint8_t * transferBuffer, uint16_t * transferBufferLength, uint16_t transferBufferMaxLength) {
  if (Device::processSetupInRequest(request, transferBuffer, transferBufferLength, transferBufferMaxLength)) {
    return true;
  }
  if (request->requestType() == SetupPacket::RequestType::Vendor) {
    if (request->bRequest() == k_webUSBVendorCode && request->wIndex() == 2) {
      // This is a WebUSB, GET_URL request
      assert(request->wValue() == k_webUSBLandingPageIndex);
      return getURLCommand(transferBuffer, transferBufferLength, transferBufferMaxLength);
    }
    if (request->bRequest() == k_microsoftOSVendorCode && request->wIndex() == 0x0004) {
      // This is a Microsoft OS descriptor, Extended Compat ID request
      assert(request->wValue() == 0);
      return getExtendedCompatIDCommand(transferBuffer, transferBufferLength, transferBufferMaxLength);
    }
  }
  return false;
}

bool Calculator::getURLCommand(uint8_t * transferBuffer, uint16_t * transferBufferLength, uint16_t transferBufferMaxLength) {
  *transferBufferLength = m_workshopURLDescriptor.copy(transferBuffer, transferBufferMaxLength);
  return true;
}

bool Calculator::getExtendedCompatIDCommand(uint8_t * transferBuffer, uint16_t * transferBufferLength, uint16_t transferBufferMaxLength) {
  *transferBufferLength = m_extendedCompatIdDescriptor.copy(transferBuffer, transferBufferMaxLength);
  return true;
}

}
}
}

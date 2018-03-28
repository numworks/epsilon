#include "calculator.h"
#include "../regs/regs.h"
#include "../keyboard.h"
#include <ion/usb.h>

namespace Ion {
namespace USB {
namespace Device {

void Calculator::Poll() {
  Calculator c;

  // Leave DFU mode when pressing the Back key, or when the calculator is
  // unplugged.
  Ion::Keyboard::Key exitKey = Ion::Keyboard::Key::A6;
  uint8_t exitKeyRow = Ion::Keyboard::Device::rowForKey(exitKey);
  uint8_t exitKeyColumn = Ion::Keyboard::Device::columnForKey(exitKey);

  Ion::Keyboard::Device::activateRow(exitKeyRow);

  // TODO also leave on detach
  while (!(Ion::Keyboard::Device::columnIsActive(exitKeyColumn)) && Ion::USB::isPlugged()) {
    c.poll();
  }
}

Descriptor * Calculator::descriptor(uint8_t type, uint8_t index) {
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
    if (request->bRequest() == k_webUSBVendorCode) {
      // This is a WebUSB request
      assert(request->wValue() == k_webUSBLandingPageIndex);
      assert(request->wIndex() == 2); // GET_URL request
      return getURLCommand(transferBuffer, transferBufferLength, transferBufferMaxLength);
    }
  }
  return false;
}

bool Calculator::getURLCommand(uint8_t * transferBuffer, uint16_t * transferBufferLength, uint16_t transferBufferMaxLength) {
  *transferBufferLength = m_workshopURLDescriptor.copy(transferBuffer, transferBufferMaxLength);
  return true;
}

}
}
}

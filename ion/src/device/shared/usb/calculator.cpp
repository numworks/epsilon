#include "calculator.h"
#include <shared/drivers/serial_number.h>
#include <ion.h>
#include <ion/usb.h>

namespace Ion {
namespace Device {
namespace USB {

void Calculator::PollAndReset() {
  /* Don't use Ion::serialNumber to avoid any data section in the relocatable
   * dfu. */
  char serialNumber[Ion::k_serialNumberLength+1];
  SerialNumber::copy(serialNumber);
  Calculator c(serialNumber, USB::stringDescriptor());

  while (Ion::USB::isPlugged() && !c.isSoftDisconnected() && !(USB::shouldInterruptDFU() && !c.isErasingAndWriting())) {
    c.poll();
  }

  if (!c.isSoftDisconnected()) {
    c.detach();
  }
  if (c.resetOnDisconnect()) {
    c.leave(c.addressPointer());
  }
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

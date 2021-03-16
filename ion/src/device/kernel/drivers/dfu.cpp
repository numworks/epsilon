#include <shared/usb/calculator.h>
#include <drivers/keyboard.h>
#include <drivers/serial_number.h>
#include <drivers/usb.h>
#include <ion.h>

namespace Ion {
namespace Device {
namespace USB {

void Calculator::PollAndReset(bool exitWithKeyboard) {
  char serialNumber[Ion::Device::SerialNumber::Length+1];
  Ion::Device::SerialNumber::copy(serialNumber);
  Calculator c(serialNumber);

  /* Leave DFU mode if the Back key is pressed, the calculator unplugged or the
   * USB core soft-disconnected. */
  Ion::Keyboard::Key exitKey = Ion::Keyboard::Key::Back;
  uint8_t exitKeyRow = Ion::Device::Keyboard::rowForKey(exitKey);
  uint8_t exitKeyColumn = Ion::Device::Keyboard::columnForKey(exitKey);

  Ion::Device::Keyboard::activateRow(exitKeyRow);

  while (!(exitWithKeyboard && !c.isErasingAndWriting() && Ion::Device::Keyboard::columnIsActive(exitKeyColumn)) &&
      Ion::Device::USB::isPlugged() &&
      !c.isSoftDisconnected()) {
    c.poll();
  }
  if (!c.isSoftDisconnected()) {
    c.detach();
  }
  if (c.resetOnDisconnect()) {
    c.leave(c.addressPointer());
  }
}

}
}
}

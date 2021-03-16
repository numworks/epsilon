#include "calculator.h"
#include <drivers/serial_number.h>
#include <drivers/usb.h>
#include <ion.h>

namespace Ion {
namespace Device {
namespace USB {

void DFU() {
  USB::Calculator c(Ion::Device::SerialNumber::get());

  while (USB::isPlugged() && !c.isSoftDisconnected()) {
    c.poll();
  }
  if (!c.isSoftDisconnected()) {
    c.detach();
  }
  /* XIP DFU doest not jump after flashing
    if (c.resetOnDisconnect()) {
    c.leave(c.addressPointer());
  }*/
}

}
}
}

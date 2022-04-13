#include <ion/events.h>
#include <ion/usb.h>
#include <drivers/usb.h>
#include <kernel/drivers/keyboard.h>

namespace Ion {
namespace USB {

bool isPlugged() {
  return Device::USB::Config::VbusPin.group().IDR()->get(Device::USB::Config::VbusPin.pin());

}

}
}

namespace Ion {
namespace Device {
namespace USB {

Events::Event shouldInterruptDFU() {
  return Keyboard::columnIsActive(Keyboard::columnForKey(Keyboard::Key::Back)) ? Events::Back : Events::None;
}

const char * stringDescriptor() {
  return Config::InterfaceFlashStringDescriptor;
}

}
}
}

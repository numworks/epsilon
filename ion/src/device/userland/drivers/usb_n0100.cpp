#include <ion/usb.h>
#include <drivers/usb.h>
#warning This is a dependency of N0100 userland to epsilon-cor
#include <shared-core/drivers/keyboard.h>
#include <shared-core/drivers/usb.h>

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

bool shouldInterruptDFU() {
  return Keyboard::columnIsActive(Keyboard::columnForKey(Ion::Keyboard::Key::Back));
}

const char * stringDescriptor() {
  return Config::InterfaceFlashStringDescriptor;
}

}
}
}

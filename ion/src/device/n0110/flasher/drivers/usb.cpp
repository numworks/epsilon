#include <shared/drivers/usb.h>
#include <flasher/drivers/config/usb.h>

namespace Ion {
namespace Device {
namespace USB {

void willExecuteDFU() {}
void didExecuteDFU() {}
bool shouldInterruptDFU() { return false; }

const char * stringDescriptor() {
  return Config::InterfaceFlashStringDescriptor;
}

}
}
}
